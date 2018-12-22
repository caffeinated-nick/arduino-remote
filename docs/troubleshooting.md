# Troubleshooting

### Remote and fans are out of sync.
**Possible causes**: Someone turned off the fans from the switch (the Arduino has no way of retrieving the state of the fans, it can only maintain its own internal state).
1. Access the webpage.
2. Hit "Kill Switch"

### Remote isn't returning the webpage.
**Possible causes**: The remote is stuck trying to return the webpage (unfortunately this code is blocking).
1. Reset the Arduino.
2. Access the webpage.
3. Hit "Kill Switch" to get the remote and fans in sync.

### Pressing buttons on the remote doesn't impact the fans.
**Possible causes**: The RF transmitter has had some issue and isn't sending signals anymore.
1. Confirm that the [physical connections are all setup properly](rf_transmitter_setup.md).
2. Reset the Arduino
3. Access the webpage.
4. Hit "Kill Switch" to resync the fans.
