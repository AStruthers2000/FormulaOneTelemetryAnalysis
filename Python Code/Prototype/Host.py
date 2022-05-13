import socket, sys
from struct import *
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from typing import NamedTuple
from threading import Thread
from DataStructures import *



class DataRepresentation:
    def __init__(self):
        self.dataStruct = Data()
        self.xVals = []
        self.yVals = []
        self.prevSessionTime = -1
        self.currentLap = 0
        self.savingLaps = True
        self.gatherData = True
        self.serv = None

        self.trackDict = {
            0:	"Melbourne",
            1:	"Paul Ricard",
            2:	"Shanghai",
            3:	"Sakhir (Bahrain)",
            4:	"Catalunya",
            5:	"Monaco",
            6:	"Montreal",
            7:	"Silverstone",
            8:	"Hockenheim",
            9:	"Hungaroring",
            10:	"Spa",
            11:	"Monza",
            12:	"Singapore",
            13:	"Suzuka",
            14:	"Abu Dhabi",
            15:	"Texas",
            16:	"Brazil",
            17:	"Austria",
            18:	"Sochi",
            19:	"Mexico",
            20:	"Baku (Azerbaijan)",
            21:	"Sakhir Short",
            22:	"Silverstone Short",
            23:	"Texas Short",
            24:	"Suzuka Short",
            25:	"Hanoi",
            26:	"Zandvoort",
            27:	"Imola",
            28:	"Portimão",
            29:	"Jeddah",
        }
        
    def GatherData(self):
        try:
            self.serv = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP) #, socket.IPPROTO_UDP)
            self.serv.bind(('', 12345))
        except OSError:
            print("[WinError 10013] An attempt was made to access a socket in a way forbidden by its access permissions")
            sys.exit()
            
        lastTime = -1
        uniquePackets = 2
        needsUpdate = uniquePackets
        myData = Data()
        while self.gatherData:
            msg = ""
            try:
                msg, addr = self.serv.recvfrom(65565)
            except OSError:
                print("[WinError 10038] An operation was attempted on something that is not a socket\n\tProbably had serv shut down from different thread.")
                break
            packet = msg
            header = packet[:24]
            print(header)
            telemetry = packet[24:]

            header = unpack("<HBBBBQfIBB", header)
            print(header)
            headerPacket = HeaderData(*header)

            #myData.sessionTime = headerPacket.sessionTime
            
            #print("Packet format: {0} Version: {1}.{2} Packet Version: {3} Packet ID: {4} Session UID: {5} Session Time: {6} Frame Identifier: {7} Player Car Index: {8} Second Car Index: {9}".format(header[0], header[1], header[2], header[3], header[4], header[5], header[6], header[7], header[8], header[9]))
            if headerPacket.packetID == 2:
                lapDataList = []
                for i in range(43, len(telemetry)+43, 43):
                    lapDataPacket = telemetry[i-43:i]
                    lap = unpack("<IIHHfffBBBBBBBBBBBBBBHHB", lapDataPacket)
                    lapPacket = LapData(*lap)
                    lapDataList.append(lapPacket)
                fullLapData = PacketLapData(headerPacket, lapDataList)

                #print(headerPacket.playerCarIndex)
                myData.currentLapTimeMS = fullLapData.lapData[headerPacket.playerCarIndex].currentLapTimeMS
                myData.lap = fullLapData.lapData[headerPacket.playerCarIndex].currentLapNum
                myData.sector = fullLapData.lapData[headerPacket.playerCarIndex].sector + 1
                lapTime = fullLapData.lapData[headerPacket.playerCarIndex].currentLapTimeMS
                myData.currentLapTime = "{0}:{1}.{2}".format(int(lapTime/(60000))%60, int(lapTime/1000)%60, int(lapTime%1000))
                
                needsUpdate -= 1

            
            if headerPacket.packetID == 6:
                carTeleList = []
                remainingData = telemetry[len(telemetry)-3:]
                remainingData = unpack("<BBb", remainingData)
                mfdPanel = remainingData[0]
                mfdPanelSecond = remainingData[1]
                suggestedGear = remainingData[2]
                
                telemetry = telemetry[:len(telemetry)-3]
                
                for i in range(60, len(telemetry)+60, 60):
                    carTeleDataPacket = telemetry[i-60:i]
                    carTeleData = unpack("<HfffBbHBBHHHHHBBBBBBBBHffffBBBB", carTeleDataPacket)

                    brakeTemps = [round((x*1.8)+32) for x in carTeleData[10:14]]
                    tyreSurfTemps = [round((x*1.8)+32) for x in carTeleData[14:18]]
                    tyreInnTemps = [round((x*1.8)+32) for x in carTeleData[18:22]]
                    tyrePres = carTeleData[23:27]
                    surfType = carTeleData[27:31]
                    carTeleData = (*carTeleData[:10], brakeTemps, tyreSurfTemps, tyreInnTemps, carTeleData[22], tyrePres, surfType)

                    telePacket = CarTelemetryData(*carTeleData)
                    carTeleList.append(telePacket)

                fullCarTelemetryData = PacketCarTelemetryData(headerPacket, carTeleList, mfdPanel, mfdPanelSecond, suggestedGear)

                isReverse = -1 if fullCarTelemetryData.carTelemetryData[headerPacket.playerCarIndex].gear == -1 else 1
                myData.speed = round(fullCarTelemetryData.carTelemetryData[headerPacket.playerCarIndex].speed / 1.609) * isReverse
                myData.drs = fullCarTelemetryData.carTelemetryData[headerPacket.playerCarIndex].drs
                myData.throttle = fullCarTelemetryData.carTelemetryData[headerPacket.playerCarIndex].throttle
                myData.brake = fullCarTelemetryData.carTelemetryData[headerPacket.playerCarIndex].brake
                myData.steer = fullCarTelemetryData.carTelemetryData[headerPacket.playerCarIndex].steer

                needsUpdate -= 1

            if headerPacket.packetID == 3:
                eventCodePacket = telemetry[:4]
                eventCode = "".join([chr(x) for x in unpack("<BBBB", eventCodePacket)])
                if eventCode == "LGOT":
                    print("And it's lights out and away we go!")
                elif eventCode == "SEND":
                    print("Finish")
                    myData.lap+=1
                    needsUpdate = uniquePackets
                    myData.sessionTime = headerPacket.sessionTime
                    lastTime = myData.sessionTime
                    self.dataStruct = myData

            if headerPacket.packetID == 1:
                #this is a temporary solution because i don't want to build the entire session packet
                trackID = telemetry[:8]
                trackID = unpack("<BbbBHBb", trackID)
                
                trackID = trackID[6]
                #print("Track ID: {0} Track Name: {1}".format(trackID, self.trackDict[trackID]))
                myData.trackID = trackID
                myData.trackName = self.trackDict[trackID]
                
            if needsUpdate == 0 and lastTime != headerPacket.sessionTime:
                needsUpdate = uniquePackets
                myData.sessionTime = headerPacket.sessionTime
                lastTime = myData.sessionTime
                self.dataStruct = myData

            
        self.serv.close()

    
    def animate(self, i):
        if self.prevSessionTime != self.dataStruct.sessionTime:
            self.prevSessionTime = self.dataStruct.sessionTime

            if self.dataStruct.lap != self.currentLap:
                if len(self.yVals) > 15:
                    aveSpeed = sum(self.yVals)/len(self.yVals)
                    topSpeed = max(self.yVals)
                    print("Lap {0}:\n\tAverage speed: {1} mph\n\tTop speed: {2} mph".format(self.currentLap, aveSpeed, topSpeed))

                    
                self.xVals = []
                self.yVals = []
                
                if self.savingLaps and self.currentLap > 0:
                    print("Saving lap {0} data".format(self.currentLap))
                    path = "D:\\Users\\Strut\\Desktop\\SeniorProject\\Lap{0}_{1}.png".format(self.currentLap, self.dataStruct.trackName)
                    plt.savefig(path)
                    
                self.currentLap = self.dataStruct.lap

            if len(self.xVals) > 15:
                if self.dataStruct.currentLapTimeMS < self.xVals[-1]:
                    #used a rewind
                    plottedX = self.xVals
                    plottedY = self.yVals
                    
                    ms = self.dataStruct.currentLapTimeMS
                        
                    index = -1
                    for x in range(len(plottedX)):
                        if plottedX[x] <= ms and ms <= plottedX[x+1]:
                            index = x
                    self.xVals = plottedX[:index]
                    self.yVals = plottedY[:index]

            self.xVals.append(self.dataStruct.currentLapTimeMS)
            self.yVals.append(self.dataStruct.speed)
            
            

            plt.cla()
            plt.title("Lap {0} at {1} - average speed: {2}".format(self.currentLap, self.dataStruct.trackName, round(sum(self.yVals)/len(self.yVals))))
            plt.xlabel("Time in Milliseconds Since Start of Lap")
            plt.ylabel("Speed (mph)")
            plt.plot(self.xVals, self.yVals)

    def run(self):
        thread = Thread(target = self.GatherData)
        thread.start()

        fig = plt.figure()
        ani = animation.FuncAnimation(fig, self.animate, interval = 100)
        plt.tight_layout()
        plt.show()
        
        self.serv.close()
        thread.join()
        
        while thread.is_alive():
            print("Not dead yet")
        print("System exited successfully")
        sys.exit()




"""
            print("Lap {0} | Sector {1} | Current Lap Time: {2}".format(myData.lap, myData.sector, myData.currentLapTime))
            print("Speed: {0} mph | DRS enabled: {1} | Throttle: {2} | Brake: {3} | Steering: {4}".format(myData.speed, "Yes" if myData.drs == 1 else "No", myData.throttle, myData.brake, myData.steer))
            print("====="*10)
"""
if __name__ == "__main__":
    dataRep = DataRepresentation()
    dataRep.run()


