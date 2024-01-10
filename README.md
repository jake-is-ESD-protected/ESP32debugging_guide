# ESP32 Debugging guide
There's a high chance that you came here while being either unknowing or frustrated. Need the most in-depth ESP32 debugging guide? Here it is. 

I'll cover debugging a classic ESP32 WROOM and the newer ESP32-C3 on both Windows and Linux with PlatformIO on VSCodium. Don't know what that is? Check out my video on it to get started with my recommended IDE for ESP32 development and then come back for some debugging! Let's get to it.  


## ESP32 WROOM
If you just have an "ESP32" then the chance is high that you have the classic WROOM model which is on many development boards for ESP32s. To be sure, read the small text on the metal casing above the chip on your board.

> **WARNING:** Debugging this board is **only** possible with an external device that supports the JTAG protocol. A cheap device that I can recommend is the Espressif **ESP-PROG**, which I will show how to use in this video. If you don't have a JTAG device, save this video, buy one, stop wasting your time with trying to debug your ESP32-WROOM and come back when the device arrives.

Got your device? Prepare your ESP32, a breadboard, 6 equally long male to female jumper cables, a micro-USB-cable and your ESP-PROG. Free up the pins from 12 to 15 on your ESP32, those are directly connected to the chip's JTAG lines on the board. Connect these 4 data lines according to this image and table:

