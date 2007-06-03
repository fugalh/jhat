// jhat - translate joystick directional hat movements to X-Plane keystrokes
// Copyright (C) 2007  Hans Fugal <hans@fugal.net>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/joystick.h>

void usage(char *argv0)
{
    fprintf(stderr, "usage: %s <joystick device path> <x axis> <z axis>\n\n", argv0);
    fprintf(stderr, "See /proc/bus/input/devices for the path.\n");
    fprintf(stderr, "Use jstest to figure out the axes.\n");
    exit(1);
}

void keyup(int xte_fd, char *key, int *state)
{
    if (*state)
    {
	write(xte_fd, "keyup ", 6);
	write(xte_fd, key, strlen(key));
	write(xte_fd, "\n", 1);
	*state = 0;
    }
}

void keydown(int xte_fd, char *key, int *state)
{
    if (!*state)
    {
	write(xte_fd, "keydown ", 8);
	write(xte_fd, key, strlen(key));
	write(xte_fd, "\n", 1);
	*state = 1;
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
	read(js_fd, &ev, sizeof(struct js_event));

	if (ev.type == JS_EVENT_AXIS)
	{
	    if (ev.number == xaxis)
	    {
		if (ev.value < 0)
		    keydown(xte_fd, "Left", &w);
		else if (ev.value > 0)
		    keydown(xte_fd, "Right", &e);
		else
		{
		    keyup(xte_fd, "Left", &w);
		    keyup(xte_fd, "Right", &e);
		}
	    }
	    else if (ev.number == zaxis)
	    {
		if (ev.value < 0)
		    keydown(xte_fd, "Up", &n);
		else if (ev.value > 0)
		    keydown(xte_fd, "Down", &s);
		else
		{
		    keyup(xte_fd, "Up", &n);
		    keyup(xte_fd, "Down", &s);
		}
	    }
	}
    }
}
