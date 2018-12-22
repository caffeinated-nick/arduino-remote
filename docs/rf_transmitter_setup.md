# RF Transmitter Setup

While running the Arduino setup should involve the ESP-13 shield connected directly to the Arduino Uno. The RF Transmitter should then be connected on the *ESP-13* side of the ESP board:

| **ESP-13**    | **RF-Transmitter** |
|---------------|--------------------|
| 2-D           | DATA               |
| 2-3V3         | VCC                |
| 2-G           | GND                |
| None          | ANT                |

ANT is the fourth connector on the transmitter, and should have a wire connected with nothing on the other end. ANT is of course short for Antenna, and this loose wire will act as such.

**Caution:** if you wish to alter this setup keep in mind that the code will expected the RF Transmitter to be on port 2.