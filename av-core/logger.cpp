
#include "av-core/logger.h"
#include <iostream>

void CoreLogger::tick () {
    int number_modules = context->module_count();

    for (int module_id = 0; module_id < number_modules; module_id ++) {
        CoreModule* module = context->get_module(module_id);
        if (module == nullptr) continue ;
        if (!module->ready()) continue ;

        bool found;
        std::string result = module->read_string_logger(&found);
        if (!found) continue ;

        std::cout << result << std::endl;
    }
}
void CoreLogger::run () {
    while (1) {
        tick();
    }
}

CoreLogger::CoreLogger (CoreContext* context) {
    this->context = context;
}

pthread_t CoreLogger::start () {
    pthread_t thread_id;

    pthread_create( &thread_id, NULL, &CoreLogger::run_thread, this );

    return thread_id;
}

void* CoreLogger::run_thread (void * context) {
    CoreLogger* logger = (CoreLogger*) context;

    logger->run();

    return nullptr;
}
