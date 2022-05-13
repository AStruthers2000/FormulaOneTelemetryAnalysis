#include <stdio.h>
#include <stdint.h>

#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define RECV_ADDR "127.0.0.1"
#define BUFLEN 4096
#define RECV_PORT 12345

#define SEND_ADDR "127.0.0.1"
#define SEND_PORT 12346

//header packet
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

//packet id 1: Session
struct __attribute__((__packed__)) MarshalZone
{
    float  m_zoneStart;   // Fraction (0..1) of way through the lap the marshal zone starts
    int8_t   m_zoneFlag;    // -1 = invalid/unknown, 0 = none, 1 = green, 2 = blue, 3 = yellow, 4 = red
};
struct __attribute__((__packed__)) WeatherForecastSample
{
    uint8_t     m_sessionType;              // 0 = unknown, 1 = P1, 2 = P2, 3 = P3, 4 = Short P, 5 = Q1
    // 6 = Q2, 7 = Q3, 8 = Short Q, 9 = OSQ, 10 = R, 11 = R2
    // 12 = Time Trial
    uint8_t     m_timeOffset;               // Time in minutes the forecast is for
    uint8_t     m_weather;                  // Weather - 0 = clear, 1 = light cloud, 2 = overcast
    // 3 = light rain, 4 = heavy rain, 5 = storm
    int8_t      m_trackTemperature;         // Track temp. in degrees Celsius
    int8_t      m_trackTemperatureChange;   // Track temp. change – 0 = up, 1 = down, 2 = no change
    int8_t      m_airTemperature;           // Air temp. in degrees celsius
    int8_t      m_airTemperatureChange;     // Air temp. change – 0 = up, 1 = down, 2 = no change
    uint8_t     m_rainPercentage;           // Rain percentage (0-100)
};
struct __attribute__((__packed__)) PacketSessionData
{
    struct PacketHeader    m_header;               // Header

    uint8_t           m_weather;              // Weather - 0 = clear, 1 = light cloud, 2 = overcast
    // 3 = light rain, 4 = heavy rain, 5 = storm
    int8_t          m_trackTemperature;       // Track temp. in degrees celsius
    int8_t          m_airTemperature;         // Air temp. in degrees celsius
    uint8_t           m_totalLaps;            // Total number of laps in this race
    uint16_t          m_trackLength;          // Track length in metres
    uint8_t           m_sessionType;          // 0 = unknown, 1 = P1, 2 = P2, 3 = P3, 4 = Short P
    // 5 = Q1, 6 = Q2, 7 = Q3, 8 = Short Q, 9 = OSQ
    // 10 = R, 11 = R2, 12 = R3, 13 = Time Trial
    int8_t            m_trackId;              // -1 for unknown, 0-21 for tracks, see appendix
    uint8_t           m_formula;              // Formula, 0 = F1 Modern, 1 = F1 Classic, 2 = F2,
    // 3 = F1 Generic
    uint16_t          m_sessionTimeLeft;      // Time left in session in seconds
    uint16_t          m_sessionDuration;      // Session duration in seconds
    uint8_t           m_pitSpeedLimit;        // Pit speed limit in kilometres per hour
    uint8_t           m_gamePaused;           // Whether the game is paused
    uint8_t           m_isSpectating;         // Whether the player is spectating
    uint8_t          m_spectatorCarIndex;    // Index of the car being spectated
    uint8_t           m_sliProNativeSupport;  // SLI Pro support, 0 = inactive, 1 = active
    uint8_t           m_numMarshalZones;      // Number of marshal zones to follow
    struct MarshalZone     m_marshalZones[21];     // List of marshal zones – max 21
    uint8_t           m_safetyCarStatus;      // 0 = no safety car, 1 = full
    // 2 = virtual, 3 = formation lap
    uint8_t           m_networkGame;          // 0 = offline, 1 = online
    uint8_t           m_numWeatherForecastSamples; // Number of weather samples to follow
    struct WeatherForecastSample m_weatherForecastSamples[56];   // Array of weather forecast samples
    uint8_t           m_forecastAccuracy;          // 0 = Perfect, 1 = Approximate
    uint8_t           m_aiDifficulty;              // AI Difficulty rating – 0-110
    uint32_t          m_seasonLinkIdentifier;      // Identifier for season - persists across saves
    uint32_t          m_weekendLinkIdentifier;     // Identifier for weekend - persists across saves
    uint32_t          m_sessionLinkIdentifier;     // Identifier for session - persists across saves
    uint8_t           m_pitStopWindowIdealLap;     // Ideal lap to pit on for current strategy (player)
    uint8_t           m_pitStopWindowLatestLap;    // Latest lap to pit on for current strategy (player)
    uint8_t           m_pitStopRejoinPosition;     // Predicted position to rejoin at (player)
    uint8_t           m_steeringAssist;            // 0 = off, 1 = on
    uint8_t           m_brakingAssist;             // 0 = off, 1 = low, 2 = medium, 3 = high
    uint8_t           m_gearboxAssist;             // 1 = manual, 2 = manual & suggested gear, 3 = auto
    uint8_t           m_pitAssist;                 // 0 = off, 1 = on
    uint8_t           m_pitReleaseAssist;          // 0 = off, 1 = on
    uint8_t           m_ERSAssist;                 // 0 = off, 1 = on
    uint8_t           m_DRSAssist;                 // 0 = off, 1 = on
    uint8_t           m_dynamicRacingLine;         // 0 = off, 1 = corners only, 2 = full
    uint8_t           m_dynamicRacingLineType;     // 0 = 2D, 1 = 3D
};


