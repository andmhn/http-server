# tool macros
CC := gcc
CFLAGS := -pedantic-errors -Wall -Wextra -Werror -Wconversion -Wfloat-equal  -Wundef  -Wshadow  -Wpointer-arith -Wstrict-overflow=5 -Wunreachable-code # -fsanitize=address -fno-omit-frame-pointer
DBGFLAGS := -g
COBJFLAGS := $(CFLAGS) -c
PREFIX := ~/.local

# path macros
BIN_PATH := bin
OBJ_PATH := $(BIN_PATH)/obj
SRC_PATH := src
DBG_PATH := debug

# compile macros
TARGET_NAME := http-server
ifeq ($(OS),Windows_NT)
	TARGET_NAME := $(addsuffix .exe,$(TARGET_NAME))
endif
TARGET := $(BIN_PATH)/$(TARGET_NAME)
TARGET_DEBUG := $(DBG_PATH)/$(TARGET_NAME)
TARGET_TEST := $(BIN_PATH)/test-$(TARGET_NAME)

# src files & obj files
TEST_SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,_test.c*)))
TEST_OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(TEST_SRC)))))
TEST_TARGET_SRC := $(TEST_SRC:_test.c=.c)
TEST_TARGET_OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(TEST_TARGET_SRC)))))

SRC := $(filter-out $(TEST_SRC), $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c*))))
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

OBJ_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))
DEPENDS := $(OBJ:.o=.d)
DEP_DEBUG := $(OBJ_DEBUG:.o=.d)
DEP_TEST := $(TEST_OBJ:.o=.d)


# clean files list
DISTCLEAN_LIST := $(OBJ) \
				  $(DEPENDS) \
                  $(OBJ_DEBUG) \
				  $(DEP_DEBUG) \
				  $(TEST_OBJ) \
				  $(DEP_TEST)

CLEAN_LIST := $(TARGET) \
			  $(TARGET_DEBUG) \
			  $(DISTCLEAN_LIST) \
			  $(TARGET_TEST)

# default rule
default: makedir all

-include ($(DEPENDS))

# non-phony targets
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) $(COBJFLAGS) -MMD -MP -o $@ $<

$(DBG_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) $(COBJFLAGS) $(DBGFLAGS) -MMD -MP -o $@ $<

$(TARGET_DEBUG): $(OBJ_DEBUG)
	$(CC) $(CFLAGS) $(DBGFLAGS) $(OBJ_DEBUG) -o $@

# test targets
$(TARGET_TEST): LDLIBS += -lcriterion
$(TARGET_TEST): $(TEST_OBJ) $(TEST_TARGET_OBJ)
	$(CC) $(LDFLAGS) $(LDLIBS) -MMD -MP -o $@ $^

# phony rules
.PHONY: makedir
makedir:
	@mkdir -p $(BIN_PATH) $(OBJ_PATH) $(DBG_PATH)

.PHONY: all
all: $(TARGET)

.PHONY: debug
debug: $(TARGET_DEBUG)
    -include ($(DEP_DEBUG))

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

.PHONY: distclean
distclean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)

.PHONY: test
test: $(TARGET_TEST) run-test

# private
.PHONY: run-test
run-test:
	./$(TARGET_TEST) --verbose

install:$(TARGET)
	cp $(TARGET) $(PREFIX)/bin

uninstall:
	rm $(PREFIX)/bin/$(TARGET_NAME)

ctags:
	@echo generating ctags
	@$(CC) -M $(CFLAGS) $(INCLUDE) $(SRC) | sed -e 's/[\\ ]/\n/g' | \
		sed -e '/^$$/d' -e '/\.o:[ \t]*$$/d' | \
		ctags -L - --c-kinds=+p --fields=+iaSl --extras=+q --langmap=c:.c.h
