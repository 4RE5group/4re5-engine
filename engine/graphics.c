#include "engine.h"

Display					*display;
Window					window;
GC						gc;
XSetWindowAttributes	window_attrs;
unsigned long			window_mask;
int						screen;

int	setupWindow(int width, int height, const char *name)
{
	// open connection to the X server
	display = XOpenDisplay(NULL);
	if (!display) {
		// fprintf(2, "Cannot open display\n");
		return (1);
	}

	screen = DefaultScreen(display);

	// create a window
	window_attrs.background_pixel = WhitePixel(display, screen);
	window_attrs.event_mask = ExposureMask | KeyPressMask;
	window_mask = CWBackPixel | CWEventMask;

	window = XCreateWindow(
		display,
		RootWindow(display, screen),
		0, 0, width, height, 0,
		CopyFromParent, InputOutput, CopyFromParent,
		window_mask, &window_attrs
	);

	// set window title
	XStoreName(display, window, name);

	// show the window
	XMapWindow(display, window);

	// Create a graphics context
	gc = XCreateGC(display, window, 0, NULL);

	return (0);
}

void	stopWindow(void)
{
	XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}