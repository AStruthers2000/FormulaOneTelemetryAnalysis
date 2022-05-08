import serial
import time

ser=serial.Serial(port='COM3',
                  baudrate=9600,
                  parity=serial.PARITY_NONE,
                  stopbits=serial.STOPBITS_ONE,
                  bytesize=serial.EIGHTBITS,
                  timeout=0)

print("Connected to: " + ser.portstr)
count = 1
line = []
#data = 
index = 0
while True:
    ser.write(chr(3).encode())
    print("Sent to uart: {0}".format(3))
    
    c = ser.read().decode()

    while True:
        try:
            print(c)
            c = chr(int(c))
            line.append(c)
            c = ser.read().decode()
        except TypeError:
            break
        
    print("Recvd: " + ''.join(line))
    line = []
    """
    for c in ser.read():
        line.append(chr(c))
        
        if chr(c) == "\n":
            print("Recvd: " + ''.join(line))
            line = []
    """
    
    #time.sleep(1)

ser.close()
