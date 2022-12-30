import time
import board

from adafruit_as726x import AS726x_I2C  # for I2C

i2c = board.I2C()  # board SCL SDA
sensor = AS726x_I2C(i2c)

sensor.conversion_mode = sensor.MODE_2  # continuos read on all channels

while True:  # wait for data to be ready
    while not sensor.data_ready:
        sensor.driver_led = True
        time.sleep(0.1)

    print("\n")
    print("R: "+sensor.violet)  # 610nm
    print("S: "+sensor.blue)  # 680nm
    print("T: "+sensor.green)  # 730nm
    print("U: "+sensor.yellow)  # 760nm
    print("V: "+sensor.orange)  # 810nm
    print("W: "+sensor.red)  # 860nm
    print("Temp: "+sensor.temperature)

    time.sleep(1)
