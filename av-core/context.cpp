
#include "av-core/context.h"


void CoreContext::register_module (
    const char* module_name,
    const char* path_module_output, 
    const char* path_module_input,
    const char* path_logger
) {
    CoreModule m(module_name, path_module_output, path_module_input, path_logger);

    this->modules.push_back(m);
}

CoreModule* CoreContext::get_module (int id) {
    if (id < 0 || id >= this->modules.size()) return nullptr;
    return &modules[id];
}
int CoreContext::module_count () {
    return modules.size();
}
