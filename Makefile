build:
	pio run

flash:
	pio run -t upload

.PHONY: build flash
