LUFA_DIR = protocol/lufa

# Path to the LUFA library
LUFA_PATH = $(TOP_DIR)/protocol/lufa/LUFA-120219

# Create the LUFA source path variables by including the LUFA root makefile
include $(LUFA_PATH)/LUFA/makefile

LUFA_SRC = $(LUFA_DIR)/lufa.c \
	   $(LUFA_DIR)/descriptor.c \
	   $(LUFA_SRC_USB)
SRC += $(subst $(LUFA_PATH)/,,$(LUFA_SRC))

# Search Path
VPATH += $(LUFA_PATH)

# Option modules
#ifdef $(or MOUSEKEY_ENABLE, PS2_MOUSE_ENABLE)
#endif

#ifdef EXTRAKEY_ENABLE
#endif

# LUFA library compile-time options and predefined tokens
LUFA_OPTS  = -D USB_DEVICE_ONLY
LUFA_OPTS += -D FIXED_CONTROL_ENDPOINT_SIZE=8
LUFA_OPTS += -D FIXED_NUM_CONFIGURATIONS=1
LUFA_OPTS += -D USE_FLASH_DESCRIPTORS
LUFA_OPTS += -D USE_STATIC_OPTIONS="(USB_DEVICE_OPT_FULLSPEED | USB_OPT_REG_ENABLED | USB_OPT_AUTO_PLL)"

OPT_DEFS += -DF_USB=$(F_USB)UL
OPT_DEFS += -DARCH=ARCH_$(ARCH)
OPT_DEFS += $(LUFA_OPTS)
OPT_DEFS += -DHOST_LUFA
