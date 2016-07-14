/* memort_host.c
 *
 * Copyright (c) 2016 Brown Deer Technology, LLC.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* DAR */

#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
//#include <sys/time.h>
//#include <math.h>
//#include <dlfcn.h>

#include "coprthr.h"
#include "coprthr_cc.h"
#include "coprthr_thread.h"

#define SIZE 4096

struct my_args { int n; int* aa; int* bb; int* cc; };

int main(int argc, char* argv[])
{

	int i;
	int n = SIZE;


	/* open device for threads */
	int dd = coprthr_dopen(COPRTHR_DEVICE_E32,COPRTHR_O_THREAD);


	/* compile thread function */
	coprthr_program_t prg = coprthr_cc_read_bin("./memory_device.e32",0);
	coprthr_sym_t thr = coprthr_getsym(prg,"my_thread");

	printf("dd=%d prg=%p krn=%p\n",dd,prg,thr);

	/* allocate memory shared with coprocessor device */
	coprthr_mem_t aa_mem = coprthr_dmalloc(dd,n*sizeof(int),0);
	coprthr_mem_t bb_mem = coprthr_dmalloc(dd,n*sizeof(int),0);
	coprthr_mem_t cc_mem = coprthr_dmalloc(dd,n*sizeof(int),0);

   int* aa = (int*)coprthr_memptr(aa_mem,0);
   int* bb = (int*)coprthr_memptr(bb_mem,0);
   int* cc = (int*)coprthr_memptr(cc_mem,0);

	
	/* set args to pass to thread on coprocessor device */
	coprthr_mem_t args_mem = coprthr_dmalloc(dd,sizeof(struct my_args),0);
	struct my_args* pargs = (struct my_args*)coprthr_memptr(args_mem,0);
	pargs->n = n;
	pargs->aa = aa,
	pargs->bb = bb,
	pargs->cc = cc;


	/* initialize A, B, and C arrays */
	for (i=0; i<n; i++) {
		aa[i] = i;
		bb[i] = 2*i;
		cc[i] = 3;
	}

	// Execute kernel on coprocessor device
	coprthr_dexec(dd,16,thr,(void*)&args_mem, 0 );
	coprthr_dwait(dd);


	for(i=0; i<n; i++) 
		printf("%d: %d + %d = %d\n",i,aa[i],bb[i],cc[i]);


	/* clean up */
	coprthr_dfree(dd,args_mem);
	coprthr_dfree(dd,aa_mem);
	coprthr_dfree(dd,bb_mem);
	coprthr_dfree(dd,cc_mem);

	coprthr_dclose(dd);
}
