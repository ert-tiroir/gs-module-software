
#include "communication/communication/rpi_spi.h"
#include "gs-socket.h"
#include <sys/socket.h>

unsigned char rx_buffer[1024 * 1024];
unsigned char tx_buffer[1024 * 1024];

int main () {
    int fd = create_vsocket("172.20.10.3", 8080);

    buffer_t rxbf = create_buffer(1024, 1924, rx_buffer);
    buffer_t txbf = create_buffer(1024, 1924, tx_buffer);

    rpi_spi_init(&rxbf);

    while (1) {
        rpi_spi_tick();

        unsigned char* page = readable_page(&rxbf);
        if (page != 0) {
            send(fd, page, 1024, 0);
            free_read(&rxbf);
        }
    }
}
