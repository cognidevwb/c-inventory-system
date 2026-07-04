CC = cc
CFLAGS = -O2 -Wall

OBJS = src/main.o src/inventory.o src/storage.o src/report.o

invmgr: $(OBJS)
	$(CC) -o invmgr $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f invmgr src/*.o

.PHONY: clean
