CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

BOARD_ID ?= EL_04002_22
BOARD_USAGE ?= SBFront1

TARGET_ELF ?= production.elf
TARGET_HEX ?= production.hex
TARGET_MAP ?= production.map

BUILD_DIR ?= ./build_$(BOARD_ID)_$(BOARD_USAGE)
SRC_DIRS ?= ./

VPATH = \
	. \
	Applications \
	Drivers \
	../Shared/Applications \
	../Shared/Drivers \
	../Shared/RTOS \
	../Shared/RTOS/FreeRTOS/Source \
	../Shared/RTOS/FreeRTOS/Source/portable/GCC/ARM_CM0 \
	../Shared/RTOS/FreeRTOS/Source/portable/MemMang

SRCS := \
	main.c \
	app_main.c \
	app_sensor.c \
	app_statuslight.c \
	drv_adc_private.c \
	drv_adc.c \
	drv_can_private.c \
	drv_can.c \
	drv_clock_private.c \
	drv_clock.c \
	drv_hsd.c \
	drv_main.c \
	drv_ws2812b_private.c \
	drv_ws2812b.c \
	port.c \
	heap_3.c \
	croutine.c \
	event_groups.c \
	list.c \
	queue.c \
	stream_buffer.c \
	tasks.c \
	timers.c \
	startup_samc21j18a.c \
	system_samc21j18a.c
	
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := \
	Applications \
	Drivers \
	../Shared/Applications \
	../Shared/Drivers \
	../Shared/SAM \
	../Shared/CMSIS \
	../Shared/RTOS \
	../Shared/RTOS/FreeRTOS/Source/include \
	../Shared/RTOS/FreeRTOS/Source/portable/GCC/ARM_CM0
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) \
	-MMD -MP \
	-mcpu=cortex-m0plus -D__SAMC21E18A__  -mthumb \
	-ffunction-sections -mlong-calls \
	-DBOARD_ID=EL_04002_22 -DBOARD_USAGE=SBFront1 \
	-Wall -Wextra

LDFLAGS ?= -mcpu=cortex-m0plus  -D__SAMC21E18A__   -T"samc21j18a_flash.ld"  -mthumb --specs=nosys.specs -Wl,--gc-sections

$(BUILD_DIR)/$(TARGET_ELF): $(BUILD_DIR) $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) -Wl,-Map=$(BUILD_DIR)/$(TARGET_MAP)

$(BUILD_DIR)/$(TARGET_HEX): $(BUILD_DIR)/$(TARGET_ELF)
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

# c source
$(BUILD_DIR)/%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
ifeq ($(OS),Windows_NT)
	mkdir $(subst /,\\,$@)
else
	mkdir -p $@
endif

.PHONY: clean production test

clean:
	$(RM) -r $(BUILD_DIR)

production: CPPFLAGS += -Os
production: $(BUILD_DIR)/$(TARGET_HEX)

debug: CPPFLAGS += -Og -DDEBUG -g
debug: $(BUILD_DIR)/$(TARGET_ELF)

test: $(BUILD_DIR) $(BUILD_DIR)/test_app_statuslight
	qemu-arm -cpu cortex-m0 $(BUILD_DIR)/test_app_statuslight

$(BUILD_DIR)/test_app_statuslight.o: Tests/test_app_statuslight.c
	$(CC) $(INC_FLAGS) -MMD -MP -mcpu=cortex-m0plus -D__SAMC21E18A__  -mthumb -Wall -Wextra -c $< -o $@
	
$(BUILD_DIR)/test_app_statuslight: $(BUILD_DIR)/test_app_statuslight.o $(BUILD_DIR)/app_statuslight.o
	$(CC) $(BUILD_DIR)/test_app_statuslight.o $(BUILD_DIR)/app_statuslight.o -o $@ -mcpu=cortex-m0plus  -D__SAMC21E18A__ --specs=rdimon.specs 

-include $(DEPS)

