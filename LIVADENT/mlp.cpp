#include <stdint.h>
#include "mlp.h"

namespace {
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

constexpr int kTensorArenaSize = 8 * 1024;
// const int kTensorArenaSize = 8 * 1024;
uint8_t* tensor_arena = nullptr;
}

void input_scaler(float ph, float visc, float output[2]){
  const float ph_mid = 6.67;
  const float ph_scale = 0.3799999999999999;
  const float visc_mid = 1.183088939;
  const float visc_scale = 0.4181607460000001;

  output[0] = (ph - ph_mid) / ph_scale;
  output[1] = (visc - visc_mid) / visc_scale;
}

void mlp_init() {
  tflite::InitializeTarget();

  tensor_arena = (uint8_t*)heap_caps_aligned_alloc(
    16, kTensorArenaSize, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

  if (tensor_arena == nullptr) {
    MicroPrintf("Gagal alokasi tensor arena!");
    return;
  }
  model = tflite::GetModel(exported_model_tflite);

  if (model->version() != TFLITE_SCHEMA_VERSION) {
    MicroPrintf("Model schema version %d tidak sama dengan supported version %d",
                model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }


  static tflite::MicroMutableOpResolver<3> resolver;

  if (resolver.AddFullyConnected() != kTfLiteOk) {
    MicroPrintf("AddFullyConnected failed");
    return;
  }

  // if (resolver.AddRelu() != kTfLiteOk) {
  //   MicroPrintf("AddRelu failed");
  //   return;
  // }

  if (resolver.AddElu() != kTfLiteOk) {
    MicroPrintf("AddElu failed");
    return;
  }

  if (resolver.AddSoftmax() != kTfLiteOk) {
    MicroPrintf("AddSoftmax failed");
    return;
  }

  static tflite::MicroInterpreter static_interpreter(
    model, resolver, tensor_arena, kTensorArenaSize);

  interpreter = &static_interpreter;

  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    MicroPrintf("AllocateTensors() failed");
    return;
  }

  input = interpreter->input(0);
  output = interpreter->output(0);

  MicroPrintf("Model siap dijalankan");
  MicroPrintf("Input type: %d", input->type);
  MicroPrintf("Output type: %d", output->type);
}

uint8_t predict_test(float ph, float viskositas) {
  float input_params[2]; 
  input_scaler(ph, viskositas, input_params); 

  if (input->type == kTfLiteInt8) {
    input->data.int8[0] = static_cast<int8_t>(
      input_params[0] / input->params.scale + input->params.zero_point);

    input->data.int8[1] = static_cast<int8_t>(
      input_params[1] / input->params.scale + input->params.zero_point);
  }

  else if (input->type == kTfLiteFloat32) {
    MicroPrintf("Float32");
    Serial.println(input_params[0]);
    Serial.println(input_params[1]);
    MicroPrintf("Float32");
    input->data.f[0] = input_params[0];
    input->data.f[1] = input_params[1];
  }

  else {
    MicroPrintf("Tipe input belum didukung");
    return 3;
  }

  TfLiteStatus invoke_status = interpreter->Invoke();

  if (invoke_status != kTfLiteOk) {
    MicroPrintf("Invoke failed");
    return 3;
  }

  float skor[3];

  if (output->type == kTfLiteInt8) {
    for (int i = 0; i < 3; i++) {
      int8_t nilai_quantized = output->data.int8[i];
      skor[i] = (nilai_quantized - output->params.zero_point) * output->params.scale;
    }
  }

  else if (output->type == kTfLiteFloat32) {
    for (int i = 0; i < 3; i++) {
      skor[i] = output->data.f[i];
    }
  }

  else {
    MicroPrintf("Tipe output belum didukung");
    return 3;
  }

  int prediksi = 0;
  float skor_tertinggi = skor[0];

  for (int i = 1; i < 3; i++) {
    if (skor[i] > skor_tertinggi) {
      skor_tertinggi = skor[i];
      prediksi = i;
    }
  }

  MicroPrintf("pH: %f", static_cast<double>(ph));
  MicroPrintf("Viskositas: %f", static_cast<double>(viskositas));

  MicroPrintf("Skor rendah: %f", static_cast<double>(skor[0]));
  MicroPrintf("Skor sedang: %f", static_cast<double>(skor[1]));
  MicroPrintf("Skor tinggi: %f", static_cast<double>(skor[2]));

  if (prediksi == 0) {
    MicroPrintf("Prediksi: Risiko rendah");
  } else if (prediksi == 1) {
    MicroPrintf("Prediksi: Risiko sedang");
  } else {
    MicroPrintf("Prediksi: Risiko tinggi");
  }

  return prediksi;

  delay(1000);
}
