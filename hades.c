#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <linux/joystick.h>

void usage(char *argv0)
{
    fprintf(stderr, "usage: %s <joystick device path> <x axis> <z axis>\n\n", argv0);
    fprintf(stderr, "See /proc/bus/input/devices for the path.\n");
    exit(1);
}

void keyup(int xte_fd, char *key, int &state)
{
    if (state)
    {
	write(xte_fd, "keyup ", 6);
	write(xte_fd, key, strlen(key));
	write(xte_fd, "\n", 1);
	state = 0;
    }
}

void keydown(int xte_fd, char *key, int &state)
{
    if (!state)
    {
	write(xte_fd, "keydown ", 8);
	write(xte_fd, key, strlen(key));
	write(xte_fd, "\n", 1);
	state = 1;
    }
}

int main(int argc, char **argv)
{
    if (argc < 4)
	usage(argv[0]);

    int js_fd = open(argv[1], O_RDONLY);
    if (js_fd == -1)
    {
	perror("Couldn't open joystick");
	usage(argv[0]);
    }

    int xaxis = strtol(argv[2],0,10);
    if (xaxis == 0 && errno == EINVAL)
	usage(argv[0]);

    int zaxis = strtol(argv[3],0,10);
    if (zaxis == 0 && errno == EINVAL)
	usage(argv[0]);

    FILE *xte = popen("xte","w");
    if (xte == NULL)
    {
	fprintf(stderr, "Couldn't start xte. Did you install xautomation?\n");
	exit(1);
    }
    setlinebuf(xte);
    int xte_fd = fileno(xte);

    int n,e,s,w;
    n = e = s = w = 0; // 0 is up, 1 is down
    while (1)
    {
	struct js_event ev;
	read(fd, &ev, sizeof(struct js_event));

	if (ev.type == JS_EVENT_AXIS)
	{
	    if (ev.number == xaxis)
	    {
		if (ev.value < 0)
		    keydown(xte_fd, "q", w);
		else if (e.value > 0)
		    keydown(xte_fd, "e", e);
		else
		{
		    keyup(xte_fd, "q", w);
		    keyup(xte_fd, "e", e);
		}
	    }
	    else if (ev.number == zaxis)
	    {
		if (ev.value < 0)
		    keydown(xte_fd, "Up", n);
		else if (e.value > 0)
		    keydown(xte_fd, "Down", s);
		else
		{
		    keyup(xte_fd, "Up", n);
		    keyup(xte_fd, "Down", s);
		}
	    }
	}
    }
}
