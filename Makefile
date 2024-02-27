# tool macros
CC := gcc
CFLAGS := -Wall -g
PREFIX := ~/.local

# path macros
BIN_PATH := bin
OBJ_PATH := $(BIN_PATH)/obj
SRC_PATH := src
MAIN_SRC := src/main.c
TEST_SRC := tests.c

# compile macros
TARGET_NAME := http-server
ifeq ($(OS),Windows_NT)
	TARGET_NAME := $(addsuffix .exe,$(TARGET_NAME))
endif
TARGET := $(BIN_PATH)/$(TARGET_NAME)

# src files & obj files
SRC := $(filter-out $(MAIN_SRC), $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c*))))
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

DEPENDS := $(OBJ:.o=.d)

# clean files list
DISTCLEAN_LIST := $(OBJ) \
				  $(DEPENDS) \
				  $(TEST_OBJ) \
				  $(DEP_TEST)

CLEAN_LIST := $(TARGET) \
			  $(DISTCLEAN_LIST) \

# default rule
default: makedir all

-include ($(DEPENDS))

# non-phony targets
$(TARGET): makedir $(OBJ)
	$(CC) $(CFLAGS) $(MAIN_SRC) -o $@ $(OBJ)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) $(CFLAGS) -c -MMD -MP -o $@ $<

# phony rules
.PHONY: test
test: makedir $(OBJ)
	$(CC) $(CFLAGS) $(TEST_SRC) -o bin/test $(OBJ)

run:$(TARGET)
	./bin/http-server

run_test: test
	./bin/test

.PHONY: makedir
makedir:
	@mkdir -p $(BIN_PATH) $(OBJ_PATH) $(DBG_PATH)

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

install:$(TARGET)
	cp $(TARGET) $(PREFIX)/bin

uninstall:
	rm $(PREFIX)/bin/$(TARGET_NAME)

ctags:
	@echo generating ctags
	@$(CC) -M $(CFLAGS) $(INCLUDE) $(SRC) | sed -e 's/[\\ ]/\n/g' | \
		sed -e '/^$$/d' -e '/\.o:[ \t]*$$/d' | \
		ctags -L - --c-kinds=+p --fields=+iaSl --extras=+q --langmap=c:.c.h
