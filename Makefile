CC = gcc
CFLAGS = -Wall -Wextra -O2
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
INCLUDE_DIR = includes
TARGET = $(BUILD_DIR)/volume_indicator

PKG_CONFIG = pkg-config
XCB_CFLAGS = $(shell $(PKG_CONFIG) --cflags xcb)
XCB_LIBS = $(shell $(PKG_CONFIG) --libs xcb)
FREETYPE_CFLAGS = $(shell $(PKG_CONFIG) --cflags freetype2)
FREETYPE_LIBS = $(shell $(PKG_CONFIG) --libs freetype2)

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(wildcard $(INCLUDE_DIR)/*.h)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(XCB_CFLAGS) $(FREETYPE_CFLAGS) -I$(INCLUDE_DIR) -lX11 -lxcb-keysyms -c $< -o $@

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(XCB_LIBS) $(FREETYPE_LIBS) -lX11 -lxcb-keysyms
all: $(TARGET)

run: all
	@./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
