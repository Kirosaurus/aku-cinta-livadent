#include <AccelStepper.h>

#define pin_step 17
#define pin_dir 18
#define pin_enable 2

AccelStepper stepper(AccelStepper::DRIVER, pin_step, pin_dir);

void setup() {
  Serial.begin(115200);
  pinMode(pin_step, OUTPUT);
  pinMode(pin_dir, OUTPUT);
  pinMode(pin_enable, OUTPUT);

  stepper.setMaxSpeed(833.4);
  stepper.setAcceleration(100);

  stepper.moveTo(20000000000);
}

void loop() {
  stepper.run();
}
