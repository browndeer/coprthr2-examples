
CCFLAGS += -O2 $(DEFS) -g
INCS = -I. -I/usr/local/browndeer/coprthr2/include 
LIBS = -L/usr/local/browndeer/coprthr2/lib -lcoprthr -lcoprthrcc -lm -ldl

COPRCC_FLAGS = -fhost
COPRCC_DEFS = $(DEFS) -DCOPRTHR_MPI_COMPAT
COPRCC_INCS =
COPRCC_LIBS = -L/usr/local/browndeer/coprthr2/lib -lcoprthr_mpi 

TARGET = hello_mpi.x

all: $(TARGET)

.PHONY: clean install uninstall $(SUBDIRS)

.SUFFIXES:
.SUFFIXES: .c .o .x

hello_mpi.x: hello_mpi.c
	coprcc $(COPRCC_FLAGS) $(COPRCC_DEFS)  $(COPRCC_INCS) hello_mpi.c \
		$(COPRCC_LIBS) -o hello_mpi.x

#.c.o:
#	$(CC) $(CCFLAGS) $(INCS) -c $<

clean: $(SUBDIRS)
	rm -f *.o *.e32
	rm -f $(TARGET)

distclean: clean 


