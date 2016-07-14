
CCFLAGS += -O2 $(DEFS) -g
INCS = -I. -I/usr/local/browndeer/coprthr2/include 
LIBS = -L/usr/local/browndeer/coprthr2/lib -lcoprthr -lcoprthrcc -lm -ldl

COPRCC_FLAGS = --info 
COPRCC_DEFS = $(DEFS) 
COPRCC_INCS =
COPRCC_LIBS = 

TARGET = memory.x memory_device.e32

all: $(TARGET)

.PHONY: clean install uninstall $(SUBDIRS)

.SUFFIXES:
.SUFFIXES: .c .o .x

memory.x: memory_host.o
	$(CC) -rdynamic -o memory.x memory_host.o $(LIBS)

memory_device.e32: memory_device.c
	coprcc $(COPRCC_FLAGS) $(COPRCC_DEFS)  $(COPRCC_INCS) memory_device.c \
		$(COPRCC_LIBS) -o memory_device.e32

.c.o:
	$(CC) $(CCFLAGS) $(INCS) -c $<

clean: $(SUBDIRS)
	rm -f *.o *.e32
	rm -f $(TARGET)

distclean: clean 


