
CCFLAGS += -O2 $(DEFS) -g
INCS = -I. -I/usr/local/browndeer/coprthr2/include 
LIBS = -L/usr/local/browndeer/coprthr2/lib -lcoprthr -lcoprthrcc -lm -ldl

COPRCC_FLAGS = 
COPRCC_DEFS = $(DEFS) 
COPRCC_INCS =
COPRCC_LIBS = 

TARGET = hello.x 

all: $(TARGET)

.PHONY: clean install uninstall 

.SUFFIXES:
.SUFFIXES: .c .o .x

hello.x: hello.c
	coprcc $(COPRCC_FLAGS) $(COPRCC_DEFS)  $(COPRCC_INCS) hello.c \
		$(COPRCC_LIBS) -fhost -o hello.x

#.c.o:
#	$(CC) $(CCFLAGS) $(INCS) -c $<

clean: $(SUBDIRS)
	rm -f *.o 
	rm -f $(TARGET)

distclean: clean 