//packet id 2: Lap Data
struct __attribute__((__packed__)) LapData
{
    uint32_t   m_lastLapTimeInMS;               // Last lap time in milliseconds
    uint32_t   m_currentLapTimeInMS;            // Current time around the lap in milliseconds
    uint16_t   m_sector1TimeInMS;               // Sector 1 time in milliseconds
    uint16_t   m_sector2TimeInMS;               // Sector 2 time in milliseconds
    float      m_lapDistance;                   // Distance vehicle is around current lap in metres – could
                                                // be negative if line hasn’t been crossed yet
    float      m_totalDistance;                 // Total distance travelled in session in metres – could
                                                // be negative if line hasn’t been crossed yet
    float      m_safetyCarDelta;                // Delta in seconds for safety car
    uint8_t    m_carPosition;                   // Car race position
    uint8_t    m_currentLapNum;                 // Current lap number
    uint8_t    m_pitStatus;                     // 0 = none, 1 = pitting, 2 = in pit area
    uint8_t    m_numPitStops;                   // Number of pit stops taken in this race
    uint8_t    m_sector;                        // 0 = sector1, 1 = sector2, 2 = sector3
    uint8_t    m_currentLapInvalid;             // Current lap invalid - 0 = valid, 1 = invalid
    uint8_t    m_penalties;                     // Accumulated time penalties in seconds to be added
    uint8_t    m_warnings;                      // Accumulated number of warnings issued
    uint8_t    m_numUnservedDriveThroughPens;   // Num drive through pens left to serve
    uint8_t    m_numUnservedStopGoPens;         // Num stop go pens left to serve
    uint8_t    m_gridPosition;         	        // Grid position the vehicle started the race in
    uint8_t    m_driverStatus;         	        // Status of driver - 0 = in garage, 1 = flying lap
                                                // 2 = in lap, 3 = out lap, 4 = on track
    uint8_t    m_resultStatus;                  // Result status - 0 = invalid, 1 = inactive, 2 = active
                                                // 3 = finished, 4 = didnotfinish, 5 = disqualified
                                                // 6 = not classified, 7 = retired
    uint8_t    m_pitLaneTimerActive;     	    // Pit lane timing, 0 = inactive, 1 = active
    uint16_t   m_pitLaneTimeInLaneInMS;   	    // If active, the current time spent in the pit lane in ms
    uint16_t   m_pitStopTimerInMS;        	    // Time of the actual pit stop in ms
    uint8_t    m_pitStopShouldServePen;   	    // Whether the car should serve a penalty at this stop
};
struct __attribute__((__packed__)) PacketLapData
{
    struct PacketHeader    m_header;              // Header

