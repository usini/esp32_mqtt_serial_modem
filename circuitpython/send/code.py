import usini_modem

modem = usini_modem.Network()

# Send with check
ok = False
while not ok:
    ok = modem.send("test/hello world")
print("Message sent")

# Send without check
"""
modem.send("test/hello world")
"""