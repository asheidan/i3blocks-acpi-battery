CC = gcc
CFLAGS = -Wall -Wextra
CFLAGS += -Os

TARGET = battery

INSTALLDIR = $(HOME)/.config/i3blocks/blocks

$(TARGET): main.o
	$(LINK.o) -o $@ $^

install: $(INSTALLDIR)/$(TARGET)
.PHONY: install

$(INSTALLDIR)/$(TARGET): $(TARGET)
	install -b -s -D $(TARGET) $(INSTALLDIR)/$(TARGET)
