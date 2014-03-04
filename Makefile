CC = gcc
LFAGS = 
CFLAGS = -Wall -O3
OBJECTS = fastsweaper.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

fastsweaper: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
debug:	CFLAGS= -Wall -O0 -g
debug:	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o fastsweaper-$@ $(LDFLAGS)
clean:
	@echo cleaning oop
	@rm -f $(OBJECTS)
	@rm -f fastsweaper
	@rm -f fastsweaper-debug














