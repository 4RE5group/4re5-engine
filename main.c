#include "engine.h"
#include "config.h"
#include <unistd.h>

int main(void)
{
	// test obj input
	Scene	scene;
	scene.object_count = 0; // important!

	__ARESengine__loadObjectFromFile(&scene, "./models/cube.obj", (Vect3){0, 0, 0}, (Vect3){1, 1, 1});
	// __ARESengine__loadObjectFromFile(&scene, "./models/pyramid.obj", (Vect3){0, 1, 0}, (Vect3){1, 1, 1});
	// __ARESengine__loadObjectFromFile(&scene, "./models/cube.obj", (Vect3){5, 2, 7}, (Vect3){1, 1, 1});


	__ARESengine__Init("4re5-engine test", 1200, 900);
	
	XEvent  event;
	while (1)
	{
        XNextEvent(display, &event);
        if (event.type == KeyPress) {
			unsigned int KeyEventCode = event.xkey.keycode;

			__ARESengine__updateDirections();
			__builtin_printf("forward:  x=%f, y=%f, z=%f\n", forward.x, forward.y, forward.z);
			__builtin_printf("	right:  x=%f, y=%f, z=%f\n", right.x, right.y, right.z);
			__builtin_printf("	camera: x=%f, y=%f, z=%f\n", camera_pos.x, camera_pos.y, camera_pos.z);
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
				// reset pos
				// scene.objects[2].pos.x = camera_pos.x;
				// scene.objects[2].pos.y = camera_pos.y;
				// scene.objects[2].pos.z = camera_pos.z;
				// __ARESengine__displayUpdate(&scene);

				// Vect3 newpos;
				// vectAdd(&newpos, &camera_pos, &look_at);
				// newpos.x *= 5;
				// newpos.y *= 5;
				// newpos.z *= 5;
				// __ARESengine__translate(&scene, 2, newpos, 20);
				// __builtin_printf("newpos: (%f, %f, %f)\n", newpos.x, newpos.y, newpos.z);
				// __builtin_printf("	forward: (%f, %f, %f)\n", forward.x, forward.y, forward.z);
				// __builtin_printf("	camera_pos: (%f, %f, %f)\n", camera_pos.x, camera_pos.y, camera_pos.z);
				// vectAdd(&camera_pos, &camera_pos, &(Vect3){0, 1, 0});
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

	stopWindow();
}
