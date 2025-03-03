#pragma once

#include "av-module/logger.h"
#include "av-core/context.h"

struct CoreLogger {
private:
    CoreContext* context;
    CoreTarget   target;
    ModuleLogger logger;

    FILE* logFile;

    std::string transceiver_send_buffer;

    LogLevel storeLogLevel = LogLevel::INFO;
    LogLevel transLogLevel = LogLevel::SUCCESS;

    void tick ();
    void run ();
public:
    CoreLogger (CoreContext* context);

    pthread_t start ();

    static void* run_thread (void * context);
};

