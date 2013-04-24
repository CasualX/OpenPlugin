# Module name
NAME = OpenPlugin

# Set to yes if you want to compile in debug mode
DEBUG = no

# Directory where the module will go after the build is done
CP_PATH = "../addons/$(NAME).so"

# Linker & compiler - g++ or clang++ can be used
CC = g++

# link flags for your mod, make sure to include any special libraries here
LDFLAGS = bin/libtier0.so -shared -m32

# flags passed to the c compiler
CFLAGS = \
	-march=pentium3 -mmmx -msse -m32 -std=c++11 \
	-fvisibility=hidden -fvisibility-inlines-hidden \
	-fno-strict-aliasing -Wno-delete-non-virtual-dtor -Wno-unused -Werror \
	-D_LINUX -DLINUX -DOPENPLUGIN_EXPORTS -DUSE_FANCY_CPP_FEATURES

ifeq ($(CC), g++)
	CFLAGS += -mfpmath=sse
endif
	
# special flags depending if we are in release or debug mode
ifeq ($(DEBUG), yes)
	CFLAGS += -g3 -DEBUG
else
	CFLAGS += -O3 -DNDEBUG
	LDFLAGS += -s
endif

# the dir we want to put binaries we build into
BUILD_DIR = Release_Linux
# the place to put object files
BUILD_OBJ_DIR = obj

#####################################################################

PLUGIN_OBJS = \
	$(BUILD_OBJ_DIR)/advspec.o \
	$(BUILD_OBJ_DIR)/game.o \
	$(BUILD_OBJ_DIR)/ifaces.o \
	$(BUILD_OBJ_DIR)/name.o \
	$(BUILD_OBJ_DIR)/openfov.o \
	$(BUILD_OBJ_DIR)/plugin.o \
	$(BUILD_OBJ_DIR)/sdk.o \
	$(BUILD_OBJ_DIR)/stdafx.o \
	$(BUILD_OBJ_DIR)/transvm.o \
	$(BUILD_OBJ_DIR)/util.o \
	$(BUILD_OBJ_DIR)/vmthooks.o \

all: dirs $(NAME).so cp

dirs:
	-mkdir -p $(BUILD_OBJ_DIR)
	-mkdir -p $(BUILD_DIR)

$(NAME).so: $(PLUGIN_OBJS)
	$(CC) $(PLUGIN_OBJS) $(LDFLAGS) -o $(BUILD_DIR)/$@

$(BUILD_OBJ_DIR)/%.o: %.cpp
	$(CC) -I . $(CFLAGS) -o $@ -c $<

cp:
	-cp "$(BUILD_DIR)/$(NAME).so" $(CP_PATH)

clean:
	-rm -rf $(BUILD_OBJ_DIR)
	-rm -f $(NAME).so
