
CCFLAGS += -O2 $(DEFS) -g
INCS = -I. -I/usr/local/browndeer/coprthr2/include 
#LIBS = -L/usr/local/browndeer/coprthr2/lib -lcoprthr -lcoprthrcc -lm -ldl
#LIBS = -L/usr/local/browndeer/coprthr2/lib -lcoprthr -lcoprthrcc -lm -ldl
LIBS = -L/usr/local/browndeer/coprthr2/lib -lcoprthr -lcoprthrcc 

COPRCC_FLAGS = 
COPRCC_DEFS = $(DEFS) 
COPRCC_INCS =
COPRCC_LIBS = -L/usr/local/browndeer/coprthr2/lib 

TARGET = hello_host.x hello_device.e32

all: $(TARGET)

.PHONY: clean install uninstall 

.SUFFIXES:
.SUFFIXES: .c .o .x

hello_host.x: hello_host.c
	$(CC) $(CCFLAGS) -o hello_host.x $(DEFS) $(INCS) hello_host.c $(LIBS)

hello_device.e32: hello_device.c
	coprcc $(COPRCC_FLAGS) $(COPRCC_DEFS)  $(COPRCC_INCS) hello_device.c \
		$(COPRCC_LIBS) -o hello_device.e32

clean: $(SUBDIRS)
	rm -f *.o 
	rm -f $(TARGET)

distclean: clean 


