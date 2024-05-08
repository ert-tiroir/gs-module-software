
#include "av-core/transceiver.h"

const char* AV_TRANSCEIVER__module_output = "/tmp/av-transceiver-output";
const char* AV_TRANSCEIVER__module_input  = "/tmp/av-transceiver-input";
const char* AV_TRANSCEIVER__logger        = "/tmp/av-transceiver-logger";

void PacketHeader::normalize () {
    packetID = packetID & PACKET_ID_MASK;
    moduleID = moduleID & MODULE_ID_MASK;
    checkSum = checkSum & CHECK_SUM_MASK;
    size     = size     & SIZE_MASK;
}
void PacketHeader::write (char* _buffer) {
    normalize();

    const int BITS8 = (1 << 8) - 1;

    unsigned char* buffer = (unsigned char*) _buffer;
    buffer[0] = packetID & BITS8;
    buffer[1] = (packetID >> 8) & BITS8;
    buffer[2] = (packetID >> 16) & BITS8;
    buffer[3] = (packetID >> 32) & BITS8;
    buffer[4] = checkSum & BITS8;
    buffer[5] = (checkSum >> 8) & BITS8;
    buffer[6] = ((moduleID << 2) | (size << 6)) & BITS8;
    if (hasNextPacket) buffer[6] |= 1;
    if (isFirst) buffer[6] |= 2;
    buffer[7] = (size >> 2) & BITS8;
}
PacketHeader::PacketHeader (char* __buffer) {
    unsigned char* buffer = (unsigned char*) __buffer;

    unsigned int ibuffer[8];
    for (int i = 0; i < 8; i ++) ibuffer[i] = buffer[i];

    packetID = (ibuffer[0] << 0)
             | (ibuffer[1] << 8)
             | (ibuffer[2] << 16)
             | (ibuffer[3] << 24);
    checkSum = (ibuffer[4]) | (ibuffer[5] << 8);
    hasNextPacket = (ibuffer[6] & 1) != 0;
    isFirst = (ibuffer[6] & 2) != 0;
    moduleID = (ibuffer[6] >> 2) & MODULE_ID_MASK;
    size = (ibuffer[6] >> 6) | (ibuffer[7] << 2);
}

