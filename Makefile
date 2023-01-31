CC=g++
CFLAGS=-c -pipe -O3 -std=gnu++20

TARGET=$(if $(ENV_TARGET),$(ENV_TARGET),out)
BUILD_DIR=.build
SOURCE_DIR=src

SOURCES=$(shell find $(SOURCE_DIR) -name '*.cpp')
OBJECTS=$(SOURCES:%=$(BUILD_DIR)/%.o)
DEPENDENCIES=$(OBJECTS:.o=.d)

CPPFLAGS=-MMD -MP

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(ENV_FLAGS) -c $< -o $@

PHONY: test
test:
	@ENV_FLAGS="-D TESTING_ENV" ENV_TARGET="test" $(MAKE)
	@./.build/test

PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

-include $(DEPENDENCIES)
