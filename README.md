# Color Board 
ECE 3140 Final Project Spring 2018

# Introduction
Coloring Board allows user to color on top of an image - similar to a coloring book. However, there is a slight twist in the method of coloring as the magnetometer on the FRDM board is incorporated to allow coloring.

The user will be able to color on the image displayed on an LCD screen through tilting the TFT board. The data from the magnetometer (on the FRDM board) which will give information regarding the orientation of the board will be used to map coordinates.

# High Level Design
## Description of how Major Components interact
The FRDM board is primarily responsible for the majority of our project’s creation. One of the important functions is to obtain data from its onboard magnetometer. The board thus reads the magnetometer state continuously and then operates with the x and y components of the data. These components are translated to pseudo-displacements stored in variables representing the cursor’s current location with respect to the TFT display. Assuming the draw functionality is not suspended, a circle is then created at the aforementioned location and filled to simulate the drawing of a point. The communication of this data from the FRDM to the TFT implements an SPI library we converted to C for FRDM. The process of generating locations and drawing circles is repeated as the user manipulates the FRDM board to emulate a rolling draw functionality (essentially like rolling an ink covered ball on a screen). However, the user also has the option to suspend the drawing function by pressing the SW2 switch on the board. Then the drawing point can be moved without dragging a line across the screen.

## System Diagram
The software can be most simply divided into 4 main blocks the main function which will actually call on all the implementation and the functions of magnetometer and the tft library. The magnetometer and the tft provides abstraction and provides function for the main method to call on. Similarly, the SPI provides abstraction which the tft methods can call on. The relationship and the work will be further explained in the sections below. 


 

## Explain (and justify major design choices)
In creating the project there were several key design choices for implementation. The first of which was that we would use SPI to communicate with the TFT board. The rationale behind this decision was that the TFT was able to be operated with only 2 data pins from the FRDM when using SPI, whereas using I2C would have needed 6 data pins. The SPI communication was written from scratch so that the code will be cleaner and easier to understand and use. There were a lot of pre-existing SPI library files for Arduino, PI and other frequently used microcontrollers, but non existed for FRDM board. Additionally, within the SPI library (named tft.c & tft.h), a lot of helper functions were included to include a level of abstraction in which will allow easier integration of the project on the software level.

There was consideration whether to implement the magnetometer or accelerometer for displacement data. After several days of working with the accelerometer it was determined that the measurements were too noisy . Then the switch was made to the magnetometer, and while data collected was still not ideal, it was easier to have more control in the rate of position adjustment.

The next consideration was how fast the board would be updated with new points. The primary considerations were to make as smooth of a drawing as possible while not drawing too frequently as to slow down the response time of the screen. As such, it was found that an acceptable frequency of drawing was approximately 100 Hz. The next decision was how to minimize the noise factor of the magnetometer data. The method we implemented was to create an adaptive zeroing and then thresholding. Upon board restart there is a slight delay so that 1000 magnetometer states can be measured, and the average x and y components found. These averages are then subtracted from the following magnetometer states to effectively zero them. The next step is to then apply a threshold so natural noise in the data isn’t harshly affecting the control’s performance. The threshold decided was a value of 15 on the zeroed readings, as we wanted to preserve as sensitive of movements as possible, but the natural variation observed in measured data when not moving was approximately 10. Staying on the trek of magnetometer data, when converting it to a position and storing this position, we elected to use global variables instead of a pointer to an object of type position. The reasoning for this will be further discussed in the section for components that didn’t work, but the general idea was to gain as much speed as possible. The final design choices made centered around usability. The onboard LEDs were implemented in order to allow the user to readily see whether or not drawing was suspended. Thus, a green LED is displayed while the board is in its drawing mode, and the red LED is on when the drawing is suspended.

The final consideration was to not use external buttons, but rather use the contained SW2 to toggle drawing active and paused. The decision was made in order to reduce the number of external components that would need to be wired to the board, and as such reducing its ease of use.

# Hardware Design

 

The only peripheral we had was a TFT LCD Board from Adafruit.

LCD Display 2.8" TFT LCD with Touchscreen Breakout Board w/MicroSD Socket - ILI9341

PRODUCT ID: 1770, (already have) https://www.adafruit.com/product/1770

# Software Design
The protocol used to communicate with the peripheral device is SPI. SPI was chosen because it uses less data lines and TFT already had a library available for SPI as well as hardware pin selects. The primary data structure used for our position tracking was simply two double values, one for the x and y position respectively. These variables were used as the first attempt was made using a pointer to a position type variable that stored the x and y coordinates. However, updating the position of the pointer was much slower than using the integers, and as such it was decided simply using two variables would be the best approach to speed up our program. Most frequently repeated routine which the code users is to first, obtain the magnetometer data, then update the position based on the data from magnetometer. Ever millisecond, function to draw a figure on the TFT board will be called according to the position data. The function will draw on the TFT through SPI protocol. Additional routine is when the on button is pushed, the drawing will be paused but will allow the position to change.

All parts of the code was written by ourselves. The magnetometer set up was done following the guidelines given, but the implementation and the algorithm as to collecting the magnetometer data and analyzing the data was written ourselves. As for the SPI protocol, it was also written by ourselves, but inspiration was drawn from Arduino SPI library. Additionally, for the TFT commands, they parts were copied from the TFT Adafruit library given on the Adafruit website's link.

