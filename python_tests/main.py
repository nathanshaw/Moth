import board
import busio
import adafruit_tca9548a
import adafruit_veml7700
import neopixel
from time import sleep

# lux sensors - veml7700
i2c = busio.I2C(board.SCL, board.SDA)
tca = adafruit_tca9548a.TCA9548A(i2c)
veml1 = adafruit_veml7700.VEML7700(tca[0])
veml2 = adafruit_veml7700.VEML7700(tca[1])
lux_min = 20
lux_max = 6000

# neopixels
neop = neopixel.NeoPixel(board.D21, 12)
pixel_num = 12
neop.brightness = 0.2
neop.auto_write = True
bright_f = 0.2
bright_r = 0.2

def constrainLux(lux, min=lux_min, max=lux_max):
    """
    This function constrains the lux value according to
    min and max.If lux is larger than max then max will be
    the output. If lux is less than min then min will be the
    output

    INPUTS
    -------------------
    lux - the lux value as captured from the veml7700 sensor
    min - the lowest lux value which will be allowed
    max - the maximum lux value which will be allowed
    """
    return min(max(min, lux), max)

def mapLux(lux, min=min, max=max):
    """
    This function returns a value between 0 and 1, where 0 equals min
    and 1 equals max.

    INPUTS
    -------------------
    lux - the lux value as captured from the veml7700 sensor
    min - the lowest lux value which is allowed
    max - the maximum lux value which is allowed

    """
    lux_range = lux_max - lux_min
    return (lux - lux_min) / lux_range

def constrainAndMapLux(lux, min=lux_min, max=lux_max):
    """
    This function first constrains the lux value according to
    min and max before passing the constrained value into the
    map function which returns a value between 0 and 1, where 0 equals min
    and 1 equals max

    INPUTS
    -------------------
    lux - the lux value as captured from the veml7700 sensor
    min - the lowest lux value which will be allowed
    max - the maximum lux value which will be allowed
    """
    return mapLux(constrainLux(lux, min, max))

def testLuxToBrightness()
    print("lux (front/back)     : ", veml1.lux, " | ", veml2.lux)
    bright_f = constrainAndMapLux(veml1.lux)
    bright_r = constrainAndMapLux(veml2.lux)
    print("lux mapped to bright : ", bright_f, " | ", bright_r)
    print("|-"*20)
    neop.fill((255,0,0));
    neop.brightness = bright_f
    time.sleep(2.5)

while __name__ == "__main__":
    while True:
        testLuxToBrightness()
        time.sleep(2.5)
