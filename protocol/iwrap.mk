OPT_DEFS += -DHOST_IWRAP

SRC +=	iwrap.c \
	suart.S \
	sendchar_uart.c \
	uart.c


# Search Path
VPATH += $(TOP_DIR)/protocol/iwrap
