
#include <coprthr.h>
#include <coprthr_mpi.h>
#include <host_stdio.h>

#define MPI_BUF_SIZE 512

int main( int argc, char** argv ) {

	int procid, nprocs;

	MPI_Init(0,MPI_BUF_SIZE);

	MPI_Comm_rank(MPI_COMM_THREAD, &procid);
	MPI_Comm_size(MPI_COMM_THREAD, &nprocs);

	host_printf("Hello, World from MPI proc %d of %d\n",procid,nprocs);

	MPI_Finalize();

}

