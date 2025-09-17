#include "engine.h"
#include "config.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int	__ARESengine__loadObjectFromFile(Scene *scene, char *path, Vect3 pos, Vect3 scale, Vect3 rotation)
{
	int		status = 4096;
	char	buffer[status];
	int		fd = open(path, O_RDONLY);
	int		i;
	if (fd == -1 || !scene)
	{
		__builtin_printf("error: could not find file: '%s'\n", path);
		return (1);
	}

	if (scene->object_count + 1 >= MAX_VERTICES_PER_OBJECT)
	{
		__builtin_printf("error: scene is full\n");
		return (1);
	}
	// add new object
	// reset counts
	scene->objects[scene->object_count].vertices_count = 0;
	scene->objects[scene->object_count].faces_count = 0;

	// set pos
	scene->objects[scene->object_count].pos.x = pos.x;
	scene->objects[scene->object_count].pos.y = pos.y;
	scene->objects[scene->object_count].pos.z = pos.z;

	// set scale
	scene->objects[scene->object_count].scale.x = scale.x;
	scene->objects[scene->object_count].scale.y = scale.y;
	scene->objects[scene->object_count].scale.z = scale.z;

	// set rotation
	scene->objects[scene->object_count].rotation.x = rotation.x;
	scene->objects[scene->object_count].rotation.y = rotation.y;
	scene->objects[scene->object_count].rotation.z = rotation.z;

	while (status)
	{
		status = read(fd, buffer, status);
		i = 0;
		long	line_num = -1;
		while (i < status)
		{
			char	line[256];
			int		j = 0;
			
			line_num++;

			// copy line
			while (buffer[i+j] != '\n' && i+j < status)
			{
				line[j] = buffer[i+j];
				j++;
			}
			line[j] = '\0'; // ensure null terminator
			i += j+1;

			if (j == 0 || line[0] == '#' || line[0] == ' ') // skip comments & empty lines
				continue;
			if (line[0] == 'f' && line[1] == ' ') // thats a face input
			{
				j = 1; // start at offset 2 (skip 'f') then skip spaces
				while (line[j] == ' ')
					j++;
				double x = parseDouble(&line[j], &j); // increment j to end of double
				while (line[j] == ' ')
					j++;
				
				double y = parseDouble(&line[j], &j);
				while (line[j] == ' ')
					j++;

				double z = parseDouble(&line[j], &j);
				while (line[j] == ' ')
					j++;

				double w = -1.0;

				if (line[j] >= '0' && line[j] <= '9') // if thats a polygon that has more faces than a triangle
				{
					w = parseDouble(&line[j], &j);
					while (line[j] == ' ')
						j++;
				}
				// TODO: handle n-gons by transforming them into triangles
				
				if (line[j] != '\0' && line[j] != '#') // check line end / comment
				{
					__builtin_printf("error: invalid face line at %s:%ld\n	line: '%s'\n", path, line_num, line);
					return (1);
				}
				// fill the faces
				scene->objects[scene->object_count].faces[scene->objects[scene->object_count].faces_count][0] = x;
				scene->objects[scene->object_count].faces[scene->objects[scene->object_count].faces_count][1] = y;
				scene->objects[scene->object_count].faces[scene->objects[scene->object_count].faces_count][2] = z;
				scene->objects[scene->object_count].faces[scene->objects[scene->object_count].faces_count][3] = w;
				scene->objects[scene->object_count].faces_count++;
			}
			else if (line[0] == 'v' && line[1] == ' ') // thats a vertice input
			{
				j = 1; // start at offset 2 (skip 'v') then skip spaces
				while (line[j] == ' ')
					j++;
				double x = parseDouble(&line[j], &j); // increment j to end of double
				while (line[j] == ' ')
					j++;
				
				double y = parseDouble(&line[j], &j);
				while (line[j] == ' ')
					j++;

				double z = parseDouble(&line[j], &j);
				while (line[j] == ' ')
					j++;
				
				if (line[j] != '\0' && line[j] != '#') // check line end / comment
				{
					__builtin_printf("error: invalid vertice line at %s:%ld\n	line: '%s'\n", path, line_num, line);
					return (1);
				}
				// fill the vertices
				scene->objects[scene->object_count].vertices[scene->objects[scene->object_count].vertices_count][0] = x;
				scene->objects[scene->object_count].vertices[scene->objects[scene->object_count].vertices_count][1] = y;
				scene->objects[scene->object_count].vertices[scene->objects[scene->object_count].vertices_count][2] = z;
				scene->objects[scene->object_count].vertices[scene->objects[scene->object_count].vertices_count][3] = 1;
				scene->objects[scene->object_count].vertices_count++;
			}
		}
	}
	__builtin_printf("Successfully loaded %s (%i vertices, %i faces)\n", path, scene->objects[scene->object_count].vertices_count, scene->objects[scene->object_count].faces_count);
	scene->object_count++;
	return (0);
}