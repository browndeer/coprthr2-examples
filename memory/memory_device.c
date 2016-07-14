/* memory_device.c
 *
 * Copyright (c) 2016 Brown Deer Technology, LLC.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* DAR */

#include <coprthr.h>
#include <host_stdio.h>

typedef struct { 
	int n; int* aa; int* bb; int* cc; 
} my_args_t;

void __entry my_thread( void* p )
{
	int i;

	int tid = coprthr_get_thread_id();

	my_args_t* pargs = (my_args_t*)p;

	int n = pargs->n;
	int* aa = pargs->aa;
	int* bb = pargs->bb;
	int* cc = pargs->cc;

	int m = n/16;
	int offset = m*tid;
	int sz = m*sizeof(int);


	// allocate local buffers of size n/16
	void* memfree = coprthr_tls_sbrk(0);
	int* buf_a = (int*)coprthr_tls_sbrk(sz);
	int* buf_b = (int*)coprthr_tls_sbrk(sz);
	int* buf_c = (int*)coprthr_tls_sbrk(sz);

	// read C
	coprthr_memcopy_align(buf_c,cc+offset,sz,COPRTHR2_M_DMA_0);


	// read A and B using two DMA channels and non-blocking memcopy calls
	coprthr_memcopy_align(buf_a,aa+offset,sz,COPRTHR2_M_DMA_1|COPRTHR2_E_NOWAIT);
	coprthr_memcopy_align(buf_b,bb+offset,sz,COPRTHR2_M_DMA_0|COPRTHR2_E_NOWAIT);
	coprthr_wait(COPRTHR2_E_DMA_1);
	coprthr_wait(COPRTHR2_E_DMA_0);


	// c = a + b
	for(i=0; i<m; i++)
		buf_c[i] = buf_a[i] + buf_b[i];


	// write C
	coprthr_memcopy_align(cc+offset,buf_c,sz,COPRTHR2_M_DMA_0);


	// clean up
	coprthr_tls_brk(memfree);

}

