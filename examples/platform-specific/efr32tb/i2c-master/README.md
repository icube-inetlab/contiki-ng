
author : Pacheu Maxime
This example is designed to work on Platform efr32tb for the Board stghw00036.
This example is supposed to work with the a slave that sends interrupts
to the stghw00036 board, to trigger the I2C write/read operations.

The interrupt is set up on the SPI MOSI pin, used as an input pin.

This example also runs a full IPv6 stack with 6LoWPAN and RPL.
It is possible, for example to ping such a node.

```
make TARGET=efr32tb GECKO_SDK_PATH=$HOME/Documents/gecko_v2.2/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite BOARD=xlbee
```
