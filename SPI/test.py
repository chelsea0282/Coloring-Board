import RPi.GPIO as GPIO
import spidev
import time

class TFT():
  def __init__(self):
    self.width = 320
    self.height= 240
    self.freq = 8000000
    self.clk = 11
    self.mosi = 10
    self.miso = 9
    self.cs = 8
    self.dc = 25
    self.rst = 22

    GPIO.setup(self.clk, GPIO.OUT)
    GPIO.setup(self.mosi, GPIO.OUT)
    GPIO.setup(self.miso, GPIO.IN)
    GPIO.setup(self.cs, GPIO.OUT)
    GPIO.setup(self.dc, GPIO.OUT)
    GPIO.setup(self.rst, GPIO.OUT)
    #self.spi = spidev.SpiDev()

  def set(self, pin, value):
    GPIO.output(pin, value)

  def get(self, pin):
    return GPIO.input(pin)

  def write_spi(self, value):
    bit = 0x80
    while bit > 0:
      if value & bit == bit:
        self.set(self.mosi, 1)
      else:
        self.set(self.mosi, 0)

      self.set(self.clk, 0)
      self.set(self.clk, 1)
      bit = bit >> 1

  def write_spi32(self, value):
    self.write_spi(value >> 24)
    self.write_spi(value >> 16)
    self.write_spi(value >> 8)
    self.write_spi(value >> 0)

  def read_spi(self):
    r = 0
    for i in range(8):
      self.set(self.clk, 0)
      self.set(self.clk, 1)
      r = r << 1
      if self.get(self.miso) == 1:
        r = r | 0x01

    return r

  def write_data(self, data):
    for b in data:
      self.write_spi(b)

  def write_command(self, cmd):
    self.set(self.dc, 0)
    self.write_spi(cmd)
    self.set(self.dc, 1)

  def write(self, cmd, data):
    self.write_command(cmd)
    self.write_data(data)

  def open(self):
    self.set(self.cs, 0)
    self.set(self.clk, 0)
    self.set(self.mosi, 0)

  def close(self):
    self.set(self.cs, 1)

  def read_command(self, cmd):
    self.open()
    self.write(0xD9, [0x10])
    self.write_command(cmd)
    r = self.read_spi()
    self.close()
    return r

  def setAddrWindow(self, x, y, w, h):
    xa = (x << 16) | (x + w - 1)
    ya = (y << 16) | (y + h - 1)
    self.write_command(0x2A) # ILI9341_CASET
    self.write_spi32(xa);
    self.write_command(0x2B) # ILI9341_PASET
    self.write_spi32(ya);
    self.write_command(0x2C) # ILI9341_RAMWR

  def writeColor(self, c, l):
    while l > 0:
      self.write_spi(c >> 8)
      self.write_spi(c & 0xFF)
      l -= 1

  def begin(self):
    self.set(self.dc, 0)
    self.set(self.rst, 0)
    time.sleep(0.5)
    self.set(self.rst, 1)
    time.sleep(0.5)
    self.open()
    # SPI_BEGIN()

    self.write(0xEF, [0x03, 0x80, 0x02])
    self.write(0xCF, [0x00, 0xC1, 0x30])
    self.write(0xED, [0x64, 0x03, 0x12, 0x81])
    self.write(0xE8, [0x85, 0x00, 0x78])
    self.write(0xCB, [0x39, 0x2C, 0x00, 0x34, 0x02])
    self.write(0xF7, [0x20])
    self.write(0xEA, [0x00, 0x00])

    self.write(0xC0, [0x23]) # Power control 1
    self.write(0xC1, [0x10]) # Power control 2
    self.write(0xC5, [0x3E, 0x28]) # VCM control 1
    self.write(0xC7, [0x86]) # VCM control 2
    self.write(0x36, [0x48]) # Memory access control
    self.write(0x37, [0x00, 0x00]) # Vertical scroll

    self.write(0x3A, [0x55]) # ILI9341_PIXFMT
    self.write(0xB1, [0x00, 0x18]) # ILI9341_FRMCTR1
    self.write(0xB6, [0x08, 0x82, 0x27]) # ILI9341_DFUNCTR
    self.write(0xF2, [0x00])
    self.write(0x26, [0x01]) # ILI9341_GAMMASET
    self.write(0xE0, [0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00]) # ILI9341_GMCTRP1
    self.write(0xE1, [0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F]) # ILI9341_GMCTRN1

    self.write_command(0x11) # Sleep out
    time.sleep(0.12)

    self.write_command(0x29) # Display on
    time.sleep(0.12)

    self.close()

  def fillScreen(self, x, y, w, h, c):
    self.open()
    self.setAddrWindow(x, y, w, h)
    l = w * h
    self.writeColor(c, l)
    self.close()

if __name__ == "__main__":
  GPIO.setmode(GPIO.BCM)

  tft = TFT()
  tft.begin()
  time.sleep(1)
  print(tft.read_command(0x0A)) # ILI9341_RDMODE
  print(tft.read_command(0x0B)) # ILI9341_RDMADCTL
  print(tft.read_command(0x0C)) # ILI9341_RDPIXFMT
  print(tft.read_command(0x0D)) # ILI9341_RDIMGFMT
  print(tft.read_command(0x0F)) # ILI9341_RDSELFDIAG

  # Fill screen
  # setAddrWindow
  tft.fillScreen(100, 100, 100, 100, 0xF800) # ILI9341_RED

  GPIO.cleanup()