![esp-prog-hookup](https://docs.platformio.org/en/latest/_images/esp-prog_jtag_connector.png)

| ESP-PROG Pin | ESP32 Pin |
| ------------ | --------- |
| 2 (TMS)      | 14        |
| 4 (TCK)      | 13        |
| 6 (TDO)      | 15        |
| 8 (TDI)      | 12        | 

Of course, you connect one of the GND pins to the ESP's GND pin and connect pin 1 (VDD) of the ESP-PROG with a pin labeled "5V" or "VIN" on your ESP. It's important to switch the little jumper on the ESP-PROG to 5V next to the connection bay for the JTAG lines. Plug your USB-micro cable into the ESP-PROG and connect it to your PC. Both the ESP-PROG and the ESP32 should now light their power LED (if they have one). Notice that the USB-port of the ESP32 is left alone. This also means, that we won't be using `Serial` with this approach. For some extra info on that, see the additional notes on the ESP-PROG later on.

### Installation on Windows
As always, despite Window's millions of drivers the one you need is always missing. So let's install them. 

The ESP-PROG contains a USB-to-Serial and USB-to-JTAG chip by company **FTDI** and Windows doesn't know them by default. Head to their website and download the executable driver installation file found [here](https://ftdichip.com/wp-content/uploads/2021/08/CDM212364_Setup.zip). After installing, the ESP-PROG will now feed through both the programming and debugging interface as a serial device. 

However, this is still not right, so we have to install some additional drivers. Get the software [Zadig](https://zadig.akeo.ie/), which will be used to overwrite the drivers for these serial devices. After install and opening, go to `Options`-`List all devices`. Then select `Dual RS232-HS (Interface 0)` and notice how the driver is listed as `FTDIBUS (vx.xx.xx.x)`. Now exchange that with the dropdown menu next to the arrow with `WinUSB (vx.x.xxxx.xxxxx)` and hit replace driver. Unplug the ESP-PROG and plug it in again. You are now ready to debug.

### Installation on Linux
Here drivers are easier to install, albeit in a non intuitive fashion. We need to enable the device drivers that PlatformIO utilizes. Luckily, they list all their supported devices in a single so called `udev.rules` file which we can download and dump into the right folder in our system. Open a terminal and write

yes, this is one line:
```
curl -fsSL https://raw.githubusercontent.com/platformio/
platformio-core/develop/platformio/assets/system/99-
platformio-udev.rules | sudo tee /etc/udev/rules.d/99-
platformio-udev.rules
```
(check the repo in the description for copy+paste!)<!-- {{remove}}-->

If your screen is now full of text listing all sorts of microcontrollers, don't worry, it worked. Restart your `udev` service with `sudo service udev restart` and add your user to the "dialout" group like this:

Ubuntu/Debian/Zorin:
```bash
sudo usermod -a -G dialout $USER
sudo usermod -a -G plugdev $USER
sudo service restart udev
```
Arch:
```bash
sudo usermod -a -G uucp $USER
sudo usermod -a -G lock $USER
sudo service restart udev
```
### Debugging in PlatformIO
As said before, if you don't know PlatformIO, a super versatile IDE for embedded development that makes the old Arduino IDE look like your grandma's carpet, check out my video on it.  It is important that you know what board you have, otherwise the correct hardware mapping will be impossible. So simply having an "ESP32" won't cut it this time. For example, I am currently using the **AZ-Delivery Devkit V4** with an ESP32 WROOM on it. Check out PlatformIO's website to see which boards are available (it's a lot).
Within `platformio.ini` add the following configuration:

```ini
[env:az-delivery-devkit-v4]
platform = espressif32
board = az-delivery-devkit-v4
framework = arduino
monitor_speed = 115200
debug_tool = esp-prog
upload_protocol = esp-prog
debug_init_break = tbreak setup
debug_speed = 1000
build_type = debug
debug_build_flags = -O0 -g -ggdb
```
* `[env:az-delivery-devkit-v4]` is a specifier for the parameters below it. You can write multiple specifiers if you have code that is able to run on different systems. This will then pop up at the bottom of your VSCodium window.
* `platform = espressif32` denotes the ESP platform as opposed to for example STM or Atmel.
* `board = az-delivery-devkit-v4` specifies the board we are using. Such a board is often unique in its regard to the combination of chip and hardware peripherals, so be careful that you choose the right board.
* `framework = arduino` specifies the framework for programming. With the Arduino FW we can use simple things like `Serial` or `digitalWrite`, which is different on other frameworks such as the ESP-IDF or the STM HAL.
* `monitor_speed` is the baudrate of `Serial`, in case you have it connected. This is redundant for this example.
* `debug_tool = esp-prog` tells PlatformIO to use our connected ESP-PROG for debugging.
* `upload_protocol = esp-prog` tells PlatformIO to also upload your program through the ESP-PROG even without debugging.
* `debug_init_break = tbreak setup` is a flag to insert a first hidden breakpoint on the first line inside the `setup()` function. If you don't specify this, the debugger will halt even before `setup()` is called, which is code not supposed to be seen when using the Arduino FW.
* `debug_speed = 1000` is the clock speed for the JTAG protocol. Keep in mind, this value here is in kHz and can go faster than this. However, it depends on the length and setup of your cables, so you better start at 1000 and go up to 5000.
* `build_type = debug` tells PlatformIO to build not in release mode but in debug mode, which is what we want.
* `debug_build_flags = -O0 -g -ggdb` is a list of flags that can be appended to your debug build process. Ever handled a Makefile? This works the same way. The flags `-O0 -g -ggdb` tell the compiler to leave everything as is without optimization, because otherwise some variables may be optimized out during debugging, which makes the process harder to understand. Of course in a release build, you will want optimization.

Save the file, wait for it to update and then hit `F5` or navigate to `Run`-`Debug`. You will see your project compile again for debug mode and it will open up the debug console where a few handshakes will be made and your program will be uploaded. After that, the program will stop on the first line in `setup()`. You can now navigate line by line, set breakpoints by clicking the space next to row numbers and run to the next breakpoint. You might have an issue with navigating to code from included libraries because they are not yours. And of course you won't be able to look into precompiled libraries.
Anything you print with `Serial` will not appear on the debugger, because the UART-line `Serial` uses goes from the ESP32-WROOM chip to your boards USB-to-UART converter chip and from there to the USB port. Since that is not connected, nothing will be received. If you still want that, check out the additional notes on the ESP-PROG.

### Possible errors
#### Stuff related to `libUSB()`:
* Check if *Zadig* changed drivers to `WINUSB` (Windows)
* Check if `udev.rules` includes your board (Linux)
This is a driver issue. Be sure that Zadig overwrote the default drivers with `WINUSB` on Windows or that your `udev.rules` file lists the ESP32-Prog. As of now, installing these drivers on an ARM device with Windows 11 does not work. If it still doesn't work, be sure that it's not an update issue by rebooting your PC and checking again.

#### Stuff related to flash regions:
* Check board type in `platformio.ini`
* Make sure the type matches/is similar
Be sure that you selected the right board. There are many and some names are confusing. Look them up on the PlatformIO website.

#### Other weird stuff:
- Check voltage at ESP32
- Keep JTAG-cables short
- Keep JTAG cables away from PWM signals!
- Get a new ESP32
Be sure that all your power connections are stable and that all involved cables are short and far away from cables that drive motors or LEDs or very active buses. The hard pill to swallow is, that more ESPs arrive dysfunctional than you think. As much as I hate that fact myself, consider buying them in larger quantities to avoid being stuck with one defect ESP32.

### Additional notes on the ESP-PROG and ESP32-flashing in general
Wonder why the ESP-PROG shows up as two devices on your computer? You might have noticed a smaller patchbay next to the JTAG one on your ESP-PROG. When you turn the board around you will see the pins

`TX`  
`RX`  
`EN`  
`IO`  
`GND`  
`VDD`  

This is the so called "programmer interface" and with it you do exactly what you would normally do with an ESP32 directly connected to a USB-cable: **Flashing it**. 
On the ESP32-WROOM programs are flashed using the UART TX and RX lines. Yes, you are right, before your program even runs, `Serial` already transported your program to the ESP32, although it was not called `Serial` at that point, but they were the same copper wires. But how can the ESP32 then differentiate between an incoming `Serial` message or a new program flashing? That's done by resetting IO (sometimes called BOOT) and ENABLE (sometimes called RST). Pulling RST low puts the controller in reset-state and pulling BOOT low before RST is also low puts it into flashing mode. It is only then, that the ESP32 is ready to receive a new program. 

The smart foxes among you might have noticed that most boards feature buttons that are exactly labeled like that. But how does flashing with just a USB cable work then at all? This would require the USB line of the directly connected USB cable to have these BOOT and EN lines, which they don't or the user would have to press these buttons every time a new program gets flashed.

![schematic](https://i0.wp.com/www.exasub.com/wp-content/uploads/2022/08/ESP32-module-schematics.jpg)
This is where Espressif installed the whackiest hardware hack ever. Let's take a look at the schematic. Remember the USB-to-UART converter? Besides the TX and RX lines it additionally breaks out two lines "RTS" (request to send) and "DTR" (data terminal ready), which are part of the UART protocol but aren't necessary in many applications. Espressif sends the outputs of these lines to this weird transistor circuit (bottom left corner), which is then again sent to the BOOT and RST pins. So by sending a specific serial signal, the transistors switch the BOOT and RST pins in the right order and the ESP accepts a new program. (*Be careful not to confuse RTS (request to send) and RST (reset)!*)

## Debugging the ESP32-C3
If you got one of the newer and more powerful RISC-V ESP32-C3s, then you're in luck, you don't need any additional hardware; **almost**. The ESP32-C3 has a USB-to-JTAG converter **inside** the chip, which means that all we have to do is interface the ESP32-C3 with USB directly. For this, we somehow need to get the USB data lines `D-` connected to pin 18 and `D+` connected to pin 19. You can achieve this by either splicing an unloved USB-cable or you get one of those USB-micro breakout boards. Here you can completely bypass the ESP32-C3 onboard USB-micro connector if you additionally hook up the USB cable's `VBUS` to `5V` and `GND` to `GND` on the ESP32-C3 board.

### Installation Windows
On windows, you also need the driver updater tool "Zadig" described in the [chapter above](#esp32-wroom). Here, you need to go to `Options` and click `list all devices`. Then, select the JTAG interface 2 and downgrade the driver from `WINUSBv10.x.xxxx.xxxxx` to `WINUSBv6.x.xxxx.xxxxx`. Again, you might want to reboot your PC.

### Installation on Linux
Good news! If you installed the `udev.rules` mentioned in the guide for the ESP32-WROOM, then you don't have to do anything! If you don't, repeat the steps from the ESP32-WROOM for Linux.

### Debugging in PlatformIO
Back in PlatformIO, all we have to do is change some lines from the `platformio.ini` example from before.

```ini
[env:esp32-c3-devkitm-1]
platform = espressif32
board = esp32-c3-devkitm-1
framework = arduino
monitor_speed = 115200
debug_tool = esp-builtin
upload_protocol = esp-builtin
debug_init_break = tbreak setup
debug_speed = 1000
build_type = debug
debug_build_flags = -O0 -g -ggdb
```
Since we selected a different board with a different chip, we will update the name, board and debug tool. The debug tool is now fittingly called `esp-builtin`.  Press `F5` like before and let the debugging begin! That's it!

