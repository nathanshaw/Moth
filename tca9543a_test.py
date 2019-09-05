from py_i2c_register.register_list import RegisterList
from py_i2c_register.register import Register
from py_i2c_register.register_segment import RegisterSegment
import adafruit_veml7700
import board
import busio

i2c = busio.I2C(board.SCL, board.SDA)
veml = adafruit_veml7700.VEML7700(i2c)

tca_rst = 4
controls = RegisterList(0x70, i2c, {})


