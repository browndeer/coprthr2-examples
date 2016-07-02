
CCFLAGS += -O2 $(DEFS) -g
INCS = -I. -I/usr/local/browndeer/coprthr2/include 
LIBS = -L/usr/local/browndeer/coprthr2/lib -lcoprthr -lm -ldl

COPRCC_FLAGS = -fhost
COPRCC_DEFS = $(DEFS) -DCOPRTHR_MPI_COMPAT
COPRCC_INCS = -I/usr/local/browndeer/coprthr2/include

TARGET = main.x

all: $(TARGET)

.PHONY: clean install uninstall 

.SUFFIXES:
.SUFFIXES: .c .o .x

main.x: main.c
	coprcc $(COPRCC_FLAGS) $(COPRCC_DEFS)  $(COPRCC_INCS) main.c \
		$(COPRCC_LIBS) -o main.x

.c.o:
	$(CC) $(CCFLAGS) $(INCS) -c $<

clean: $(SUBDIRS)
	rm -f *.o *.e32 
	rm -f $(TARGET)
	rm -f *.dat

distclean: clean 


