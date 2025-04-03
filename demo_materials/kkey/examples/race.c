#include <err.h>
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_KEYS 128

static void pin_to_cpu(int n)
{

	if (n >= sysconf(_SC_NPROCESSORS_ONLN))
		errx(1, "no such cpu %d", n);

	cpu_set_t set;

	CPU_ZERO(&set);
	CPU_SET(n, &set);

	if (0 > sched_setaffinity(0, sizeof(set), &set))
		err(1, "sched_setaffinity");
}

int main(void)
{
	int fds[NUM_KEYS];
	char buf[] = "/dev/kkeyXXX";
	for(size_t i = 0; i < NUM_KEYS; ++i)
	{
		if(3 != snprintf(buf+9, 4, "%03zu", i))
			errx(1, "not enough space for path");
		if(0 > (fds[i] = open(buf, O_RDWR)))
			err(1, "unable to open fd %zu", i);
	}
	pid_t pid;
	if(0 > (pid = fork()))
		err(1, "unable to create child");
	pin_to_cpu(!pid);
	for(size_t i = !pid; i < NUM_KEYS; i += 2)
	{
		if(1 != write(fds[i], "1", 1))
			err(1, "failed to write");
		usleep(100);
		if(1 != write(fds[i], "0", 1))
			err(1, "failed to write");
	}
	if (pid)
		waitpid(pid, NULL, 0);
	return 0;
}

