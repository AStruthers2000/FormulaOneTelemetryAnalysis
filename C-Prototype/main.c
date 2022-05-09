#include <stdio.h>
#include <stdint.h>

#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER "127.0.0.1"
#define BUFLEN 4096
#define PORT 12345

struct __attribute__((__packed__)) PacketHeader{
    uint16_t    m_packetFormat;
    uint8_t     m_gameMajorVersion;
    uint8_t     m_gameMinorVersion;
    uint8_t     m_packetVersion;

    uint8_t     m_packetId;

    uint64_t    m_sessionUID;
    float       m_sessionTime;
    uint32_t    m_frameIdentifier;

    uint8_t     m_playerCarIndex;
    uint8_t     m_secondaryPlayerCarIndex;
};

struct __attribute__((__packed__)) CarTelemetryData
{
    uint16_t    m_speed;                    // Speed of car in kilometres per hour
    float       m_throttle;                 // Amount of throttle applied (0.0 to 1.0)
    float       m_steer;                    // Steering (-1.0 (full lock left) to 1.0 (full lock right))
    float       m_brake;                    // Amount of brake applied (0.0 to 1.0)
    uint8_t     m_clutch;                   // Amount of clutch applied (0 to 100)
    int8_t      m_gear;                     // Gear selected (1-8, N=0, R=-1)
    uint16_t    m_engineRPM;                // Engine RPM
    uint8_t     m_drs;                      // 0 = off, 1 = on
    uint8_t     m_revLightsPercent;         // Rev lights indicator (percentage)
    uint16_t    m_revLightsBitValue;        // Rev lights (bit 0 = leftmost LED, bit 14 = rightmost LED)
    uint16_t    m_brakesTemperature[4];     // Brakes temperature (celsius)
    uint8_t     m_tyresSurfaceTemperature[4]; // Tyres surface temperature (celsius)
    uint8_t     m_tyresInnerTemperature[4]; // Tyres inner temperature (celsius)
    uint16_t    m_engineTemperature;        // Engine temperature (celsius)
    float       m_tyresPressure[4];         // Tyres pressure (PSI)
    uint8_t     m_surfaceType[4];           // Driving surface, see appendices
};

struct __attribute__((__packed__)) PacketCarTelemetryData
{
    struct PacketHeader    	    m_header;

    struct CarTelemetryData     m_carTelemetryData[22];

    uint8_t                     m_mfdPanelIndex;
    uint8_t                     m_mfdPanelIndexSecondaryPlayer;
    int8_t                      m_suggestedGear;
};


int main(void){
    printf("%d", sizeof(uint64_t));
    struct sockaddr_in si_other;
    int s, slen=sizeof(si_other);

    char buf[BUFLEN];
    char message[BUFLEN];
    WSADATA wsa;

    printf("\nInitialising Winsock...\n");
    if(WSAStartup(MAKEWORD(2,2),&wsa) != 0){
        printf("Failed. Error Code: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialized\n");

    if((s= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR){
        printf("Socket failed with error code: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

    bind(s, (struct sockaddr *)&si_other, sizeof si_other);

    //start communication
    while(1){
        memset(buf, '\0', BUFLEN);
        //try to receive data
        if(recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR){
            printf("recvfrom failed with error code: %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
        struct PacketHeader *hdr = (struct PacketHeader*) &buf[0];
        switch(hdr->m_packetId) {
            struct PacketCarTelemetryData *data;
            case 6:
                data = (struct PacketCarTelemetryData *) &buf[0];
                printf("Telemetry data\n");
                float speed_mph = data->m_carTelemetryData[data->m_header.m_playerCarIndex].m_speed/1.609;
                printf("%4f\n", speed_mph);
                break;
            default:
                break;
        }
        //puts(buf);
    }

    closesocket(s);
    WSACleanup();

    return 0;
}