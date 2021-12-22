######################################
# target
######################################
TARGET = homeserv

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og

#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
main.c \

#######################################
# binaries
#######################################
CC = gcc

#######################################
# CFLAGS
#######################################
# C includes
C_INCLUDES =  \
-Ihttpserver.h \

CFLAGS = $(C_INCLUDES)

ifeq ($(DEBUG), 1)
CFLAGS += -g
endif

#######################################
# LDFLAGS
#######################################
# libraries
LIBS = 
LIBDIR = 
LDFLAGS = $(LIBDIR) $(LIBS)

# default action: build all
all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET)

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: %.c 
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir -p $@	

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)
