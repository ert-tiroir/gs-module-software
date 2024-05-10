
#include "av-module/core.h"
#include "av-module/logger.h"

#include <fcntl.h>
#include <unistd.h>

const char* PATH_SENSORS__module_output = "/tmp/sensors-module-output";
const char* PATH_SENSORS__module_input  = "/tmp/sensors-module-input";
const char* PATH_SENSORS__logger        = "/tmp/sensors-module-logger";

const char* PATH_SENSORS_result = "GS_Sensor_Received_Data";

int main () {
    CoreTarget target(
        PATH_SENSORS__module_output,
        PATH_SENSORS__module_input,
        PATH_SENSORS__logger
    );
    ModuleLogger logger (&target);

    logger << "Successfully opened sensors logger" << LogLevel::SUCCESS;

    FILE* sensorsResult = fopen( PATH_SENSORS_result, "w" );

    while (1) {
        bool found;
        std::string str = target.read_string_from_core(&found);
        if (!found) continue ;

        if (str[0] == 1) {
            float* values = (float*) (str.data() + 9);
            long long* tv = (long long*) (str.data() + 1);
        
            float temperature = values[0]; std::string str_temperature = std::to_string(temperature); 
            float pressure    = values[1]; std::string str_pressure    = std::to_string(pressure);
            float altitude    = values[2]; std::string str_altitude    = std::to_string(altitude);

            logger << "Telemetry from t='" << tv[0] << " ms'" << LogLevel::INFO;
            logger << "Temperature T='" << str_temperature << "*C'" << LogLevel::INFO;
            logger << "Pressure P='" << str_pressure << "hPa'" << LogLevel::INFO;
            logger << "Atltiude h='" << str_altitude << "m'" << LogLevel::INFO;

            fprintf(sensorsResult, "Telemetry from time t='%lld ms'", tv[0]);
            fprintf(sensorsResult, "Temperature T='%f *C'\n", temperature);
            fprintf(sensorsResult, "Pressure P='%f hPa'\n", pressure);
            fprintf(sensorsResult, "Altitude h='%f m'\n", altitude);
        }
    }
}