    struct LapData         m_lapData[22];         // Lap data for all cars on track
};

//packet id 3: Event
union EventDataDetails
{
    struct
    {
        uint8_t	vehicleIdx; // Vehicle index of car achieving fastest lap
        float	lapTime;    // Lap time is in seconds
    } FastestLap;

    struct
    {
        uint8_t   vehicleIdx; // Vehicle index of car retiring
    } Retirement;

    struct
    {
        uint8_t   vehicleIdx; // Vehicle index of team mate
    } TeamMateInPits;

    struct
    {
        uint8_t   vehicleIdx; // Vehicle index of the race winner
    } RaceWinner;

    struct
    {
        uint8_t penaltyType;		// Penalty type – see Appendices
        uint8_t infringementType;		// Infringement type – see Appendices
        uint8_t vehicleIdx;         	// Vehicle index of the car the penalty is applied to
        uint8_t otherVehicleIdx;    	// Vehicle index of the other car involved
        uint8_t time;               	// Time gained, or time spent doing action in seconds
        uint8_t lapNum;             	// Lap the penalty occurred on
        uint8_t placesGained;       	// Number of places gained by this
    } Penalty;

    struct
    {
        uint8_t vehicleIdx;		// Vehicle index of the vehicle triggering speed trap
        float speed;      		// Top speed achieved in kilometres per hour
        uint8_t overallFastestInSession;   // Overall fastest speed in session = 1, otherwise 0
        uint8_t driverFastestInSession;    // Fastest speed for driver in session = 1, otherwise 0
    } SpeedTrap;

    struct
    {
        uint8_t numLights;		// Number of lights showing
    } StartLIghts;

    struct
    {
        uint8_t vehicleIdx;                 // Vehicle index of the vehicle serving drive through
    } DriveThroughPenaltyServed;

    struct
    {
        uint8_t vehicleIdx;                 // Vehicle index of the vehicle serving stop go
    } StopGoPenaltyServed;

    struct
    {
        uint32_t flashbackFrameIdentifier;  // Frame identifier flashed back to
        float flashbackSessionTime;       // Session time flashed back to
    } Flashback;

    struct
    {
        uint32_t        m_buttonStatus;    // Bit flags specifying which buttons are being pressed
        // currently - see appendices
    } Buttons;
};
struct PacketEventData
{
    struct PacketHeader    	m_header;               	// Header

    uint8_t           	m_eventStringCode[4];   	    // Event string code, see below
    union EventDataDetails	m_eventDetails;         	// Event details - should be interpreted differently
                                                        // for each type
};

//packet id 6: Car Telemetry
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


//custom data packet --  36 bytes
struct __attribute__((__packed__)) CustomDataPacket{
    int         speed;
    int         topSpeed;
    float       averageSpeed;
    uint8_t     drs;
    float       throttle;
    float       brake;
    float       steer;
    uint8_t     currentLap;
    uint8_t     currentSector;
    uint32_t    currentLapTimeInMS;
    uint32_t    currentSector1Time;
    uint32_t    currentSector2Time;
    uint32_t    currentSector3Time;
    uint32_t    bestSector1Time;
    uint32_t    bestSector2Time;
    uint32_t    bestSector3Time;
    uint32_t    theoreticalLapTime;
    float       sessionTime;
    int8_t      trackID;
    //add more as necessary
};

