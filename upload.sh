#!/bin/bash
echo "COMPILING"
arduino-cli compile -p "COM4" -b arduino:avr:nano ~/Documents/software/ledDriver
echo "COMPILING DONE"

echo "UPLOADING"
arduino-cli upload -b arduino:avr:nano:cpu=atmega328old -p COM3 -i ~/Documents/software/ledDriver/ledDriver.arduino.avr.nano.hex
echo "UPLOADING COMPLETE"
rm *.hex *.elf
exit
