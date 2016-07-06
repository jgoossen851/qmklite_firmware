# The MIT License (MIT)
# 
# Copyright (c) 2016 Fred Sundvik
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

GFXLIB = $(VISUALIZER_DIR)/ugfx
SRC += $(VISUALIZER_DIR)/visualizer.c
UINCDIR += $(GFXINC) $(VISUALIZER_DIR)

ifdef LCD_ENABLE
UDEFS += -DLCD_ENABLE
ULIBS += -lm
USE_UGFX = yes
endif

ifdef LCD_BACKLIGHT_ENABLE
SRC += $(VISUALIZER_DIR)/lcd_backlight.c
ifndef EMULATOR
SRC += lcd_backlight_hal.c
endif
UDEFS += -DLCD_BACKLIGHT_ENABLE
endif

ifdef LED_ENABLE
SRC += $(VISUALIZER_DIR)/led_test.c
UDEFS += -DLED_ENABLE
USE_UGFX = yes
endif

ifdef USE_UGFX
include $(GFXLIB)/gfx.mk
SRC += $(GFXSRC)
UDEFS += $(patsubst %,-D%,$(patsubst -D%,%,$(GFXDEFS)))
ULIBS += $(patsubst %,-l%,$(patsubst -l%,%,$(GFXLIBS)))
endif

ifndef VISUALIZER_USER
VISUALIZER_USER = visualizer_user.c
endif
SRC += $(VISUALIZER_USER)

ifdef EMULATOR
UINCDIR += $(TMK_DIR)/common
endif