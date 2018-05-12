# Color Board 
ECE 3140 Final Project Spring 2018

## Beginning Steps
### Setting up Accelorometer
Setup originally from Lab 6 handout
### Testing TFT
- tested the TFT screen using an Arduino Uno
- Parts of the TFT board had to be soldered together following the instuctions in: https://learn.adafruit.com/adafruit-3-5-color-320x480-tft-touchscreen-breakout/spi-wiring-and-test
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
    *https://www.nxp.com/docs/en/reference-manual/KSDK20APIRM.pdf?&fpsp=1&WT_TYPE=Reference%20Manuals&WT_VENDOR=FREESCALE&WT_FILE_FORMAT=pdf&WT_ASSET=Documentation&fileExt=.pdf
    page 1000 is some SPI stuff
### Physical Connection
FRDM Wiring: https://os.mbed.com/platforms/FRDM-K64F/
TFT Wiring: https://learn.adafruit.com/adafruit-2-dot-8-color-tft-touchscreen-breakout-v2/spi-wiring-and-test
Using the acclerometer on the FRDM board, had to allow long enough connection so that the board will move freely but also maintain the connection with the FRDM board. Decided to use a breadboard to maintain a stable connection between the TFT board and the breadboard, and breadboard to the FRDM board. However, have to keep in mind that SPI works best in close distances. 

## Applying Accelerometer Data
- created variables:
  1. current position x
  2. current position y
  3. current position z
  4. previous position x
  5. previous position y
  6. previous position z
  7. velocity x
  8. velocity y
  9. velocity z


(Chelsea) - pick up from here
https://stackoverflow.com/questions/2003505/how-do-i-delete-a-git-branch-both-locally-and-remotely
https://learn.adafruit.com/circuitpython-basics-i2c-and-spi/spi-devices
https://www.arduino.cc/en/Reference/TFTLibrary
