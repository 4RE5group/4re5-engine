#include "engine.h"
#include "config.h"

int		__ARESengine__loadObjectFromFile(Scene *scene, char *path, Vect3 pos, Vect3 scale, Vect3 rotation)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1 || !scene) {
        __builtin_printf("error: could not find file: '%s'\n", path);
        if (fd != -1) close(fd);
        return 1;
    }

    if (scene->object_count + 1 >= MAX_OBJECTS_PER_SCENE) {
        __builtin_printf("error: scene is full\n");
        close(fd);
        return 1;
    }

    // Initialize the object
    Object *obj = &scene->objects[scene->object_count];
    obj->vertices_count = 0;
    obj->faces_count = 0;
    obj->uvs_count = 0;
    obj->pos = pos;
    obj->scale = scale;
    obj->rotation = rotation;

    // Allocate initial buffers (will be reallocated as needed)
    size_t vertices_capacity = 32;
    size_t faces_capacity = 32;
    size_t uvs_capacity = 32;

    obj->vertices = malloc(vertices_capacity * sizeof(Vertex));
    obj->uvs = malloc(uvs_capacity * sizeof(UVCoord));
    obj->faces = malloc(faces_capacity * sizeof(int[4]));
    obj->uv_indices = malloc(faces_capacity * sizeof(int[4]));

    if (!obj->vertices || !obj->uvs || !obj->faces || !obj->uv_indices) {
        __builtin_printf("error: could not allocate memory for object\n");
        free(obj->vertices);
        free(obj->uvs);
        free(obj->faces);
        free(obj->uv_indices);
        close(fd);
        return 1;
    }

    // Read file in chunks
    char *buffer = malloc(4096);
    if (!buffer) {
        __builtin_printf("error: could not allocate buffer\n");
        free(obj->vertices);
        free(obj->uvs);
        free(obj->faces);
        free(obj->uv_indices);
        close(fd);
        return 1;
    }

    ssize_t status;
    while ((status = read(fd, buffer, 4096)) > 0) {
        int i = 0;
        long line_num = -1;
        while (i < status) {
            char line[256];
            int j = 0;
            line_num++;

            // Copy line
            while (i + j < status && buffer[i + j] != '\n') {
                line[j] = buffer[i + j];
                j++;
            }
            line[j] = '\0'; // Ensure null terminator
            i += j + 1;
            if (j == 0 || line[0] == '#' || line[0] == ' ') // Skip comments & empty lines
                continue;

            if (line[0] == 'v' && line[1] == ' ') // Vertex input
            {
                j = 1; // Start at offset 2 (skip 'v') then skip spaces
                while (line[j] == ' ')
                    j++;
                double x = parseDouble(&line[j], &j);
                while (line[j] == ' ')
                    j++;
                double y = parseDouble(&line[j], &j);
                while (line[j] == ' ')
                    j++;
                double z = parseDouble(&line[j], &j);
                while (line[j] == ' ')
                    j++;

                if (line[j] != '\0' && line[j] != '#') // Check line end / comment
                {
                    __builtin_printf("error: invalid vertex line at %s:%ld\n\tline: '%s'\n", path, line_num, line);
                    free(buffer);
                    free(obj->vertices);
                    free(obj->uvs);
                    free(obj->faces);
                    free(obj->uv_indices);
                    close(fd);
                    return 1;
                }

                // Resize vertices array if needed
                if (obj->vertices_count >= vertices_capacity) {
                    vertices_capacity *= 2;
                    Vertex *new_vertices = realloc(obj->vertices, vertices_capacity * sizeof(Vertex));
                    if (!new_vertices) {
                        __builtin_printf("error: could not reallocate vertices\n");
                        free(buffer);
                        free(obj->vertices);
                        free(obj->uvs);
                        free(obj->faces);
                        free(obj->uv_indices);
                        close(fd);
                        return 1;
                    }
                    obj->vertices = new_vertices;
                }

                // Fill the vertices
                obj->vertices[obj->vertices_count].x = x;
                obj->vertices[obj->vertices_count].y = y;
                obj->vertices[obj->vertices_count].z = z;
                obj->vertices[obj->vertices_count].w = 1;
                obj->vertices_count++;
            }
            else if (line[0] == 'v' && line[1] == 't') // UV coordinate input
            {
                j = 2; // Start at offset 3 (skip 'vt') then skip spaces
                while (line[j] == ' ')
                    j++;
                double u = parseDouble(&line[j], &j);
                while (line[j] == ' ')
                    j++;
                double v = parseDouble(&line[j], &j);
                while (line[j] == ' ')
                    j++;

                if (line[j] != '\0' && line[j] != '#') // Check line end / comment
                {
                    __builtin_printf("error: invalid UV line at %s:%ld\n\tline: '%s'\n", path, line_num, line);
                    free(buffer);
                    free(obj->vertices);
                    free(obj->uvs);
                    free(obj->faces);
                    free(obj->uv_indices);
                    close(fd);
                    return 1;
                }

                // Resize UVs array if needed
                if (obj->uvs_count >= uvs_capacity) {
                    uvs_capacity *= 2;
                    UVCoord *new_uvs = realloc(obj->uvs, uvs_capacity * sizeof(UVCoord));
                    if (!new_uvs) {
                        __builtin_printf("error: could not reallocate UVs\n");
                        free(buffer);
                        free(obj->vertices);
                        free(obj->uvs);
                        free(obj->faces);
                        free(obj->uv_indices);
                        close(fd);
                        return 1;
                    }
                    obj->uvs = new_uvs;
                }

                // Fill the UVs
                obj->uvs[obj->uvs_count].u = u;
                obj->uvs[obj->uvs_count].v = v;
                obj->uvs_count++;
            }
            else if (line[0] == 'f' && line[1] == ' ') // Face input
            {
                j = 1; // Start at offset 2 (skip 'f') then skip spaces
                while (line[j] == ' ')
                    j++;

                // Resize faces and UV indices arrays if needed
                if (obj->faces_count >= faces_capacity) {
                    faces_capacity *= 2;
                    int (*new_faces)[4] = realloc(obj->faces, faces_capacity * sizeof(int[4]));
                    int (*new_uv_indices)[4] = realloc(obj->uv_indices, faces_capacity * sizeof(int[4]));
                    if (!new_faces || !new_uv_indices) {
                        __builtin_printf("error: could not reallocate faces or UV indices\n");
                        free(buffer);
                        free(obj->vertices);
                        free(obj->uvs);
                        free(obj->faces);
                        free(obj->uv_indices);
                        close(fd);
                        return 1;
                    }
                    obj->faces = new_faces;
                    obj->uv_indices = new_uv_indices;
                }

                int vertex_indices[4] = {0};
                int uv_indices[4] = {0};
                int count = 0;

                // Parse face indices (e.g., "1/1 2/2 3/3 4/4")
                for (int k = 0; k < 4; k++) {
                    vertex_indices[k] = (int)parseDouble(&line[j], &j);
                    if (line[j] == '/') {
                        j++; // Skip '/'
                        uv_indices[k] = (int)parseDouble(&line[j], &j);
                    }
                    while (line[j] == ' ')
                        j++;
                    if (line[j] == '\0' || line[j] == '#')
                        break;
                    count++;
                }

                if (line[j] != '\0' && line[j] != '#') // Check line end / comment
                {
                    __builtin_printf("error: invalid face line at %s:%ld\n\tline: '%s'\n", path, line_num, line);
                    free(buffer);
                    free(obj->vertices);
                    free(obj->uvs);
                    free(obj->faces);
                    free(obj->uv_indices);
                    close(fd);
                    return 1;
                }

                // Fill the faces and UV indices
                for (int k = 0; k < count; k++) {
                    obj->faces[obj->faces_count][k] = vertex_indices[k];
                    obj->uv_indices[obj->faces_count][k] = uv_indices[k];
                }
                obj->faces_count++;
            }
        }
    }

    free(buffer);

    __builtin_printf("Successfully loaded %s (%li vertices, %li faces, %li UVs)\n",
                     path,
                     obj->vertices_count,
                     obj->faces_count,
                     obj->uvs_count);

    scene->object_count++;
    close(fd);
    return 0;
}

