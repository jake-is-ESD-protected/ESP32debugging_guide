# ESP32debugging_guide
Various tests on the ESP32 and ESP-Prog JTAG debugger

Based on [this guide](https://flowduino.com/2021/09/02/debugging-esp32-programs-properly-esp-prog/).

## Making it work

>Beware! This only works in exactly this configuration, anything else is not tested!

My findings include:

1. Hardware: ESP32 WROOM from AZ-Delivery (This is important!) and the ESP-Prog
2. Instead of connecting the ESP32 via USB, use the programmer-interface as a USB-replacement and the JTAG-interface for debugging
3. Supply the ESP32 with 5V into its Vin/5V pin from the programmer-interface (this needs the jumper on the ESP-prog to be changed)
4. Only the ESP-prog should be connected via USB, check the COM-port
5. Wiring is documented in the guide above, but the VCC and GND wires from the JTAG-interface are not needed
6. In total, 10 cables should be used
7. Install the FTDI-drivers
8. Get software ZADIG and replace the FTDI-driver of RS232 interface 0 with WINUSB
9. Set up the `platform.ini` file like mine
10. BE SURE TO SELECT **EXACTLY** THE BOARD YOU ARE USING OR YOU WILL RUN INTO ALL SORTS OF ERRORS! This guide uses the `az-delivery-devkit-v4`

>If you are having upload issues, press the boot-button down and click upload again. It should then find itself uploading and blinking correctly.