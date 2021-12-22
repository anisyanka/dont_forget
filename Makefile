.PHONY: all mcu home-server clean flash on-lamp off-lamp setup-uart-port

FIRMWARE_DIR = ./mcu/firmware/
HOME_SERVER_DIR = ./home_server

all: mcu home-server

mcu:
	$(MAKE) -C $(FIRMWARE_DIR) all

flash:
	st-flash write $(FIRMWARE_DIR)/build/firmware.bin 0x08000000

home-server:
	$(MAKE) -C $(HOME_SERVER_DIR) all

setup-uart-port:
	sudo chmod 666 /dev/ttyUSB0 
	stty -F /dev/ttyUSB0 115200

on-lamp:
	@echo "L1" > /dev/ttyUSB0

off-lamp:
	@echo "L0" > /dev/ttyUSB0

clean:
	$(MAKE) -C $(FIRMWARE_DIR) clean
	$(MAKE) -C $(HOME_SERVER_DIR) clean
