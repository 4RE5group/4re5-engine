#include "engine.h"

Display					*display;
Window					window;
GC						gc;
XSetWindowAttributes	window_attrs;
unsigned long			window_mask;
int						screen;

int	setupWindow(int width, int height, const char *name)
{
	// Open connection to the X server
	display = XOpenDisplay(NULL);
	if (!display) {
		// fprintf(2, "Cannot open display\n");
		return (1);
	}

	screen = DefaultScreen(display);

	// Create a window
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

	// Set window title
	XStoreName(display, window, name);

	// Map (show) the window
	XMapWindow(display, window);

	// Create a graphics context
	gc = XCreateGC(display, window, 0, NULL);

	// Set the foreground color to red
	XSetForeground(display, gc, 0xff0000);

	return (0);
}

void	stopWindow(void)
{
	XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}