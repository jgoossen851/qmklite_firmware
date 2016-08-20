STARTING_MAKEFILE := $(firstword $(MAKEFILE_LIST))
ROOT_MAKEFILE := $(lastword $(MAKEFILE_LIST))
ROOT_DIR := $(dir, $(ROOT_MAKEFILE))
ifeq ($(ROOT_DIR),)
    ROOT_DIR := .
endif
ABS_STARTING_MAKEFILE := $(abspath $(STARTING_MAKEFILE))
ABS_ROOT_MAKEFILE := $(abspath $(ROOT_MAKEFILE))
ABS_STARTING_DIR := $(dir $(ABS_STARTING_MAKEFILE))
ABS_ROOT_DIR := $(dir $(ABS_ROOT_MAKEFILE))
STARTING_DIR := $(subst $(ABS_ROOT_DIR),,$(ABS_STARTING_DIR))

PATH_ELEMENTS := $(subst /, ,$(STARTING_DIR))

define NEXT_PATH_ELEMENT
    $$(eval CURRENT_PATH_ELEMENT := $$(firstword  $$(PATH_ELEMENTS)))
    $$(eval PATH_ELEMENTS := $$(wordlist  2,9999,$$(PATH_ELEMENTS)))
endef

$(eval $(call NEXT_PATH_ELEMENT))

ifeq ($(CURRENT_PATH_ELEMENT),keyboards)
    $(eval $(call NEXT_PATH_ELEMENT))
    KEYBOARD := $(CURRENT_PATH_ELEMENT)
    $(eval $(call NEXT_PATH_ELEMENT))
    ifeq ($(CURRENT_PATH_ELEMENT),keymaps)
        $(eval $(call NEXT_PATH_ELEMENT))
        KEYMAP := $(CURRENT_PATH_ELEMENT)
    else ifneq ($(CURRENT_PATH_ELEMENT),)
        SUBPROJECT := $(CURRENT_PATH_ELEMENT)
        $(eval $(call NEXT_PATH_ELEMENT))
        ifeq ($(CURRENT_PATH_ELEMENT),keymaps)
            $(eval $(call NEXT_PATH_ELEMENT))
            KEYMAP := $(CURRENT_PATH_ELEMENT)
        endif
    endif
endif

$(info $(ROOT_DIR)/keyboards)
KEYBOARDS := $(notdir $(patsubst %/.,%,$(wildcard $(ROOT_DIR)/keyboards/*/.)))

$(info Keyboard: $(KEYBOARD))
$(info Keymap: $(KEYMAP))
$(info Subproject: $(SUBPROJECT))
$(info Keyboards: $(KEYBOARDS))


# Compare the start of the RULE_VARIABLE with the first argument($1)
# If the rules equals $1 or starts with $1-, RULE_FOUND is set to true
#     and $1 is removed from the RULE variable
# Otherwise the RULE_FOUND variable is set to false
# The function is a bit tricky, since there's no built in $(startswith) function
define COMPARE_AND_REMOVE_FROM_RULE
    RULE_FOUND := false
    ifeq ($1,$$(RULE))
        RULE:=
        RULE_FOUND := true
    else
        STARTDASH_REMOVED=$$(subst START$1-,,START$$(RULE))
        ifneq ($$(STARTDASH_REMOVED),START$$(RULE))
            RULE_FOUND := true
            RULE := $$(STARTDASH_REMOVED)
        else
            RULE_FOUND := false
        endif
    endif
endef

# Recursively try to find a match
# $1 The list to be checked
# If a match is found, then RULE_FOUND is set to true
# and MATCHED_ITEM to the item that was matched
define TRY_TO_MATCH_RULE_FROM_LIST
    ifneq ($1,)
        $$(eval $$(call COMPARE_AND_REMOVE_FROM_RULE,$$(firstword $1)))
        ifeq ($$(RULE_FOUND),true)
            MATCHED_ITEM := $$(firstword $1)
        else 
            $$(eval $$(call TRY_TO_MATCH_RULE_FROM_LIST,$$(wordlist 2,9999,$1)))
        endif
    endif
endef

define ALL_IN_LIST_LOOP
    OLD_RULE$1 := $$(RULE)
    $$(eval $$(call $1,$$(ITEM$1)))
    RULE := $$(OLD_RULE$1)
endef

define PARSE_ALL_IN_LIST
    $$(foreach ITEM$1,$2,$$(eval $$(call ALL_IN_LIST_LOOP,$1)))
endef

define PARSE_ALL_KEYBOARDS
    $$(eval $$(call PARSE_ALL_IN_LIST,PARSE_KEYBOARD,$(KEYBOARDS)))
endef

define PARSE_ALL_KEYMAPS
    $$(eval $$(call PARSE_ALL_IN_LIST,PARSE_KEYMAP,$$(KEYMAPS)))
endef

define PARSE_ALL_SUBPROJECTS
    ifeq ($$(SUBPROJECTS),)
        $$(eval $$(call PARSE_SUBPROJECT,))
    else
        $$(eval $$(call PARSE_ALL_IN_LIST,PARSE_SUBPROJECT,$$(SUBPROJECTS)))
    endif
endef

# $1 Subproject
define PARSE_SUBPROJECT
    CURRENT_SP := $1
    KEYMAPS := $$(notdir $$(patsubst %/.,%,$$(wildcard $(ROOT_DIR)/keyboards/$$(CURRENT_KB)/keymaps/*/.)))
    $$(eval $$(call COMPARE_AND_REMOVE_FROM_RULE,allkm))
    ifeq ($$(RULE_FOUND),true)
        $$(eval $$(call PARSE_ALL_KEYMAPS))
    else
        $$(eval $$(call TRY_TO_MATCH_RULE_FROM_LIST,$$(KEYMAPS)))
        ifeq ($$(RULE_FOUND),true)
            $$(eval $$(call PARSE_KEYMAP,$$(MATCHED_ITEM)))
        endif
    endif
