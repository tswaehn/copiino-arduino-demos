# makefile
# by sven.ginka@gmail.com
# 21.dec.2013
#
# run in command line
# invoke: "sudo make compile_and_run"
#
# please note that we need 'sudo' here to access hardware
# any hints on how-to remove the sudo for hardware access
# are welcome. 
# 

# config
ARDUINO_DIR=/usr/share/arduino
TARGET=blinky
BOARD_TAG=copiino
ARDUINO_PORT=/dev/spidev0.0
ARDUINO_LIBS= 
TOOLS_DIR=/usr/share/arduino/hardware/tools/
BOARDS_TXT=$(TOOLS_DIR)boards.txt

# include default build-process
include /usr/share/arduino/Arduino.mk

# execute the build-process as usual but use
# my own reset and programming routines
#
compile_and_run: all my_reset my_ispload my_start
	
my_reset:	
	@echo "--- reseting chip ---"
	@$(TOOLS_DIR)check-sudo.sh
	@$(TOOLS_DIR)reset-chip.sh
	@echo "done"
	
my_ispload:
	@echo "--- checking fuses ---"
	@$(TOOLS_DIR)check-fuses.sh
	@echo "--- uploading program to chip ---"
	@$(TOOLS_DIR)burn-chip.sh $(TARGET_HEX)

my_start:
	@echo "--- run uploaded program ---"
	@$(TOOLS_DIR)run-chip.sh
	@echo "finished!"
	@echo "thank you - have fun"


