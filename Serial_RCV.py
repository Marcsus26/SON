import serial

ser = serial.Serial('usb:1130000', 9600) # Remplacez COM3 par votre port
with open('recu.wav', 'wb') as f:
    while True:
        data = ser.read()
        if not data: break
        f.write(data)