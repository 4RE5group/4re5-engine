#include "engine.h"
#include "config.h"
#include <unistd.h>

/*
	Apply translation to object to make it go to new_pos in delta_t frames
*/
void	__ARESengine__translate(Scene *scene, int obj_index, Vect3 new_pos, float delta_t)
{
	if (!scene || obj_index < 0 || obj_index >= MAX_OBJECTS_PER_SCENE)
	{
		__builtin_printf("error: invalid obj index or scene. could not translate.\n");
		return ;
	}
	Vect3 translation;
	vectSub(&translation, &scene->objects[obj_index].pos, &new_pos); // calculate translation vector

	// abs vector
	translation.x = ABS(translation.x);
	translation.y = ABS(translation.y);
	translation.z = ABS(translation.z);

	vectDiv(&translation, delta_t);
	for (int t = 0; t < delta_t; t++)
	{
		vectAdd(&scene->objects[obj_index].pos, &scene->objects[obj_index].pos, &translation);
		__ARESengine__displayUpdate(scene);
		usleep(10000);
	}
}