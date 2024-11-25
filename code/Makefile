# Compiler
CC = clang

# Compiler Flags
CFLAGS = -lncurses -lmenu -lcdk

# Source Files
SRCS = main.c \
       seller_portal/roughtui.c \
			 seller_portal/product_new.c \
       seller_portal/wow.c \
       customer_portal/cart.c \
       customer_portal/display.c \
       customer_portal/main.c \
       customer_portal/product.c \

# Output Executable
TARGET = main

# Default Target
all: $(TARGET)

# Always rebuild the target
$(TARGET):
	$(CC) $(SRCS) -o $(TARGET) $(CFLAGS)

# Clean Target
clean:
	rm -f $(TARGET)

# Phony Targets
.PHONY: all clean $(TARGET)