While implementing, there were certain parts that required adjustments due to failures and inefficiency. The first component of our project that failed was attempting to use the accelerometer to track displacements. Despite our best efforts to rectify the data through zeroing and thresholding as described earlier, the data was simply too noisy and inaccurate to accurately track the positions of the board. The evidence for this was that while debugging by outputting the calculated displacements, it was found that the accelerometer would quickly max out in one direction while the board was stationary. The solution then was to move to the magnetometer data from the board.

The next aspect of our initial design that failed was attempting to implement the position of the drawing cursor with a pointer to data type position_t that had fields for the x and y displacements. However, in debugging it was found that this implementation was too slow to update the values, and thus the position tracking was moved to global integers which greatly increased the update speed.

# Testing
The first phase of testing involving the board was focused with best implementing the accelerometer data. In order to analyze the data received PuTTY was employed to give a read out of the data collected. Once the data was known to be appropriately received, testing began on how to best utilize the data. The first approach was to zero the data by collecting samples while at rest and then subtracting their mean from later accelerometer data collected. However, due to noise it was determined through testing that thresholding would also be ideal to reduce the effects of noise on the data. However, after both of these steps, the data was too variable to use effectively, and thus the switch was made to the magnetometer data. The thresholding and zeroing methods were still used, but this time the results were much better with the control of position being much more sensitive. On the aspect of board control, the interrupts essential to our device were also tested in the debugger and real time. The debugger allowed us to see that both the PIT and switch interrupt were triggered as expected and performed the actions exactly as we wanted, and in the real time test it was useful to base the interrupt triggering off of LEDs. However, the LEDs did limit the precision of operation we were able to see.

The other aspect of testing focused around creating effective communications between the FRDM board and the TFT display. These tests began with first implementing several basic functions in Arduino and then ensuring that they worked as anticipated to get a good high level understanding before moving on to the more low level recreation. Once the Arduino aspect was understood fully, testing for FRDM board implementation was delayed until a final working implementation was created as the only test was whether or not images were appropriately displayed to the TFT. After catching a few minor errors the TFT interaction was perfected and was able to run smoothly, only hindered by the display’s own limitations for how fast it could draw.

# Results

The project was able to end in success, but this does not mean that there were not some modifications along the way. The first major change came when issues with using the accelerometer arose. After several days of testing it was found that the accelerometer was simply too error prone and inconsistent to accurately be used for our project. Once the issue was discovered it was determined that the best path forward would be to use the magnetometer instead. The next variation from the intended proposal is that there was not enough time to be able to implement additional features as originally desired, i.e. the ability to change line thickness or color. The final adjustment made was to use an onboard button (the SW2) instead of connecting an external button to the board in order to make it more user friendly by limiting the number of wire connections created that could hinder movement. Besides these differences, the project was able to be carried out the rest of the way in the manner we proposed originally.

The design was primarily assessed qualitatively as there are no explicit parameters/ time constraints for functionality, but rather how well it functions as a whole. Thus, the qualitative analysis was the primary determination on how well our project worked. After operating the board there are several issues that arose. The first of which was that the suspend drawing button did not work exactly as intended and would occasionally not register with the board. The next issue that arose was too sporadic of movements with the FRDM would create odd reactions in the drawing as the position was likely changing too much. However, the overall function was present and thus the intended goal was accomplished as best as possible.

Our design had no considerable safety concern for the user.

# Conclusions
The final design is a bit different from the initial imagined design. Initially, we envisioned the user will be moving the FRDM board as they would move a pencil and the user will be able to watch their strokes be displayed on the TFT board. The FRDM would have been attached to the TFT board, the main reasoning behind this being the short wiring between TFT and FRDM. However, as the accelerometer data turned out to be too noisy especially after integrating it twice, we switched over to use the magnetometer data. And because magnetometer is based on the orientation of the board, the method of coloring changed to something that would be similar to a ball covered in ink rolling around the display screen. Even though this was not a result we had expected, it seems like a better solution because it will allow more intuitive interaction with the user. With the initial design, the user would have had to move the FRDM board and watch the TFT display that would have been attached to the FRDM - a rather unintuitive design. The final design is more intuitive as the FRDM and TFT is still attached, but it will be easier to watch the screen as the FRDM will not have to move, only tilt.

However, we were unable to add fun parts that we wished to while writing the proposal due to complications that were caused elsewhere. If we were to work on a similar project scale next time, we would make sure to work on the hardware entirely and get that working before moving into the software. Additionally, we will try to integrate the separate work more frequently between the group members.

We don’t believe that there is any ability for us to publish our creation; however all of our work was created from our own skills or by converting open source files down to the C level. As such, there are no concerns with patents or any other trademark laws.

# References you used
FRDM Users Manual Guide

Mapping of the FRDM board - https://os.mbed.com/platforms/FRDM-K64F/

Wiring for the TFT Board and tutorials on how to set it up

https://learn.adafruit.com/adafruit-2-dot-8-color-tft-touchscreen-breakout-v2/spi-wiring-and-test

# Peripherals
The only external peripheral that was used for the project was a TFT LCD board which was connected through the use of several ports (PINOUTs) on the FRDM board. The TFT board is able to display preloaded images from a micro-SD card, and then allow for drawings to be added on top of it by communicating drawing specific info via SPI or I2C protocols. The board is also able to be used to read touch input on the screen itself, but that aspect was not implemented in our project.
