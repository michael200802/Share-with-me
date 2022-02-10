#include <sys/poll.h>
#include <stdio.h>

int main()
{
	struct pollfd fd;
	fd.fd = fileno(stdin);
	fd.events = POLLIN;

	printf("%d\n",poll(&fd,1,-1));
	return 0;
}
