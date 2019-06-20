TARGET  = alex4
PREFIX  = /usr/local
DATADIR = $(PREFIX)/share/$(TARGET)

CFLAGS ?= -g -Wall -O2
CFLAGS += `allegro-config --cflags`
LIBS    = -lm -laldmb -ldumb `allegro-config --libs`
DEFINES = -DDATADIR=\"$(DATADIR)/\"

OBJS    =  src/main.o \
           src/actor.o \
           src/bullet.o \
           src/control.o \
           src/edit.o \
           src/hisc.o \
           src/map.o \
           src/options.o \
           src/particle.o \
           src/player.o \
           src/script.o \
           src/scroller.o \
           src/shooter.o \
           src/timer.o \
           src/token.o \
           src/unix.o

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEFINES) -o $@ -c $<

install: $(TARGET)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(DATADIR)
	mkdir -p $(DESTDIR)$(PREFIX)/share/pixmaps
	mkdir -p $(DESTDIR)$(PREFIX)/share/applications
	install -p -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin
	install -p -m 644 data/*.dat $(DESTDIR)$(DATADIR)
	install -p -m 644 $(TARGET).png $(DESTDIR)$(PREFIX)/share/pixmaps
	install -p -m 644 $(TARGET).desktop $(DESTDIR)$(PREFIX)/share/applications

clean:
	rm -f $(OBJS) $(TARGET) *~
