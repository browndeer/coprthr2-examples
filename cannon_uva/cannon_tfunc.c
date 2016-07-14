/* cannon_tfunc.c
 *
 * Copyright (c) 2016 Brown Deer Technology, LLC.  All rights reserved.
 * Copyright (c) 2015 James A. Ross.  All rights reserved.
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
/* JAR */

#include <coprthr.h>
#include <coprthr_mpi.h>


// This function performs a serial matrix-matrix multiplication c = a * b
void MatrixMultiply(int n, float *a, float *b, float *c);

void __usrmem_call MatrixMatrixMultiply(
	int n, int dim, float *a, float *b, float *c, void* p_comm_2d
)
{
	int i;
	int npes;
	int myrank, my2drank, mycoords[2];
	int uprank, downrank, leftrank, rightrank, coords[2];
	int shiftsource, shiftdest;
	MPI_Status status;

	MPI_Comm comm_2d = (MPI_Comm)p_comm_2d;


	// Get the communicator related information
	MPI_Comm_size(comm_2d, &npes);
	MPI_Comm_rank(comm_2d, &myrank);


	// Get the rank and coordinates with respect to the 2D topology
	MPI_Comm_rank(comm_2d, &my2drank);
	MPI_Cart_coords(comm_2d, my2drank, 2, mycoords);


	// Compute ranks of the up and left shifts
	MPI_Cart_shift(comm_2d, 0, -1, &rightrank, &leftrank);
	MPI_Cart_shift(comm_2d, 1, -1, &downrank, &uprank);


	// Perform the initial matrix alignment. First for A and then for B

	MPI_Cart_shift(comm_2d, 0, -mycoords[1], &shiftsource, &shiftdest);

	MPI_Sendrecv_replace(a, n*n, MPI_FLOAT, shiftdest, 1, shiftsource, 1,
		comm_2d, &status);

	MPI_Cart_shift(comm_2d, 1, -mycoords[0], &shiftsource, &shiftdest);

	MPI_Sendrecv_replace(b, n*n, MPI_FLOAT, shiftdest, 1, shiftsource, 1,
		comm_2d, &status);

	
	// Get into the main computation loop

	for (i=1; i<dim; i++) {

		MatrixMultiply(n, a, b, c);

		// Shift matrix a left by one and shift matrix b up by one

		MPI_Sendrecv_replace(a, n*n, MPI_FLOAT, leftrank, 1, rightrank, 1,
			comm_2d, &status);

		MPI_Sendrecv_replace(b, n*n, MPI_FLOAT, uprank, 1, downrank, 1,
			comm_2d, &status);

	}

	MatrixMultiply(n, a, b, c);
}


typedef struct { 
	int N; int s; int d; float* ga; float* gb; float* gc; 
} my_args_t;


void __entry my_thread( void* p) {

	int tid = coprthr_get_thread_id();

	my_args_t* pargs = (my_args_t*)p;

	int N = pargs->N, s = pargs->s, d = pargs->d; 
	float *ga = pargs->ga, *gb = pargs->gb, *gc = pargs->gc;
	int n = N/d;

	int myrank_2d, mycoords[2];
	int dims[2] = {d, d};
	int periods[2] = {1, 1};

	MPI_Init(0,MPI_BUF_SIZE);

	MPI_Comm comm = MPI_COMM_THREAD;
	MPI_Comm comm_2d;
	MPI_Cart_create(comm, 2, dims, periods, 1, &comm_2d);
	MPI_Comm_rank(comm_2d, &myrank_2d);
	MPI_Cart_coords(comm_2d, myrank_2d, 2, mycoords);

	int x = mycoords[1];
	int y = mycoords[0];

	void* memfree = coprthr_tls_sbrk(0);
	float* a = (float*)coprthr_tls_sbrk(n*n*sizeof(float));
	float* b = (float*)coprthr_tls_sbrk(n*n*sizeof(float));
	float* c = (float*)coprthr_tls_sbrk(n*n*sizeof(float));


	int i,j,k;
	for (i=0; i<s; i++) {

		for (j=0; j<s; j++) {

			float* rgc = gc + ((i*N + x*n)*s + j)*N + y*n;

			// read C
			coprthr_memcopy2d_align(c,rgc,n*sizeof(float),s*N*sizeof(float),
				n*sizeof(float),n,COPRTHR2_M_DMA_0);
	
			for (k=0; k<s; k++) {

				float* rga = ga + ((i*N + x*n)*s + k)*N + y*n;
				float* rgb = gb + ((k*N + x*n)*s + j)*N + y*n;


				// read A and B

				coprthr_event_t ev0,ev1;

				ev1 = coprthr_memcopy2d_align(b,rgb, n*sizeof(float),
					s*N*sizeof(float), n*sizeof(float), n,
					COPRTHR2_M_DMA_1|COPRTHR2_E_NOWAIT);

				ev0 = coprthr_memcopy2d_align(a,rga, n*sizeof(float),
					s*N*sizeof(float), n*sizeof(float), n,
					COPRTHR2_M_DMA_0|COPRTHR2_E_NOWAIT);

				coprthr_wait(ev1);
				coprthr_wait(ev0);


				MatrixMatrixMultiply(n,d,a,b,c,comm_2d);

			}

			// write C
			coprthr_memcopy2d_align(rgc,c,s*N*sizeof(float),n*sizeof(float),
				n*sizeof(float),n,COPRTHR2_M_DMA_1);

		}
	}

	coprthr_tls_brk(memfree);

	MPI_Finalize();

}


