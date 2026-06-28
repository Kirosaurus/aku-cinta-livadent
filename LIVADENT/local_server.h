#pragma once

#include <FS.h> 
#include <WebServer.h>
 
extern WebServer server;
 
extern void handle_root();
extern void send_cors_headers();
extern void local_server_init();