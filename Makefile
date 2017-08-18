# this value should be calibrated for more accurate voltage measurements
REF_VOLTAGE ?= 1100

# this address should be unique for each cell monitor on the serial bus
CELL_ADDRESS ?= 1

build:
	REF_VOLTAGE=${REF_VOLTAGE} CELL_ADDRESS=${CELL_ADDRESS} pio run

clean:
	pio run -t clean

flash:
	REF_VOLTAGE=${REF_VOLTAGE} CELL_ADDRESS=${CELL_ADDRESS} pio run -t upload

.PHONY: build clean flash