void CoreTransceiver::tick () {
    int number_modules = context->module_count();

    for (int module_id = 0; module_id < number_modules; module_id ++) {
        CoreModule* module = context->get_module(module_id);
        if (module == nullptr) continue ;
        if (!module->ready()) continue ;

        bool found;
        std::string result = module->read_string_from_module(&found);
        if (!found) continue ;

        const int PAGE_SIZE = 1024;
        const int HEAD_SIZE = 8;
        const int DATA_SIZE = PAGE_SIZE - HEAD_SIZE;

        int amount  = (result.size() + DATA_SIZE - 1) / DATA_SIZE;
        int lastSum = 0;
        
        for (int i = 0; i < amount; i ++) {
            std::string buffer(PAGE_SIZE, '.');

            int offset = i * DATA_SIZE;
            int count  = std::min((unsigned long) DATA_SIZE, result.size() - offset);
            unsigned int provCheckSum = lastSum;
            for (int j = 0; j < count; j ++) {
                buffer[j + HEAD_SIZE] = result[offset + j];
                provCheckSum += (unsigned char) buffer[j + HEAD_SIZE];
            }

            PacketHeader headerWithoutCheckSum(currentPacketID, 0, i + 1 != amount, i == 0, module_id, count);
            headerWithoutCheckSum.write(buffer.data());

            for (int j = 0; j < HEAD_SIZE; j ++)
                provCheckSum += (unsigned char) buffer[j];
            PacketHeader header (currentPacketID, provCheckSum, i + 1 != amount, i == 0, module_id, count);
            header.write(buffer.data());

            strings_to_push_on_buffer.push( buffer );
            fwrite(buffer.c_str(), 1, PAGE_SIZE, txRadioLogs);

            lastSum = provCheckSum;
        }

        currentPacketID ++;
    }

    while (1) {
        unsigned char* txpage = writable_page(tx_buffer);
        if (txpage == 0) break ;

        if (strings_to_push_on_buffer.size() == 0) break ;

        std::string data_tx = strings_to_push_on_buffer.front();
        strings_to_push_on_buffer.pop();

        for (int i = 0; i < tx_buffer->pag_size; i ++)
            txpage[i] = data_tx[i];

        free_write(tx_buffer);
    }

    while (1) {
        unsigned char* rxpage = readable_page(rx_buffer);
        if (rxpage == 0) break ;

        fwrite(rxpage, 1, rx_buffer->pag_size, rxRadioLogs);

        PacketHeader header((char*) rxpage);
        if (header.isFirst) {
            current_rx_packet_id = header.packetID;
            current_rx_check_sum = 0;
            current_rx_state.clear();
        }

        unsigned int new_check_sum = current_rx_check_sum;
        for (int i = 0; i < 8 + header.size; i ++)
            new_check_sum += rxpage[i];
        new_check_sum -= rxpage[4];
        new_check_sum -= rxpage[5];
        new_check_sum &= CHECK_SUM_MASK;
        if (header.checkSum != new_check_sum
         || current_rx_packet_id != header.packetID) {
            if (header.checkSum != new_check_sum)
                logger << "Wrong check sum received by transceiver, a packet was lost." << LogLevel::DEBUG;
            if (header.packetID != current_rx_packet_id && current_rx_packet_id != -1)
                logger << "Wrong packet ID received before an EOP, a packet was lost." << LogLevel::DEBUG;

            current_rx_packet_id = - 1;
            current_rx_check_sum = 0;
            current_rx_state.clear();
            free_read(rx_buffer);
            continue ;
        }

        std::string str(header.size, '.');
        for (int i = 0; i < header.size; i ++)
            str[i] = rxpage[i + 8];
        current_rx_state.append(str);

        current_rx_check_sum = new_check_sum;

        if (!header.hasNextPacket) {
            CoreModule* module = context->get_module( header.moduleID );
            if (module != nullptr) {
                module->write_string_to_module(current_rx_state);
            } else logger << "Wrong Module ID, the packet could not be forwarded to the receiver" << LogLevel::DEBUG;

            current_rx_packet_id = - 1;
            current_rx_check_sum = 0;
            current_rx_state.clear();
        }
        
        free_read(rx_buffer);
    }

    fflush(rxRadioLogs);
    fflush(txRadioLogs);
}
void CoreTransceiver::run () {
    this->logger << "Joined Transceiver Thread" << LogLevel::SUCCESS;

    while (1) {
        tick();
    }
}

CoreTransceiver::CoreTransceiver (CoreContext* context, buffer_t* rx_buffer, buffer_t* tx_buffer) {
    this->context = context;

    this->context->register_module(
        "AV TRC",
        AV_TRANSCEIVER__module_output,
        AV_TRANSCEIVER__module_input,
        AV_TRANSCEIVER__logger
    );

    this->target = CoreTarget(
        AV_TRANSCEIVER__module_output,
        AV_TRANSCEIVER__module_input,
        AV_TRANSCEIVER__logger
    );
    this->logger = ModuleLogger(&this->target);

    this->rxRadioLogs = fopen("AV_Core_RX_Radio_Logs", "w");
    this->txRadioLogs = fopen("AV_Core_TX_Radio_Logs", "w");

    this->rx_buffer = rx_buffer;
    this->tx_buffer = tx_buffer;
}

pthread_t CoreTransceiver::start () {
    pthread_t thread_id;

    pthread_create( &thread_id, NULL, &CoreTransceiver::run_thread, this );

    return thread_id;
}

void* CoreTransceiver::run_thread (void * context) {
    CoreTransceiver* logger = (CoreTransceiver*) context;

    logger->run();

    return nullptr;
}
