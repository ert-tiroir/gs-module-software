
#include "av-core/logger.h"
#include "utils/consts.h"

#include <iostream>
#include <chrono>
#include <iomanip>

const char* AV_LOGGER__module_output = "/tmp/av-logger-output";
const char* AV_LOGGER__module_input  = "/tmp/av-logger-input";
const char* AV_LOGGER__logger        = "/tmp/av-logger-logger";

const char* from_log_level (LogLevel level) {
    switch (level) {
        case DEBUG:   return "DBG";
        case INFO:    return "INF";
        case SUCCESS: return "SCS";
        case WARNING: return "WRN";
        case ERROR:   return "ERR";
    }
    return "0";
}
std::string get_time () {
    auto now = std::chrono::system_clock::now();
    auto tim = std::chrono::system_clock::to_time_t(now);

    std::stringstream stream;
    stream << std::put_time(
        std::localtime(&tim),
        "%Y-%m-%d %X"
    );
    stream << ":";
    stream << (std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000).count();
    return stream.str();
}

void CoreLogger::tick () {
    int number_modules = context->module_count();

    for (int module_id = 0; module_id < number_modules; module_id ++) {
        CoreModule* module = context->get_module(module_id);
        if (module == nullptr) continue ;
        if (!module->ready()) continue ;

        bool found;
        std::string result = module->read_string_logger(&found);
        if (!found) continue ;

        LogLevel level = (LogLevel) result[0];
        std::string ts = get_time();

        // TODO print + store + forward to FO
        std::stringstream result_stream;
        result_stream << "[";
        result_stream << module->get_name();
        result_stream << "::";
        result_stream << from_log_level(level);
        result_stream << " @ ";
        result_stream << ts.c_str();
        result_stream << "] ";
        result_stream << result.substr(1, result.size() - 1);
        result_stream << "\n";
        
        std::string result_string = result_stream.str();

        printf("%s", result_string.c_str());

        if (lessOrEquals(storeLogLevel, level)) {
            fwrite(result_string.c_str(), 1, result_string.size(), this->logFile);
            fflush(this->logFile);
        }
        if (lessOrEquals(transLogLevel, level)) {
            if (transceiver_send_buffer.size() + result_string.size() >= 1016
             && transceiver_send_buffer.size() > 0) {
                target.write_string_to_core(transceiver_send_buffer);
                transceiver_send_buffer.clear();
            }

            transceiver_send_buffer.append(result_string);
        }
    }
}
void CoreLogger::run () {
    this->logger << "Joined Logger Thread" << LogLevel::SUCCESS;

    while (1) {
        tick();
    }
}

CoreLogger::CoreLogger (CoreContext* context) {
    this->context = context;

    this->context->register_module(
        "AV LOG",
        AV_LOGGER__module_output,
        AV_LOGGER__module_input,
        AV_LOGGER__logger
    );

    this->target = CoreTarget(
        AV_LOGGER__module_output,
        AV_LOGGER__module_input,
        AV_LOGGER__logger
    );
    this->logger = ModuleLogger(&this->target);

    this->logFile = fopen("AV_Core_Logs", "w");
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
