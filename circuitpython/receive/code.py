import board
import busio
import time
import usini_modem
import digitalio

led = digitalio.DigitalInOut(board.LED)
led.direction = digitalio.Direction.OUTPUT
modem = usini_modem.Network()

while True:
    data = modem.receive()
    if data is not None:
        print(data)
    if data == "gateway1/rx/led_on":
        led.value = False
    if data == "gateway1/rx/led_off":
        led.value = True
