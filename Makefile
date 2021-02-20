MAKEFLAGS += --no-builtin-rules
BUILD_DIR = build
TARGET_LIST = exercise1 exercise2
INCLUDE_DIR = include
SOURCE_DIR = source

#TODO - -lm should not be in CFLAGS

OBJ_FLAGS = -flto
LDFLAGS = $(OBJ_FLAGS)
CFLAGS = -march=native -O3 -lm -Wall -Werror

#Get targets in build dir
_TARGET_LIST = $(TARGET_LIST:%=$(BUILD_DIR)/%)
#Get dependency filed in build dir
_DEP_LIST = $(TARGET_LIST:%=$(BUILD_DIR)/%.d)

_INCLUDE_FLAGS = $(INCLUDE_DIR:%=-I %)

all: $(_TARGET_LIST)

#Include all dependency rules we have generated
include $(_DEP_LIST)

#Specifics for exercise2
exercise2_DEPS = exercise2.o 2d_geometry.o
exercise2_LIBS = glfw GL

_exercise2_DEPS = $(exercise2_DEPS:%=$(BUILD_DIR)/%)
_exercise2_LD_FLAGS = $(exercise2_LIBS:%=-l %)

#Specific rule for generating target binary from object files
build/exercise2: $(_exercise2_DEPS)
	gcc $(CFLAGS) $(LDFLAGS) $(_INCLUDE_FLAGS) $(_exercise2_LD_FLAGS) $(_exercise2_DEPS) -o $@;


#Generic rules ↓↓↓

#Generic rule for generating dependency file from c-file
$(BUILD_DIR)/%.d: $(SOURCE_DIR)/%.c
	gcc $(_INCLUDE_FLAGS) $< -MM -MP -MT $(BUILD_DIR)/$* -MF $(BUILD_DIR)/$*.d;

#Generic rule for generating target binary from c file
$(BUILD_DIR)/%: $(SOURCE_DIR)/%.c
	gcc $(CFLAGS) $(_INCLUDE_FLAGS) $< -o $@;

#Generic rule for generating an object file from c file
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	gcc $(CFLAGS) $(OBJ_FLAGS) -c $(_INCLUDE_FLAGS) $< -o $@;


.PHONY: clean diff

clean:
	rm -r $(BUILD_DIR)/*

# @ silences the printing of the command
# $(info ...) prints output 
diff:
	$(info The status of the repository, and the volume of per-file changes:)
	@git status
	@git diff --stat