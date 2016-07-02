
#include <coprthr.h>

struct arg_struct {
	int n;
	float* a;
	float* b;
	float* c;
};

void __entry thread( void* p )
{
	int i;

	int tid = coprthr_get_thread_id();
	int nthr = coprthr_get_num_threads();

	struct arg_struct* parg = (struct arg_struct*)p;

	int n = parg->n;
	float* a = parg->a;
	float* b = parg->b;
	float* c = parg->c;

	int nb = n/nthr;
	int offset = nb*tid;

	for(i=offset; i<offset+nb; i++) c[i] = a[i] + b[i];
	
}

