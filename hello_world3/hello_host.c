
#include <coprthr.h>
#include <coprthr_cc.h>
#include <coprthr_thread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NTHR 16

struct arg_struct { int a; };


int main( int argc, char* argv[] )
{
	// We will control number of threads from host code
	int nthr = NTHR;


	// Open the coprocessor device
	int dd = coprthr_dopen(COPRTHR_DEVICE_E32,COPRTHR_O_STREAM);


	// Read the coprocessor binary and get the thread function
	coprthr_program_t prg = coprthr_cc_read_bin("./hello_device.e32",0);
	coprthr_sym_t thr = coprthr_getsym(prg,"thread");


	// Allocate struct for Pthreads-style argument passing
	size_t argsz = sizeof(struct arg_struct);
   coprthr_mem_t argmem = coprthr_dmalloc(dd,argsz,0);


	// Get the device memory pointer which may be dereferenced on the host
	struct arg_struct* parg = (struct arg_struct*)coprthr_memptr(argmem,0);
	parg->a = 2112;


	// Create threads on coprocessor device
   coprthr_attr_t attr;
   coprthr_td_t td;
   void* status;
   coprthr_attr_init( &attr );
   coprthr_attr_setdetachstate(&attr,COPRTHR_CREATE_JOINABLE);
   coprthr_attr_setdevice(&attr,dd);
   coprthr_ncreate( nthr, &td, &attr, thr, (void*)&argmem );


	// Wait until threads are complete
   coprthr_join(td,&status);


	// clean up
   coprthr_attr_destroy( &attr);
   coprthr_dfree(dd,argmem);
	coprthr_dclose(dd);

}

