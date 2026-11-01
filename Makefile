CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic
TARGET ?= loan-eligibility
SOURCE = main.c

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe
