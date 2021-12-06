#!/bin/bash

arduino-cli compile -p "COM4" -b arduino:avr:nano  -e
#arduino-cli upload -b arduino:avr:nano:oldbootloader -p COM4 -i ~/Documents/software/piTrain-decoder/piTrain-decoder.arduino.avr.nano.hex
#rm *.hex *.elf
exit


