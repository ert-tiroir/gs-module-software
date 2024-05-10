
#include "av-module/core.h"
#include "av-module/logger.h"

#include <fcntl.h>
#include <unistd.h>

#include "gs-socket.h"

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

    FILE* cameraResult = nullptr;

    socket_t socket(1);

    logger << "Succesfully opened Camera Module" << LogLevel::SUCCESS;

    bool running = false;

    for (int i = 0; i < 1000000; i ++) {
        socket.tick();
        std::string rcv = socket.recv();
        if (rcv == "START") {
            if (cameraResult != nullptr) {
                logger << "Cannot start camera module if it is already started" << LogLevel::WARNING;
                continue ;
            }
            cameraResult = fopen(PATH_CAMERA_result, "w");
            running = true;
        } else if (rcv == "STOP") {
            if (cameraResult == nullptr) {
                logger << "Cannot stop camera module if it is already stopped" << LogLevel::WARNING;
                continue ;
            }
            fclose(cameraResult);
            cameraResult = nullptr;
            running = false;
        }
        if (rcv != "") target.write_string_to_core(rcv);

        bool found = false;
        std::string cameraData = target.read_string_from_core(&found);
        if (!found) continue ;
        if (!running) continue ;

        fwrite(cameraData.data(), 1, cameraData.size(), cameraResult);
        fflush(cameraResult);
    }
}
