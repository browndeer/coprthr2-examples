
#include <coprthr.h>
#include <esyscall.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <host_fcntl.h>
#include <host_stdio.h>

void main( int argc, char** argv )
{

	int tid = coprthr_get_thread_id();

	char local_text[] = "And echoes with the sound of salesmen...";
	char* text = __ega(local_text); // convert local ptr to global ptr

	char local_filename[32] = "output_file_";
	char* filename =  __ega(local_filename); // convert local ptr to global ptr

	host_sprintf(filename, "output_%d.dat", tid );

	host_printf("%s\n",filename);

	int fd = open( filename, O_CREAT|O_WRONLY, S_IWUSR|S_IRUSR);

	host_printf("fd = %d\n",fd);

	write(fd,text,strlen(local_text)-tid);

	close(fd);

}

