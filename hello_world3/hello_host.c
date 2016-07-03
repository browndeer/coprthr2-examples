
#include <coprthr.h>
#include <coprthr_cc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NTHR1 16
#define NTHR2 7

struct arg_struct { int a; };


int main( int argc, char* argv[] )
{
	// We will control number of threads from host code and execute the thread
	// function twice.
	int nthr1 = NTHR1;
	int nthr2 = NTHR2;


	// Open the coprocessor device
	int dd = coprthr_dopen(COPRTHR_DEVICE_E32,COPRTHR_O_STREAM);


	// Read the coprocessor binary and get the thread function
	coprthr_program_t prg = coprthr_cc_read_bin("./hello_device.e32",0);
	coprthr_kernel_t krn = coprthr_getsym(prg,"thread");


	// Allocate structs for Pthreads-style argument passing
	size_t argsz = sizeof(struct arg_struct);
   coprthr_mem_t argmem1 = coprthr_dmalloc(dd,argsz,0);
   coprthr_mem_t argmem2 = coprthr_dmalloc(dd,argsz,0);


	// Get the device memory pointer which may be dereferenced on the host
	struct arg_struct* parg1 = (struct arg_struct*)coprthr_memptr(argmem1,0);
	struct arg_struct* parg2 = (struct arg_struct*)coprthr_memptr(argmem2,0);
	parg1->a = 2112;
	parg2->a = 1001001;

	
	// Execute kernel on coprocessor device
	coprthr_dexec(dd,nthr1,krn, (void*)&argmem1, 0 );
	coprthr_dexec(dd,nthr2,krn, (void*)&argmem2, 0 );


	// Wait for coprocessor device operations to complete
	coprthr_dwait(dd);


	// clean up
   coprthr_dfree(dd,argmem1);
   coprthr_dfree(dd,argmem2);
	coprthr_dclose(dd);

}