int main(void){
    struct sockaddr_in si_other_in;
    int s_in, slen_in=sizeof(si_other_in);

    char buf[BUFLEN];

    WSADATA wsa;

    printf("\nInitialising Winsock...\n");
    if(WSAStartup(MAKEWORD(2,2),&wsa) != 0){
        printf("Failed. Error Code: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialized\n");

    if((s_in = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR){
        printf("Socket in failed with error code: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    memset((char *) &si_other_in, 0, sizeof(si_other_in));
    si_other_in.sin_family = AF_INET;
    si_other_in.sin_port = htons(RECV_PORT);
    si_other_in.sin_addr.S_un.S_addr = inet_addr(RECV_ADDR);

    int __attribute__((__unused__)) bind_result = bind(s_in, (struct sockaddr *)&si_other_in, sizeof si_other_in);

    struct sockaddr_in si_other_out;
    int s_out, slen_out=sizeof(si_other_out);
    char message[BUFLEN];
    if((s_out = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR){
        printf("Socket out failed with error code: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    memset((char *) &si_other_out, 0, sizeof (si_other_out));
    si_other_out.sin_family = AF_INET;
    si_other_out.sin_port = htons(SEND_PORT);
    si_other_out.sin_addr.S_un.S_addr = inet_addr(SEND_ADDR);
    int __attribute__((__unused__)) connect_result = connect(s_out, (struct sockaddr *) &si_other_out, sizeof si_other_out);


    int packetUpdateCount = 0;
    struct CustomDataPacket customDataPacket = {};

    uint16_t topSpeed = 0;
    int dataSamples = 0;

    int currentLap;

    //start communication
    while(1){
        memset(buf, '\0', BUFLEN);
        //try to receive data
        if(recvfrom(s_in, buf, BUFLEN, 0, (struct sockaddr *) &si_other_in, &slen_in) == SOCKET_ERROR){
            printf("recvfrom failed with error code: %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
        struct PacketHeader *hdr = (struct PacketHeader*) &buf[0];

        switch(hdr->m_packetId) {

            struct PacketSessionData *sessionData;
            struct PacketLapData *lapData;
            struct PacketEventData *eventData;
            struct PacketCarTelemetryData *telemData;

            case 1:
                sessionData = (struct PacketSessionData *) &buf[0];
                customDataPacket.trackID = sessionData->m_trackId;
                customDataPacket.sessionTime = sessionData->m_header.m_sessionTime;

                packetUpdateCount++;
                break;

            case 2:
                lapData = (struct PacketLapData *) &buf[0];

                if(lapData->m_lapData[lapData->m_header.m_playerCarIndex].m_sector + 1 >= customDataPacket.currentSector ||
                   lapData->m_lapData[lapData->m_header.m_playerCarIndex].m_currentLapNum > currentLap){
                    switch(lapData->m_lapData[lapData->m_header.m_playerCarIndex].m_sector){
                        case 0:
                            if(customDataPacket.currentSector3Time < customDataPacket.bestSector3Time || customDataPacket.bestSector3Time == 0){
                                customDataPacket.bestSector3Time = customDataPacket.currentSector3Time;
                            }
                            break;
                        case 1:
                            if(customDataPacket.currentSector1Time < customDataPacket.bestSector1Time || customDataPacket.bestSector1Time == 0){
                                customDataPacket.bestSector1Time = customDataPacket.currentSector1Time;
                            }
                            break;
                        case 2:
                            if(customDataPacket.currentSector2Time < customDataPacket.bestSector2Time || customDataPacket.bestSector2Time == 0){
                                customDataPacket.bestSector2Time = customDataPacket.currentSector2Time;
                            }
                            break;
                        default:
                            printf("In a sector that doesn't exist\n");
                            break;
                    }

                }

                customDataPacket.currentLapTimeInMS = lapData->m_lapData[lapData->m_header.m_playerCarIndex].m_currentLapTimeInMS;
                customDataPacket.currentLap = lapData->m_lapData[lapData->m_header.m_playerCarIndex].m_currentLapNum;
                customDataPacket.currentSector = lapData->m_lapData[lapData->m_header.m_playerCarIndex].m_sector+1;

                switch(lapData->m_lapData[lapData->m_header.m_playerCarIndex].m_sector){
                    case 0:
                        customDataPacket.currentSector1Time = customDataPacket.currentLapTimeInMS;
                        break;
                    case 1:
                        customDataPacket.currentSector2Time = customDataPacket.currentLapTimeInMS - customDataPacket.currentSector1Time;
                        break;
                    case 2:
                        customDataPacket.currentSector3Time = customDataPacket.currentLapTimeInMS - customDataPacket.currentSector2Time - customDataPacket.currentSector1Time;
                        break;
                    default:
                        printf("In a sector that doesn't exist\n");
                        break;
                }




                packetUpdateCount++;
                break;

            case 3:
                eventData = (struct PacketEventData *) &buf[0];



                //uint8_t startCode[4] = {'L', 'G', 'O', 'T'};
                //uint8_t endCode[4] = {'S', 'E', 'N', 'D'};

                if(eventData->m_eventStringCode[0] == 'L' &&
                    eventData->m_eventStringCode[1] == 'G' &&
                    eventData->m_eventStringCode[2] == 'O' &&
                    eventData->m_eventStringCode[3] == 'T'){

                    printf("Start\n");
                    dataSamples = 0;
                    customDataPacket.averageSpeed = 0;

                }

                if(eventData->m_eventStringCode[0] == 'S' &&
                   eventData->m_eventStringCode[1] == 'E' &&
                   eventData->m_eventStringCode[2] == 'N' &&
                   eventData->m_eventStringCode[3] == 'D'){

                    printf("Finish\n");

                }

                packetUpdateCount++;
                break;

            case 6:
                dataSamples++;

                telemData = (struct PacketCarTelemetryData *) &buf[0];

                char isReverse = telemData->m_carTelemetryData[telemData->m_header.m_playerCarIndex].m_gear;

                int rawSpeed = telemData->m_carTelemetryData[telemData->m_header.m_playerCarIndex].m_speed;
                if(isReverse == -1){
                    rawSpeed *= -1;
                }

                customDataPacket.speed = rawSpeed;
                customDataPacket.drs = telemData->m_carTelemetryData[telemData->m_header.m_playerCarIndex].m_drs;
                customDataPacket.throttle = telemData->m_carTelemetryData[telemData->m_header.m_playerCarIndex].m_throttle;
                customDataPacket.brake = telemData->m_carTelemetryData[telemData->m_header.m_playerCarIndex].m_brake;
                customDataPacket.steer = telemData->m_carTelemetryData[telemData->m_header.m_playerCarIndex].m_steer;

                customDataPacket.averageSpeed += (((float)customDataPacket.speed-customDataPacket.averageSpeed) / ((float)dataSamples));

                if(customDataPacket.speed > topSpeed){
                    topSpeed = customDataPacket.speed;
                }

                customDataPacket.topSpeed = topSpeed;

                packetUpdateCount++;
                break;
            default:
                break;
        }



        if(packetUpdateCount >= 4){
            packetUpdateCount = 0;
            //send custom data packet out to Python script

            if(customDataPacket.currentLap > 1) {
                customDataPacket.theoreticalLapTime =
                        customDataPacket.bestSector1Time + customDataPacket.bestSector2Time +
                        customDataPacket.bestSector3Time;
            }

            if(sendto(s_out,
                      (struct CustomDataPacket*) &customDataPacket,
                              (sizeof(customDataPacket)),
                              0,
                              (struct sockaddr *) &si_other_out,
                                      sizeof(si_other_out)
                    )==SOCKET_ERROR){
                printf("sendto failed with error code: %d", WSAGetLastError());
                exit(EXIT_FAILURE);
            }
            //printf("Just sent customDataPacket\n");

            //printf("Current lap: %d --- current sector: %d --- speed: %d --- average speed: %4.1f --- top speed: %d\n", customDataPacket.currentLap, customDataPacket.currentSector, customDataPacket.speed, customDataPacket.averageSpeed, customDataPacket.topSpeed);


            if(customDataPacket.currentLap > currentLap){

                printf("Current lap: %d --- average speed last lap: %4.1f\n", customDataPacket.currentLap, customDataPacket.averageSpeed);

                dataSamples = 0;
                customDataPacket.averageSpeed = 0;
            }

            currentLap = customDataPacket.currentLap;



        }
        //puts(buf);
    }

    closesocket(s_in);
    WSACleanup();

    return 0;
}