.PHONY: mcu clean flash

FIRMWARE_DIR = ./mcu/firmware/

mcu:
	$(MAKE) -C $(FIRMWARE_DIR) all

flash:
	st-flash write $(FIRMWARE_DIR)/build/firmware.bin 0x08000000

clean:
	$(MAKE) -C $(FIRMWARE_DIR) clean
