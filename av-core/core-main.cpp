
#include "av-core/context.h"

unsigned char tx_char_buffer[1024 * 1024];
unsigned char rx_char_buffer[1024 * 1024];

int main () {
    buffer_t tx_buffer = create_buffer(1024, 1024, tx_char_buffer);
    buffer_t rx_buffer = create_buffer(1024, 1024, rx_char_buffer);

    CoreContext context;
    CoreLogger  logger(&context);
    CoreTransceiver transceiver (&context, &tx_buffer, &rx_buffer);
    CorePhysical    physical    (&context, &tx_buffer, &rx_buffer);

    pthread_t logger_id = logger     .start();
    pthread_t transc_id = transceiver.start();
    pthread_t physic_id = physical   .start();

    void* result;
    pthread_join(logger_id, &result);
    pthread_join(transc_id, &result);
    pthread_join(physic_id, &result);
}
