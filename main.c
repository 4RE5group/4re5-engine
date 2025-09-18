#include "engine.h"
#include "config.h"
#include <unistd.h>

int		main(void)
{
	Scene	scene;
	scene.object_count = 0; // important!

	__ARESengine__loadObjectFromFile(&scene, "./models/cube.obj", (Vect3){0, 0, 0}, (Vect3){1, 1, 1}, (Vect3){0, 0, 0});
	// __ARESengine__loadObjectFromFile(&scene, "./models/pyramid.obj", (Vect3){0, 1, 0}, (Vect3){1, 1, 1}, (Vect3){0, 0, 0});
	// __ARESengine__loadObjectFromFile(&scene, "./models/cube.obj", (Vect3){3, 3, 3}, (Vect3){1, 1, 1}, (Vect3){0, 0, 0});

	__ARESengine__Init("4re5-engine test", 800, 600);
	
	XEvent  event;
	while (1)
	{
        XNextEvent(display, &event);
        if (event.type == KeyPress) {
			unsigned int KeyEventCode = event.xkey.keycode;

			// __builtin_printf("pressed key: %u\n", KeyEventCode);
			// movement
			if (KeyEventCode == KEY_UP)
			{
				vectAdd(&camera_pos, &camera_pos, &forward);
				vectAdd(&look_at, &look_at, &forward);
			}
			else if (KeyEventCode == KEY_DOWN)
			{
				vectSub(&camera_pos, &camera_pos, &forward);
				vectSub(&look_at, &look_at, &forward);
			}
			else if (KeyEventCode == KEY_LEFT)
			{
				vectAdd(&camera_pos, &camera_pos, &right);
				vectAdd(&look_at, &look_at, &right);
			}
			else if (KeyEventCode == KEY_RIGHT)
			{
				vectSub(&camera_pos, &camera_pos, &right);
				vectSub(&look_at, &look_at, &right);
			}
			else if (KeyEventCode == KEY_SPACE)
			{
				__ARESengine__rotate(&scene, 0, (Vect3){PI/6, PI/6, PI/6}, 20);
			}

			// cam
			else if (KeyEventCode == KEY_CAM_UP)
				__ARESengine__rotateCamera(-0.05, 0);
			else if (KeyEventCode == KEY_CAM_DOWN)
				__ARESengine__rotateCamera(0.05, 0);
			else if (KeyEventCode == KEY_CAM_LEFT)
				__ARESengine__rotateCamera(0, -0.05);
			else if (KeyEventCode == KEY_CAM_RIGHT)
				__ARESengine__rotateCamera(0, 0.05);
			else if (KeyEventCode == 9) // ESC
				break;
	        }
		__ARESengine__displayUpdate(&scene);
		usleep(FPS * 100);
    }

	free(texture1.pixels);

	__ARESengine__cleanup(&scene);
	stopWindow();
}
