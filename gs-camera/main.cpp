
#include "av-module/core.h"
#include "av-module/logger.h"

#include <fcntl.h>
#include <unistd.h>

const char* PATH_CAMERA__module_output = "/tmp/camera-module-output";
const char* PATH_CAMERA__module_input  = "/tmp/camera-module-input";
const char* PATH_CAMERA__logger        = "/tmp/camera-module-logger";

const char* PATH_CAMERA_result = "GS_Camera_Received_Data";

int main () {
    CoreTarget target(
        PATH_CAMERA__module_output,
        PATH_CAMERA__module_input,
        PATH_CAMERA__logger
    );
    ModuleLogger logger (&target);

    FILE* cameraResult = fopen(PATH_CAMERA_result, "w");

    logger << "Succesfully opened Camera Module" << LogLevel::SUCCESS;

    std::string startString = "START";
    std::string stopString = "STOP";
    target.write_string_to_core(startString);
    logger << "Sent start message" << LogLevel::INFO;

    for (int i = 0; i < 1000000; i ++) {
        bool found = false;
        std::string cameraData = target.read_string_from_core(&found);
        if (!found) continue ;

        fwrite(cameraData.data(), 1, cameraData.size(), cameraResult);
        fflush(cameraResult);
    }

    target.write_string_logger(stopString);
    logger << "Sent stop message" << LogLevel::INFO;

    while (1) {
        bool found = false;
        std::string cameraData = target.read_string_from_core(&found);
        if (!found) continue ;

        fwrite(cameraData.data(), 1, cameraData.size(), cameraResult);
        fflush(cameraResult);
    }
}
