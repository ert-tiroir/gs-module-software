#pragma once

#include "av-core/context.h"
#include "av-module/logger.h"
#include "communication/buffer.h"

struct CorePhysical {
private:
    CoreContext* context;
    CoreTarget   target;
    ModuleLogger logger;

    buffer_t* tx_buffer;
    buffer_t* rx_buffer;

    void tick ();
    void run ();
public:
    CorePhysical (CoreContext* context, buffer_t* tx_buffer, buffer_t* rx_buffer);

    pthread_t start ();

    static void* run_thread (void * context);
};


