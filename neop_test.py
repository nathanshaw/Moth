import board
import nepixel

neop = neopixel.NeoPixel(board.D21, 12)

neop.brightness = 50

neop.auto_write = True

neop.fill((0, 255, 0))
