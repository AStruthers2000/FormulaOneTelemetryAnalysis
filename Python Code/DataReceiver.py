import sys
import socket
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from struct import *
from threading import Thread

class CustomPacket():
    def __init__(self):
        self.speed = 0
        self.topSpeed = 0
        self.averageSpeed = 0
        self.drs = 0
        self.throttle = 0
        self.brake = 0
        self.steer = 0
        self.currentLap = 0
        self.currentSector = 0
        self.currentLapTimeInMS = 0
        self.currentSector1Time = 0
        self.currentSector2Time = 0
        self.currentSector3Time = 0
        self.bestSector1Time = 0
        self.bestSector2Time = 0
        self.bestSector3Time = 0
        self.theoreticalLapTime = 0
        self.sessionTime = 0
        self.trackID = 0

customData = CustomPacket()
serv = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
serv.bind(('', 12346))

fig, ax = plt.subplots(2,2)

xdata, speed, drs, throttle, brake, steer = [], [], [], [], [], []

data = [0 for _ in range(50)]

new_lap = False


def init():
    return None

def animate(i):
    global new_lap
    global xdata
    global speed
    global drs
    global throttle
    global brake
    global steer
    global customData

    try:
        len_before = len(xdata)

        xdata.append(customData.currentLapTimeInMS)
        speed.append(customData.speed/1.609)
        drs.append(customData.drs*100)
        throttle.append(customData.throttle*100)
        brake.append(customData.brake*100)
        steer.append(customData.steer*100)


        if len(xdata) > len_before:
            ax[0][0].cla()
            ax[0][1].cla()
            ax[1][0].cla()
            #ax[0].title("Speed")
            #ax[0].xlabel("Time in Milliseconds Since Start of Lap")
            #ax[0].ylabel("Speed (mph")

            
            
            ax[0][0].plot(xdata, speed, label="Speed (MPH)")
            #ax[0].plot(xdata, topSpeed)
            ax[0][0].axhline(y=customData.topSpeed/1.609, color='r', linestyle='-', label="Top Speed (MPH)")
            ax[0][0].axhline(y=customData.averageSpeed/1.609, color='g', linestyle='-', label="Average Speed (MPH")
            ax[0][0].legend(loc="upper left")
            ax[0][0].set_title("Speed on lap {0} mph - Average speed: {1} mph - Top speed: {2} mph".format(round(customData.speed/1.609), round(customData.averageSpeed/1.609), round(customData.topSpeed/1.609)))
            ax[0][0].yaxis.set_label_text("Speed (MPH)")
            ax[0][0].xaxis.set_label_text("Time (MS)")


            ax[0][1].plot(xdata, xdata, label="Current Lap Time (MS)", color='g')

            ax[0][1].axvline(x=customData.currentSector1Time, color='red', linestyle='dashed')
            ax[0][1].axvline(x=customData.currentSector2Time + customData.currentSector1Time, color='blue', linestyle='dashed')
            ax[0][1].axvline(x=customData.currentSector3Time + customData.currentSector2Time + customData.currentSector1Time, color='yellow', linestyle='dashed')
            
            #ax[0][1].axhline(y=customData.bestSector1Time, color='r', linestyle='-')
            #ax[0][1].axhline(y=customData.bestSector2Time + customData.bestSector1Time, color='b', linestyle='-')
            #ax[1].axhline(y=customData.bestSector3Time + customData.bestSector2Time + customData.bestSector1Time, color='y', linestyle='-')
            ax[0][1].axvline(x=customData.bestSector1Time, color='r', linestyle='-')
            ax[0][1].axvline(x=customData.bestSector2Time + customData.bestSector1Time, color='b', linestyle='-')
            #ax[1].axvline(x=customData.bestSector3Time + customData.bestSector2Time + customData.bestSector1Time, color='y', linestyle='-')

            #ax[0][1].axhline(y=customData.theoreticalLapTime, color='purple', linestyle='-')
            ax[0][1].axvline(x=customData.theoreticalLapTime, color='purple', linestyle='-')
            
            ax[0][1].legend(loc="upper left")
            ax[0][1].set_title("Sector and Lap Times (MS)")
            ax[0][1].yaxis.set_label_text("Time (MS)")
            ax[0][1].xaxis.set_label_text("Time (MS)")


            ax[1][0].plot(xdata, throttle, color='green', label='Throttle')
            ax[1][0].plot(xdata, drs, color='yellow', label='DRS')
            ax[1][0].plot(xdata, brake, color='red', label='Brake')
            ax[1][0].yaxis.set_label_text("Percent Applied")
            ax[1][0].xaxis.set_label_text("Time (MS)")
            ax[1][0].set_title("Gas and Brake Levels")
            #ax[1][0].plot(xdata, steer, color='blue', label='Steer')
            
        if new_lap:
            print("new lap")
            xdata = []
            speed = []
            drs = []
            throttle = []
            brake = []
            steer = []
            new_lap = False
        
    except IndexError as ex:
        print(ex)
        pass

def GatherData():
    global data
    global customData
    global new_lap
    while True:
        msg, addr = serv.recvfrom(65565)
        #print(msg)
        data = unpack("<iifBfffBBIIIIIIIIfB", msg)

        if data[7] > customData.currentLap:
            new_lap = True
        
        customData.speed = data[0]
        customData.topSpeed = data[1]
        customData.averageSpeed = data[2]
        customData.drs = data[3]
        customData.throttle = data[4]
        customData.brake = data[5]
        customData.steer = data[6]
        customData.currentLap = data[7]
        customData.currentSector = data[8]
        customData.currentLapTimeInMS = data[9]
        customData.currentSector1Time = data[10]
        customData.currentSector2Time = data[11]
        customData.currentSector3Time = data[12]
        customData.bestSector1Time = data[13]
        customData.bestSector2Time = data[14]
        customData.bestSector3Time = data[15]
        customData.theoreticalLapTime = data[16]
        customData.sessionTime = data[17]
        customData.trackID = data[18]
        #print(data)
    
thread = Thread(target = GatherData)
thread.start()
    
ani = FuncAnimation(fig, animate, interval=20)

#plt.legend(loc="upper left")
plt.show()

serv.close()
thread.join()
sys.exit()

#while True:
    
    
