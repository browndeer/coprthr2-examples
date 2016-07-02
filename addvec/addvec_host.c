
#include <coprthr.h>
#include <coprthr_cc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NTHR 16
#define N 128


struct arg_struct { 
	int n;
	float* a; 
	float* b; 
	float* c; 
};


int main( int argc, char* argv[] )
{
	int i;
	int n = N;

	// We will control number of threads from host code
	int nthr = NTHR;


	// Open the coprocessor device
	int dd = coprthr_dopen(COPRTHR_DEVICE_E32,COPRTHR_O_STREAM);


	// Allocate memory for vectors and initial some data
	coprthr_mem_t a_mem = coprthr_dmalloc(dd,n*sizeof(float),0);
	coprthr_mem_t b_mem = coprthr_dmalloc(dd,n*sizeof(float),0);
	coprthr_mem_t c_mem = coprthr_dmalloc(dd,n*sizeof(float),0);
	float* a = (float*)coprthr_memptr(a_mem,0);
	float* b = (float*)coprthr_memptr(b_mem,0);
	float* c = (float*)coprthr_memptr(c_mem,0);
	for(i=0; i<n; i++) {
		a[i] = 1.0f * i;
		b[i] = 2.0f * i;
		c[i] = 0;
	}


	// Read the coprocessor binary and get the thread function
	coprthr_program_t prg = coprthr_cc_read_bin("./addvec_device.e32",0);
	coprthr_kernel_t krn = coprthr_getsym(prg,"thread");


	// Allocate struct for Pthreads-style argument passing
	size_t argsz = sizeof(struct arg_struct);
   coprthr_mem_t argmem = coprthr_dmalloc(dd,argsz,0);


	// Get the device memory pointer which may be dereferenced on the host
	struct arg_struct* parg = (struct arg_struct*)coprthr_memptr(argmem,0);
	parg->n = n;
	parg->a = a;
	parg->b = b;
	parg->c = c;

	
	// Execute kernel on coprocessor device
	coprthr_dexec(dd,nthr,krn, (void*)&argmem, 0 );


	// Wait for coprocessor device operations to complete
	coprthr_dwait(dd);

	for(i=0; i<n; i++) 
		printf("%f %f %f\n",a[i],b[i],c[i]);

	// clean up
   coprthr_dfree(dd,argmem);
	coprthr_dclose(dd);

}