void MatrixMultiply(int n, float *a, float *b, float *c)
//void __usrmem_call MatrixMultiply(int n, float *a, float *b, float *c)
{
	int i, j, k;
	for (i=0; i<n; i+=4) {
		for (j=0; j<n; j+=4) {
			float c00 = c[(i+0)*n+j+0];
			float c10 = c[(i+0)*n+j+1];
			float c20 = c[(i+0)*n+j+2];
			float c30 = c[(i+0)*n+j+3];
			float c01 = c[(i+1)*n+j+0];
			float c11 = c[(i+1)*n+j+1];
			float c21 = c[(i+1)*n+j+2];
			float c31 = c[(i+1)*n+j+3];
			float c02 = c[(i+2)*n+j+0];
			float c12 = c[(i+2)*n+j+1];
			float c22 = c[(i+2)*n+j+2];
			float c32 = c[(i+2)*n+j+3];
			float c03 = c[(i+3)*n+j+0];
			float c13 = c[(i+3)*n+j+1];
			float c23 = c[(i+3)*n+j+2];
			float c33 = c[(i+3)*n+j+3];
			for (k=0; k<n; k+=4) {
				float* a0 = a+(i+0)*n+k;
				float* a1 = a+(i+1)*n+k;
				float* a2 = a+(i+2)*n+k;
				float* a3 = a+(i+3)*n+k;
				float* b0 = b+(k+0)*n+j;
				float* b1 = b+(k+1)*n+j;
				float* b2 = b+(k+2)*n+j;
				float* b3 = b+(k+3)*n+j;
				c00 += a0[0]*b0[0] + a0[1]*b1[0] + a0[2]*b2[0] + a0[3]*b3[0];
				c10 += a0[0]*b0[1] + a0[1]*b1[1] + a0[2]*b2[1] + a0[3]*b3[1];
				c20 += a0[0]*b0[2] + a0[1]*b1[2] + a0[2]*b2[2] + a0[3]*b3[2];
				c30 += a0[0]*b0[3] + a0[1]*b1[3] + a0[2]*b2[3] + a0[3]*b3[3];
				c01 += a1[0]*b0[0] + a1[1]*b1[0] + a1[2]*b2[0] + a1[3]*b3[0];
				c11 += a1[0]*b0[1] + a1[1]*b1[1] + a1[2]*b2[1] + a1[3]*b3[1];
				c21 += a1[0]*b0[2] + a1[1]*b1[2] + a1[2]*b2[2] + a1[3]*b3[2];
				c31 += a1[0]*b0[3] + a1[1]*b1[3] + a1[2]*b2[3] + a1[3]*b3[3];
				c02 += a2[0]*b0[0] + a2[1]*b1[0] + a2[2]*b2[0] + a2[3]*b3[0];
				c12 += a2[0]*b0[1] + a2[1]*b1[1] + a2[2]*b2[1] + a2[3]*b3[1];
				c22 += a2[0]*b0[2] + a2[1]*b1[2] + a2[2]*b2[2] + a2[3]*b3[2];
				c32 += a2[0]*b0[3] + a2[1]*b1[3] + a2[2]*b2[3] + a2[3]*b3[3];
				c03 += a3[0]*b0[0] + a3[1]*b1[0] + a3[2]*b2[0] + a3[3]*b3[0];
				c13 += a3[0]*b0[1] + a3[1]*b1[1] + a3[2]*b2[1] + a3[3]*b3[1];
				c23 += a3[0]*b0[2] + a3[1]*b1[2] + a3[2]*b2[2] + a3[3]*b3[2];
				c33 += a3[0]*b0[3] + a3[1]*b1[3] + a3[2]*b2[3] + a3[3]*b3[3];
			}
			c[(i+0)*n+j+0] = c00;
			c[(i+0)*n+j+1] = c10;
			c[(i+0)*n+j+2] = c20;
			c[(i+0)*n+j+3] = c30;
			c[(i+1)*n+j+0] = c01;
			c[(i+1)*n+j+1] = c11;
			c[(i+1)*n+j+2] = c21;
			c[(i+1)*n+j+3] = c31;
			c[(i+2)*n+j+0] = c02;
			c[(i+2)*n+j+1] = c12;
			c[(i+2)*n+j+2] = c22;
			c[(i+2)*n+j+3] = c32;
			c[(i+3)*n+j+0] = c03;
			c[(i+3)*n+j+1] = c13;
			c[(i+3)*n+j+2] = c23;
			c[(i+3)*n+j+3] = c33;
		}
	}
}