endef

# $1 = Keyboard
define PARSE_KEYBOARD
    CURRENT_KB := $1
    # A subproject is any keyboard subfolder with a makefile
    SUBPROJECTS := $$(notdir $$(patsubst %/Makefile,%,$$(wildcard $(ROOT_DIR)/keyboards/$$(CURRENT_KB)/*/Makefile)))
    $$(eval $$(call COMPARE_AND_REMOVE_FROM_RULE,allsp))
    ifeq ($$(RULE_FOUND),true)
        $$(eval $$(call PARSE_ALL_SUBPROJECTS))
    else
        $$(eval $$(call TRY_TO_MATCH_RULE_FROM_LIST,$$(SUBPROJECTS)))
        ifeq ($$(RULE_FOUND),true)
            $$(eval $$(call PARSE_SUBPROJECT,$$(MATCHED_ITEM)))
        endif
    endif
endef

# $1 Keymap
define PARSE_KEYMAP
    CURRENT_KM = $1
    COMMANDS += KEYBOARD_$$(CURRENT_KB)_SUBPROJECT_$$(CURRENT_SP)_KEYMAP_$$(CURRENT_KM)
    COMMAND_KEYBOARD_$$(CURRENT_KB)_SUBPROJECT_$(CURRENT_SP)_KEYMAP_$$(CURRENT_KM) := Keyboard $$(CURRENT_KB), Subproject $$(CURRENT_SP), Keymap $$(CURRENT_KM)
endef

define PARSE_RULE
    RULE := $1
    COMMANDS :=
    $$(eval $$(call COMPARE_AND_REMOVE_FROM_RULE,allkb))
    ifeq ($$(RULE_FOUND),true)
        $$(eval $$(call PARSE_ALL_KEYBOARDS))
    else
        $$(eval $$(call TRY_TO_MATCH_RULE_FROM_LIST,$$(KEYBOARDS)))
        ifeq ($$(RULE_FOUND),true)
            $$(eval $$(call PARSE_KEYBOARD,$$(MATCHED_ITEM)))
        endif
    endif
endef

RUN_COMMAND = echo "Running": $(COMMAND_$(COMMAND));

.PHONY: %
%:
	$(eval $(call PARSE_RULE,$@))
	$(foreach COMMAND,$(COMMANDS),$(RUN_COMMAND))

.PHONY: all-keyboards
all-keyboards: allkb

.PHONY: all-keyboards-defaults
all-keyboards-defaults: allkb-default-default

.PHONY: all
all: 
	echo "Compiling"