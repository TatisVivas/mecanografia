.PHONY: clean

CC = gcc
LINK = gcc

CFLAGS = -std=c17 -pedantic -Wall -Wextra -O2

TEST_CFLAGS = -std=c17 -pedantic -Wall -Wextra -g
TEST_LDFLAGS = -lm

ifeq ($(OS), Windows_NT)
  RM = cmd /c del
  TARGET = mecc.exe
  LDFLAGS = lib/pdcurses.a -lm
  TEST_LDFLAGS += $(LDFLAGS)
else
  RM = rm -f
  TARGET = mecc
  LDFLAGS = -lcurses -lm
  TEST_LDFLAGS += $(LDFLAGS) -lrt
endif


TESTDIR = ./test

OBJECTS = main.o mecc.o lecc.o

all: $(TARGET) $(OBJECTS)

test: test_lecc 

$(TARGET): $(OBJECTS)
	$(LINK) $^ $(LDFLAGS) -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $^ -o $@

mecc.o: mecc.c
	$(CC) $(CFLAGS) -c $^ -o $@

lecc.o: lecc.c
	$(CC) $(CFLAGS) -c $^ -o $@

test_lecc: $(TESTDIR)/test_lecc.c
	$(CC) $(TEST_CFLAGS) $^ $(TEST_LDFLAGS) -o $@

clean :
	-$(RM) *.o
	-$(RM) test_lecc
	-$(RM) $(TARGET)
