
#include <coprthr.h>
#include <host_stdio.h>

struct arg_struct { int a; };

void __entry thread( void* p )
{
	int tid = coprthr_get_thread_id();

	struct arg_struct* parg = (struct arg_struct*)p;

	host_printf("hello, world from thread #%d, I received %d\n",tid,parg->a);
}

