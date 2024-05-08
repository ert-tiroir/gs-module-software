#pragma once

#include "av-core/module.h"
#include "av-core/logger.h"
#include "av-core/transceiver.h"
#include "av-core/physical.h"
#include <vector>

struct CoreContext {
private:
    std::vector<CoreModule> modules;
public:
    void register_module (
        const char* module_name,
        const char* path_module_output, 
        const char* path_module_input,
        const char* path_logger
    );

    CoreModule* get_module (int id);
    int module_count ();
};
