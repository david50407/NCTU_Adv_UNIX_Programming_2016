#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

extern void random_length_task();

int main(int argc, char **argv) {
	struct timeval tv;
	int times = 10;
	long long int last;
	if (argc > 1)
		sscanf(argv[1], "%d", &times);
	srand(time(0));

	// setpriority(PRIO_PROCESS, getpid(), -20);

	gettimeofday(&tv, NULL);
	last = tv.tv_usec;
	while (times--) {
		random_length_task();
		while ((1000000 + tv.tv_usec - last) % 1000000 < 100000) {
			// usleep(1);
			gettimeofday(&tv, NULL);
		}
		last = tv.tv_usec;
	}
	
	return 0;
}
