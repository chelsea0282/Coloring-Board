# Color Board 
ECE 3140 Final Project Spring 2018

## Beginning Steps
### Setting up Accelorometer 
### Testing TFT
- tested the TFT screen using an Arduino Uno
- tests of testing
  1. show colors on the screen
  2. writ letters on the screen
  3. fill screenw ith color then writing on top
  4. print an image on the screen
     -> had to upload through SD card b/c too large of data
  5. (possibly later) print multiple different images. 
  6. (possibly later) 
 
## Connecting TFT and FRDM   
### SPI connection between TFT and FRDM
- referenced lots of SPI library online to base ours
- found a tutorial PDF for setting up SPI
    https://community.nxp.com/docs/DOC-103944
    file:///Users/seungyeonhan/Downloads/KSDK%20SPI%20Master-Slave%20with%20FRDM-K64F.pdf
### Physical Connection
FRDM Wiring: https://os.mbed.com/platforms/FRDM-K64F/
TFT Wiring: https://learn.adafruit.com/adafruit-2-dot-8-color-tft-touchscreen-breakout-v2/spi-wiring-and-test
Using the acclerometer on the FRDM board, had to allow long enough connection so that the board will move freely but also maintain the connection with the FRDM board. Decided to use a breadboard to maintain a stable connection between the TFT board and the breadboard, and breadboard to the FRDM board. However, have to keep in mind that SPI works best in close distances. 
  

## Accelerometer
-Setup originally from Lab 6 handout

## Applying Accelerometer Data
- created variables:
  *current position x
  *current position y
  *current position z
  *previous position x
  *previous position y
  *previous position z
  *velocity x
  *velocity y
  *velocity z
