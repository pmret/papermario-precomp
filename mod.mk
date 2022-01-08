MOD_DIR := mods/$(MOD_NAME)
MOD_C_FILES := $(shell find $(MOD_DIR) -type f -name "*.c")

MOD_OBJS += $(addprefix $(BUILD_DIR)/, $(MOD_C_FILES:.c=.o))
MSGS += $(foreach file,$(shell find $(MOD_DIR) -type f -name "*.msg"),$(BUILD_DIR)/$(file).c)
