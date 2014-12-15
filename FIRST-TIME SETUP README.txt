Welcome to the Photogate Lister project!

Some quick things to get you started:
-You can download the latest Arduino software from www.arduino.cc

-The programmer that Mr. Robertson uses as of this writing is the USBTinyISP. Windows does NOT have built-in drivers for this device. You can download these drivers from https://learn.adafruit.com/usbtinyisp. HOWEVER, these drivers are not signed, and will fail to install on any version of Windows that enforces drivers to be signed (so Windows Vista and above). Please follow the appropriate instructions for your version of Windows to disable this functionality before installation.

-The Atmel chip on these boards as of this writing is the ATmega88, which is not natively supported by the Arduino software. You will need to edit the "boards.txt" file included in the Arduino software (under hardware\Arduino) to support it. A sample of this file, called "boards_SAMPLE.txt" for Arduino V.1.0.5-r2 has been included as an example to help you in editing; look for "Robertson 88" in the sample file.

-AVOID BURNING BOOTLOADERS! They take up a lot of valuable space in the chip's flash memory to no benefit, and a proper bootloader hasn't been made/found yet for the ATmega88 (as evidenced in the "boards_SAMPLE.txt" file - it uses a completely-unrelated chip's bootloader as a space-filler). You can risk bricking the chip with this if you're not careful.

-The model of the chip that operates the screen is the NXP SAA1064. However, most of the functionality of this chip has already been wrapped in functions and/or explained in commentary, so you can avoid having to dig up it's Data Sheet.


Good Luck!