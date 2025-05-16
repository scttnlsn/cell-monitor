build:
	pio run

clean:
	pio run -t clean

fuse:
	avrdude -v -p attiny85 -c usbasp -P /dev/ttyUSB* -b 19200 -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m

flash:
	pio run -t upload

.PHONY: build clean fuse flash
