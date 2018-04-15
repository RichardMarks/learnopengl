COMPILER ?= clang++
CFLAGS ?= $(shell pkg-config glew glfw3 --cflags) -O0
LDFLAGS ?= $(shell pkg-config glew glfw3 --libs) -Wl,-headerpad_max_install_names -framework OpenGL

TARGET_EXEC ?= game
TARGET_DIR ?= ./bin
BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
SRCS := $(shell find $(SRC_DIRS) -name *.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS ?= $(INC_FLAGS) -MMD -MP -g

$(TARGET_DIR)/$(TARGET_EXEC): $(OBJS)
	$(MKDIR_P) $(TARGET_DIR)
	$(COMPILER) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(COMPILER) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(TARGET_DIR)

-include $(DEPS)
MKDIR_P ?= mkdir -p
