#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	kill(getpid(), atoi(argv[1]));
}

