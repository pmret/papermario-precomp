TARGET := papermario

# Directories
SRC_DIR := src
BUILD_DIR := build

SRC_DIRS := $(shell find $(SRC_DIR)/ -type d)
BUILD_SRC_DIRS := $(addprefix $(BUILD_DIR)/,$(SRC_DIRS))

# Tools
CROSS := mips-linux-gnu-

CC      := ../papermario/tools/build/cc/gcc/gcc -B ../papermario/tools/build/cc/gcc/
AS      := $(CROSS)as
LD      := $(CROSS)ld
OBJCOPY := $(CROSS)objcopy
MKDIR   := mkdir -p
RMDIR   := rm -rf
CKSUM   := tools/n64crc

ifeq ($(shell uname -s),Darwin)
CPP     := cpp-11
else
CPP     := cpp
endif

# Inputs/outputs
ELF := $(BUILD_DIR)/$(TARGET).elf
Z64 := $(ELF:.elf=.z64)
ELF_IN := ../papermario/ver/us/build/$(TARGET).elf
Z64_IN := $(BUILD_DIR)/$(TARGET)_in.z64
Z64_IN_OBJ := $(Z64_IN:.z64=.o)

C_SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
C_OBJS := $(addprefix $(BUILD_DIR)/, $(C_SRCS:.c=.o))
A_SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.s))
A_OBJS := $(addprefix $(BUILD_DIR)/, $(A_SRCS:.s=.o))

OBJS := $(C_OBJS) $(A_OBJS) $(Z64_IN_OBJ)

# Flags
CFLAGS      := -c -ffreestanding -mfix4300 -G 0 -Wall
CPPFLAGS    := -Iinclude -I../papermario/include -I../papermario/ver/us/build/include -DF3DEX_GBI_2 -D_LANGUAGE_C
OPTFLAGS    :=
ASFLAGS     := -EB -march=vr4300 -mtune=vr4300 -Iinclude
LD_SCRIPT   := $(TARGET).ld
LDFLAGS     := -T $(BUILD_DIR)/$(LD_SCRIPT) -mips3 --accept-unknown-input-arch --no-check-sections
CPP_LDFLAGS := -P -Wno-trigraphs -DBUILD_DIR=$(BUILD_DIR) -Umips -DBASEROM=$(Z64_IN_OBJ)
BINOFLAGS   := -I binary -O elf32-big
Z64OFLAGS   := -O binary

# mips-linux-gnu toolchain specifics
ifeq ($(CROSS),mips-linux-gnu-)
CFLAGS      += -mfix4300 -mhard-float -fno-common -fno-PIC -mno-abicalls -fno-inline-functions -ffreestanding
endif

# Rules
all: $(Z64)

$(BUILD_DIR) $(BUILD_SRC_DIRS) :
	$(MKDIR) $@

$(BUILD_DIR)/%.o : %.c | $(BUILD_SRC_DIRS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OPTFLAGS) $< -o $@

$(BUILD_DIR)/%.o : %.s | $(BUILD_SRC_DIRS)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/$(LD_SCRIPT) :  $(LD_SCRIPT)
	$(CPP) $(CPP_LDFLAGS) $< -o $@

$(ELF) : $(OBJS) $(BUILD_DIR)/$(LD_SCRIPT) $(ELF_IN)
	$(LD) -R $(ELF_IN) $(LDFLAGS) -Map $(@:.elf=.map) -o $@
	
$(Z64_IN) : $(ELF_IN) | $(BUILD_DIR)
	$(OBJCOPY) $(Z64OFLAGS) $< $@

$(Z64_IN_OBJ) : $(Z64_IN) | $(BUILD_DIR)
	$(OBJCOPY) $(BINOFLAGS) $< $@
	
$(Z64) : $(ELF) $(CKSUM)
	$(OBJCOPY) $(Z64OFLAGS) $< $@
	$(CKSUM) $@

$(CKSUM) : $(CKSUM).c
	gcc -O3 $< -o $@ -Wall -Wextra -Wpedantic

clean:
	$(RMDIR) $(BUILD_DIR)

.PHONY: all clean

print-% : ; $(info $* is a $(flavor $*) variable set to [$($*)]) @true

