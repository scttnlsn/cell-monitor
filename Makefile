build:
	pio run

clean:
	pio run -t clean

flash:
	pio run -t upload

.PHONY: build clean flash
