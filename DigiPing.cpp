#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <array>
#include <atomic>
#include <chrono>

#pragma comment(lib, "Ws2_32.lib")

using TimestampMs = uint64_t;
constexpr TimestampMs TIMEOUT_MS = 1000;

// ICMP Types we're concerned with here
constexpr uint8_t ICMP_TYPE_DESTINATION_UNREACHABLE = 3;
constexpr uint8_t ICMP_TYPE_ECHO = 8;
constexpr uint8_t ICMP_TYPE_TIME_EXCEEDED = 11;
constexpr uint8_t ICMP_TYPE_ECHOREPLY = 0;

constexpr size_t PACKET_SIZE = 32; // arbitrary value of data to send: not too little, not too much ...

// ICMP Structure
struct ICMPPacket {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t sequence;
    char data[PACKET_SIZE];
};

static uint16_t compute_checksum(void* buffer, int size)
{
    uint16_t* buf = reinterpret_cast<uint16_t*>(buffer);
    uint32_t sum = 0;

    for (; size > 1; size -= 2)
        sum += *buf++;

    if (size == 1)
        sum += *reinterpret_cast<uint8_t*>(buf);

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    return static_cast<uint16_t>(~sum);
}

static void fill_icmp_packet(ICMPPacket& packet, uint16_t id, uint16_t sequence)
{
    packet = {};
    // TODO: set the other values of the packet correctly
    packet.checksum = 0;
    packet.checksum = compute_checksum(&packet, sizeof(ICMPPacket));
}

static std::atomic<bool> g_running(true);

static BOOL WINAPI console_handler(DWORD signal) 
{
    if (signal == CTRL_C_EVENT) 
    {
        g_running = false; // stops infinite ping loop on ctrl-c
        return TRUE;
    }
    return FALSE;
}

static uint8_t calculate_ip_header_size(char* buf) 
{
    /*
    * in RAW mode, we get the entire packet as recv_buf, including the IPv4 Header.
    * We want to skip forward past the IPv4 header and find the beginning of the ICMP reply.
    * How can we determine the length of the IPv4 header? 
    * Hint: Check the header definition
    */
    return 0;
}

/**
* 
* sock: our raw socket
* start_time: ticks from when we started, so we know when to stop waiting and how long its been
* from: returns where the packet came from
* from_len: length of the from struct
* out_bytes_received: how many bytes of DATA did we get? (NOT total bytes, NOT counting headers -- just ICMP data.)
* out_ttl: what was the TTL of the response?
*/
static ICMPPacket* wait_for_reply(SOCKET sock, TimestampMs start_time, sockaddr_in& from, int& from_len, int& out_bytes_received, uint8_t& out_ttl)
{
    // We've sent an echo request, now we need to wait until an echo reply (or something else) shows up
    // OR, maybe NOTHING shows up, and we should time out.
    while (GetTickCount64() - start_time < TIMEOUT_MS)
    {
        // TODO: receive the packet into a buffer. 
        // Use the "out" args of recvfrom() to find out where this packet came from 
        // **The response doesn't necessarily come from where we sent the request to!**
        
        // TODO: On Success:
        // since this is a RAW socket, we get the entire contents of the packet, 
        // starting with the IPv4 header. We need to determine the TTL value of the packet
        // and return it in out_ttl. Then read past the IPv4 header (how?) and read the
        // ICMP packet. A little bit of math will tell you how many bytes of data (within that icmp response)
        // were returned. 

        // No response? Otherwise sleep for a bit...
        Sleep(10);
    }
    // time expired
    return nullptr;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << "<hostname or ip>" << std::endl;
        return 1;
    }

    if (!SetConsoleCtrlHandler(console_handler, TRUE)) {
        std::cerr << "Failed to set control handler" << std::endl;
        return 1;
    }

    // TODO: Initialize WinSock
    
    // TODO: create a RAW socket for ICMP
    
    // TODO: Figure out our destination address:
    // First try a DNS lookup, if that fails, 
    // try to interpret the string as an IP address. 
    // If we can't work out the destination, print a helpful error message and exit.

    // TODO: make socket non-blocking

    // Use this as our ICMP ID
    uint16_t pid = static_cast<uint16_t>(GetCurrentProcessId());
    // Sequence number starts at 1 and then is incremented with each packet sent:
    int sequence = 1;

    std::cout << "Pinging " << argv[1] << " with " << PACKET_SIZE << " bytes of data:" << std::endl;
    while (g_running)
    {
        ICMPPacket packet;
        fill_icmp_packet(packet, pid, sequence++);
        TimestampMs start_time = GetTickCount64();

        // TODO: send our packet to the destination on our NON-BLOCKING socket

        // TODO get the reply, if any:
        int data_bytes_received = 0;
        uint8_t ttl = 0;
        ICMPPacket* reply = nullptr; /* wait_for_reply(sock, start_time, from, from_len, data_bytes_received, ttl); */
        if (!reply)
        {
            std::cout << argv[1] << ": Request timed out." << std::endl;
        } 
        // TODO:
        // otherwise, determine the type of the reply and print the appropriate response
        else if (reply->type == ICMP_TYPE_ECHOREPLY && ntohs(reply->id) == pid)
        {
            TimestampMs end_time = GetTickCount64();
            //TODO: print out the hostname returned by the response (NOT the one we sent to!)
            /*
            TODO:
            std::cout << "Reply from " << hostname
                << ": bytes=" << data_bytes_received
                << " time=" << end_time - start_time << "ms" << 
                " ttl=" << static_cast<int>(ttl) << std::endl;
            */
        } 
        else
        {
            // TODO: handle other icmp message types: print out the type and code received
        }
        // wait a second before sending the next ping
        Sleep(1000);
    }

    // TODO: cleanup, close and exit
    return 0;
}
