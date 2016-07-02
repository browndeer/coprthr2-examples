
CCFLAGS += -O2 $(DEFS) -g
INCS = -I. -I/usr/local/browndeer/coprthr2/include 
#LIBS = -L/usr/local/browndeer/coprthr2/lib -lcoprthr -lcoprthrcc -lm -ldl
#LIBS = -L/usr/local/browndeer/coprthr2/lib -lcoprthr -lcoprthrcc -lm -ldl
LIBS = -L/usr/local/browndeer/coprthr2/lib -lcoprthr -lcoprthrcc 

COPRCC_FLAGS = 
COPRCC_DEFS = $(DEFS) 
COPRCC_INCS =
COPRCC_LIBS = -L/usr/local/browndeer/coprthr2/lib 

TARGET = addvec.x addvec_device.e32

all: $(TARGET)

.PHONY: clean install uninstall 

.SUFFIXES:
.SUFFIXES: .c .o .x

addvec.x: addvec_host.c
	$(CC) $(CCFLAGS) -o addvec.x $(DEFS) $(INCS) addvec_host.c $(LIBS)

addvec_device.e32: addvec_device.c
	coprcc $(COPRCC_FLAGS) $(COPRCC_DEFS)  $(COPRCC_INCS) addvec_device.c \
		$(COPRCC_LIBS) -o addvec_device.e32

clean: $(SUBDIRS)
	rm -f *.o 
	rm -f $(TARGET)

distclean: clean 


