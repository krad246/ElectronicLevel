# Electronic Level
This is an electronic 'bubble level' using the MSP430G2553, an ADXL325 accelerometer, and a custom LED board. The device lights up the ring of LEDs based on its orientation.

# Operation
On boot, a 'calibration sequence' starts. For the first 4 steps, aim the highlighted LED down at the ground and press the button. For the 5th step, hold the device face up and press the button. For the last step, hold the device face down and press the button.

# How It Works
The device performs a number of tasks:
- Analog sampling and quantization for accelerometer readings
- Digital filtering (EMA) to smooth readings out
- Fixed-point arctangent to calculate spherical coordinate angles corresponding to the orientation
- Table-lookup methods to determine LED direction and intensity
- SPI to transmit LED direction as a bit vector to the LED driver
- PWM to manage LED intensity

# Future Plans
Port the code to C++ to take full advantage of classes, etc. 