#!/bin/bash
echo "COMPILING"
./build.sh
echo "COMPILING DONE"

echo "UPLOADING"
arduino-cli upload -b arduino:avr:nano -p COM3 -i build/arduino.avr.nano/MultimausIO.ino.hex
echo "UPLOADING COMPLETE"
rm *.hex *.elf
exit

#:cpu=atmega328old
