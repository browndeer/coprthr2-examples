
DEFS += -DEDIM=4 -DSIZE=128 -DSIZE2=1 -DSIZE3=1 -DMPI_BUF_SIZE=512 

CCFLAGS += -O2 $(DEFS) -g
INCS = -I. -I/usr/local/browndeer/coprthr2/include 
LIBS = -L/usr/local/browndeer/coprthr2/lib -lcoprthr -lcoprthrcc -lm -ldl

COPRCC_FLAGS = --info
COPRCC_DEFS = $(DEFS) -DCOPRTHR_MPI_COMPAT
COPRCC_INCS =
COPRCC_LIBS = -L/usr/local/browndeer/coprthr2/lib -lcoprthr_mpi

TARGET = cannon.x cannon_tfunc.e32

all: $(TARGET)

.PHONY: clean install uninstall $(SUBDIRS)

.SUFFIXES:
.SUFFIXES: .c .o .x

cannon.x: cannon_host.o
	$(CC) -rdynamic -o cannon.x cannon_host.o $(LIBS)

cannon_tfunc.e32: cannon_tfunc.c
	coprcc $(COPRCC_FLAGS) $(COPRCC_DEFS)  $(COPRCC_INCS) cannon_tfunc.c \
		$(COPRCC_LIBS) -o cannon_tfunc.e32

.c.o:
	$(CC) $(CCFLAGS) $(INCS) -c $<

clean: $(SUBDIRS)
	rm -f *.o *.e32
	rm -f $(TARGET)

distclean: clean 


