
#include <coprthr.h>
#include <host_stdio.h>

int main( int argc, char** argv )
{
	int tid = coprthr_get_thread_id();
	host_printf("hello, world from thread #%d\n",tid);
}

