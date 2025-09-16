#include "engine.h"
#include "config.h"

int main(void)
{
	__ARESengine__Init("4re5-engine test", 800, 600);
	Vect3	forward_vect;
	Vect3	right_vect;
	
	XEvent  event;
	while (1)
	{
        XNextEvent(display, &event);
        if (event.type == Expose) {
			__ARESengine__displayUpdate(); // init display
        }
        if (event.type == KeyPress) {
			XClearWindow(display, window);

			unsigned int KeyEventCode = event.xkey.keycode;
			// __builtin_printf("%i\n", KeyEventCode);
			vectSub(&forward_vect, &look_at, &camera_pos); // forward = look_at - camera_pos
			vectDiv(&forward_vect, vectNorm(&forward_vect)); // norm the vector

			vectCross(&right_vect, &up, &forward_vect); // right = up x forward
			vectDiv(&right_vect, vectNorm(&right_vect)); // norm the vector
			 
			if (KeyEventCode == KEY_UP)
				vectAdd(&camera_pos, &camera_pos, &forward_vect);
			else if (KeyEventCode == KEY_DOWN)
				vectSub(&camera_pos, &camera_pos, &forward_vect);
			else if (KeyEventCode == KEY_LEFT)
			{
				vectAdd(&camera_pos, &camera_pos, &right_vect);
				vectAdd(&look_at, &look_at, &right_vect);
			}
			else if (KeyEventCode == KEY_RIGHT)
			{
				vectSub(&camera_pos, &camera_pos, &right_vect);
				vectSub(&look_at, &look_at, &right_vect);
			}

			// cam
			else if (KeyEventCode == KEY_CAM_UP)
				look_at.y++;
			else if (KeyEventCode == KEY_CAM_DOWN)
				look_at.y--;
			else if (KeyEventCode == KEY_CAM_LEFT)
				look_at.x++;
			else if (KeyEventCode == KEY_CAM_RIGHT)
				look_at.x--;
			else if (KeyEventCode == 9) // ESC
				break;
			
            __ARESengine__displayUpdate();
        }
    }

	stopWindow();
}
