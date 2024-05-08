#pragma once

#include "av-module/core.h"
#include "av-module/logger.h"

#include "av-core/structs.h"
#include "av-core/context.h"

#include "communication/buffer.h"

#include <queue>

const unsigned int   PACKET_ID_MASK = (1 << 32) - 1;
const unsigned short CHECK_SUM_MASK = (1 << 16) - 1;
const unsigned int   MODULE_ID_MASK = (1 << 4) - 1;
const unsigned int   SIZE_MASK = (1 << 10) - 1;

struct PacketHeader {
public:
    unsigned int   packetID;
    unsigned short checkSum;

    bool hasNextPacket;
    bool isFirst;

    unsigned char moduleID;
    unsigned int  size;

    void normalize ();
    PacketHeader (int pid, int csum, bool hasNext, bool isFirst, int moduleID, int size)
        : packetID(pid), checkSum(csum), hasNextPacket(hasNext), isFirst(isFirst), moduleID(moduleID), size(size) {};
    PacketHeader (char* buffer);
    void write (char* buffer);
};

struct CoreTransceiver {
private:
    CoreContext * context;
    CoreTarget    target;
    ModuleLogger logger;

    int currentPacketID = 0;

    std::queue<std::string> strings_to_push_on_buffer;
    
    std::string current_rx_state      = "";
    unsigned int current_rx_packet_id = -1;
    unsigned int current_rx_check_sum = 0;

    buffer_t* tx_buffer;
    buffer_t* rx_buffer;

    FILE* txRadioLogs;
    FILE* rxRadioLogs;

    void tick ();
    void run ();
public:
    CoreTransceiver (CoreContext* context, buffer_t* rx_buffer, buffer_t* tx_buffer);

    pthread_t start ();

    static void* run_thread (void * context);
};
