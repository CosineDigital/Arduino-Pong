# Arduino-Pong
![gameplay](https://raw.githubusercontent.com/CosineDigital/Arduino-Pong/master/static/gameplay.png)

## Features
- Support for 2 players
- Easy-to-use configuration file to customize gameplay
- Fast and controlled Arduino-to-computer communication
- Efficient Arduino code - only sends data when required

## Circuit diagram
Below is a circuit diagram that includes one button for the Arduino controller. All resistors are 10k ohms. 5V power supply. 
![circuit-diagram](https://raw.githubusercontent.com/CosineDigital/Arduino-Pong/master/static/circuit-diagram.png)

Below is the complete circuit diagram that includes all 5 buttons for the aruino controller.
Pins 13-9 are used for input. See [firmware.ino](https://github.com/CosineDigital/Arduino-Pong/blob/master/src/arduino/firmware/firmware.ino) for more information.
![circuit-diagram-large](https://raw.githubusercontent.com/CosineDigital/Arduino-Pong/master/static/circuit-diagram-large.png)

## 3rd party libraries
This project was made possible with the following open-source libraries:
- [GLFW](https://github.com/glfw/glfw)
- [glad](https://github.com/Dav1dde/glad)
- [stb_image](https://github.com/nothings/stb)
- [GLM](https://github.com/g-truc/glm)
