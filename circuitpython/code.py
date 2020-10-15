import board
import busio
import time

uart = busio.UART(board.TX, board.RX, baudrate=115200)

def send(message):
    message = message + "\r\n"
    uart.write(bytes(message, 'utf-8'))
while  True :
    print("Sending...");
    send("test/circuitpython/hello")
    time.sleep(1)
