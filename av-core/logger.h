#pragma once

#include "av-core/context.h"

struct CoreLogger {
private:
    CoreContext* context;

    void tick ();
    void run ();
public:
    CoreLogger (CoreContext* context);

    pthread_t start ();

    static void* run_thread (void * context);
};

