#include "engine.h"

int main(void)
{
	setupWindow(800, 600, "4re5-engine test 1");
	engineInit();
	
	XEvent  event;
	while (1)
	{
        XNextEvent(display, &event);
        if (event.type == Expose) {
            // Draw a red pixel at (10, 10)
            // XDrawPoint(display, window, gc, 10, 10);
            // XFlush(display);
			transform_to_screen();
			XFlush(display);
        }
        if (event.type == KeyPress) {
			XClearWindow(display, window);
			camera_pos.x++;
			engineInit();
            transform_to_screen();
			XFlush(display);
        }
    }

	stopWindow();
}
