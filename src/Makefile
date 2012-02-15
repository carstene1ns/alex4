PREFIX  = /usr/local
DATADIR = $(PREFIX)/share/$(TARGET)
CFLAGS  ?= -g -Wall -Wno-deprecated-declarations -O2
LIBS = -laldmb -ldumb `allegro-config --libs`
DEFINES = -DDATADIR=\"$(DATADIR)/\"
OBJS    =  actor.o    edit.o  map.o       player.o    shooter.o unix.o \
           bullet.o   hisc.o  options.o   script.o    timer.o          \
           control.o  main.o  particle.o  scroller.o  token.o
TARGET  = alex4

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEFINES) -o $@ -c $<

install: $(TARGET)
	mkdir -p $(PREFIX)/bin
	mkdir -p $(DATADIR)
	install -p -m 755 $(TARGET) $(PREFIX)/bin
	install -p -m 644 ../data/*.dat $(DATADIR)

clean:
	rm -f $(OBJS) $(TARGET) *~
