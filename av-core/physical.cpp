
#include "av-core/physical.h"
#include "communication/rpi_spi.h"

const char* AV_PHYSICAL__module_output = "/tmp/av-physical-output";
const char* AV_PHYSICAL__module_input  = "/tmp/av-physical-input";
const char* AV_PHYSICAL__logger        = "/tmp/av-physical-logger";

void CorePhysical::tick () {
    rpi_spi_load(this->tx_buffer, 1);
    rpi_spi_tick();
}
void CorePhysical::run () {
    this->logger << "Joined Physical Thread" << LogLevel::SUCCESS;

    while (1) {
        tick();
    }
}

CorePhysical::CorePhysical (CoreContext* context, buffer_t* rx_buffer, buffer_t* tx_buffer) {
    this->context = context;

    this->context->register_module(
        "AV PHY",
        AV_PHYSICAL__module_output,
        AV_PHYSICAL__module_input,
        AV_PHYSICAL__logger
    );

    this->target = CoreTarget(
        AV_PHYSICAL__module_output,
        AV_PHYSICAL__module_input,
        AV_PHYSICAL__logger
    );
    this->logger = ModuleLogger(&this->target);

    this->rx_buffer = rx_buffer;
    this->tx_buffer = tx_buffer;

    rpi_spi_init(this->rx_buffer);
}

pthread_t CorePhysical::start () {
    pthread_t thread_id;

    pthread_create( &thread_id, NULL, &CorePhysical::run_thread, this );

    return thread_id;
}

void* CorePhysical::run_thread (void * context) {
    CorePhysical* logger = (CorePhysical*) context;

    logger->run();

    return nullptr;
}
