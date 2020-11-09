import board
import busio
import digitalio
import time

class Network:
    def __init__(self, tx=board.TX, rx=board.RX, baudrate=115200):
        self.uart = uart = busio.UART(tx, rx, baudrate=115200)

    def send(self, message, ack=True):
        message = message + "\r\n"
        self.uart.write(bytes(message, 'utf-8'))
        if ack is True:
            data = self.receive()
            if data == "AT+OK":
                return True
            else :
                return False

    def receive(self):
        data = self.uart.readline()
        if data is not None:
            data = ''.join([chr(b) for b in data]).strip()
        return data
