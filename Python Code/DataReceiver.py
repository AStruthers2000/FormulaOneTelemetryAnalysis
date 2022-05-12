import socket
from struct import *

serv = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
serv.bind(('', 12346))

while True:
    msg, addr = serv.recvfrom(65565)
    #print(msg)
    data = unpack("<iifBfffBBIfB", msg)
    print(data)
