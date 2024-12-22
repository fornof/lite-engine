#include "lite_gl.h"

#include <ctype.h>

DEFINE_LIST(mesh_t)
DEFINE_LIST(vertex_t)

mesh_t mesh_alloc(list_vertex_t vertices, list_GLuint indices) {
	mesh_t m   = {0};
	m.enabled  = true;
	m.vertices = vertices;
	m.indices  = indices;

	glGenVertexArrays(1, &m.VAO);
	glGenBuffers(1, &m.VBO);
	glGenBuffers(1, &m.EBO);

	glBindVertexArray(m.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * vertices.length, vertices.array,
			GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.length, indices.array,
			GL_STATIC_DRAW);

	GLuint vertStride = sizeof(vertex_t); // how many bytes per vertex?

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertStride,
			(void *)offsetof(vertex_t, position));
	glEnableVertexAttribArray(0);

	// texcoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertStride,
			(void *)offsetof(vertex_t, texCoord));
	glEnableVertexAttribArray(1);

	// normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertStride,
			(void *)offsetof(vertex_t, normal));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return m;
}

mesh_t mesh_lmod_alloc(const char* file_path) {
	debug_log("Loading lmod file from '%s'", file_path);
	file_buffer fb = file_buffer_alloc(file_path);

	if (fb.error) {
		debug_error(
				"Failed to open .lmod file at '%s' did you specity the correct path?", 
				file_path);
		assert(0);
	}

	list_vector3_t positions  = list_vector3_t_alloc();
	list_vector3_t normals    = list_vector3_t_alloc();
	list_vector2_t tex_coords = list_vector2_t_alloc();
	list_GLuint indices       = list_GLuint_alloc();

	enum {
		STATE_INITIAL = -1,
		STATE_POSITION,
		STATE_INDICES,
		STATE_NORMAL,
		STATE_TEX_COORD,
	};
	uint8_t state = STATE_INITIAL;

	for(char *c = fb.text; c < fb.text+fb.length; c++) {

		if (isspace(*c)) {
			continue;
		} else if (*c == '#') {
			while(*c != '\n' && *c != '\0') { c++; }
			continue;
		}

		char token[128];
		{
			int num_tokens = sscanf(c, "%s", token);
			if (num_tokens != 1) {
				debug_error("Failed to read token at '%s'", file_path);
				assert(0);
			}
		}

		// TODO The order in which a buffer is loaded from the lmod file matters.
		// need to change the importer so that it can handle parsing the buffers
		// in any order.

		if (strcmp(token, "vertex_indices:") == 0 || state == STATE_INDICES) {
			if (state != STATE_INDICES)
				c += sizeof("vertex_indices:");

			state = STATE_INDICES;

			GLuint index;
			int num_tokens = sscanf(c, "%u", &index);
			if (num_tokens != 1) {
				debug_error("Failed to read vertex_indices at '%s'", file_path);
				assert(0);
			}
			while(*c != ' ' && *c != '\0') { c++; } // skip the rest of the number

			//debug_error("%u ", index);
			list_GLuint_add(&indices, index);
		}

		if (strcmp(token, "vertex_texture_coordinates:") == 0 || state == STATE_TEX_COORD) {
			if (state != STATE_TEX_COORD)
				c += sizeof("vertex_texture_coordinates:");

			state = STATE_TEX_COORD;

			vector2_t tex_coord = {0};
			int num_tokens = sscanf(c, "%f %f", &tex_coord.x, &tex_coord.y);
			if (num_tokens != 2) {
				debug_error("Failed to read vertex_texture_coordinates at '%s'", file_path);
				assert(0);
			}

			list_vector2_t_add(&tex_coords, tex_coord);
			//vector2_print(tex_coords.array[tex_coords.length-1], "tex coord");
			while (*c != '\n' && *c != '\0') { c++; } 
		}

		if (strcmp(token, "vertex_normals:") == 0 || state == STATE_NORMAL) {
			if (state != STATE_NORMAL)
				c += sizeof("vertex_normals:");

			state = STATE_NORMAL;

			vector3_t normal = {0};
			int num_tokens = sscanf(c, "%f %f %f", &normal.x, &normal.y, &normal.z);
			if (num_tokens != 3) {
				debug_error("Failed to read vertex_normals at '%s'", file_path);
				assert(0);
			}

			//vector3_print(normal, "normal");
			list_vector3_t_add(&normals, normal);
			while (*c != '\n' && *c != '\0') { c++; } 
		}

		if (strcmp(token, "vertex_positions:") == 0 || state == STATE_POSITION) {
			if (state != STATE_POSITION)
				c += sizeof("vertex_positions:");

			state = STATE_POSITION;

			vector3_t position = {0};
			int num_tokens = sscanf(c, "%f %f %f", &position.x, &position.y, &position.z);
			if (num_tokens != 3) {
				debug_error("Failed to read vertex_positions at '%s'", file_path);
				assert(0);
			}

			//vector3_print(position, "position");
			list_vector3_t_add(&positions, position);
			while (*c != '\n' && *c != '\0') { c++; } 
		}
	}

	file_buffer_free(fb);

	list_vertex_t vertices = list_vertex_t_alloc();
	for(size_t i = 0; i < positions.length; i++) {
		vertex_t vertex = {0};
		if (positions.length > 0)
		vertex.position = positions.array[i];
		if (normals.length > 0)
		vertex.normal   = normals.array[i];
		if (tex_coords.length > 0)
		vertex.texCoord = tex_coords.array[i];
		list_vertex_t_add(&vertices, vertex);
	}

	list_vector3_t_free(&positions);
	list_vector3_t_free(&normals);
	list_vector2_t_free(&tex_coords);

	mesh_t mesh = mesh_alloc(vertices, indices);
	return mesh;
}

void mesh_free(mesh_t* mesh) {
	list_vertex_t_free(&mesh->vertices);
	list_GLuint_free(&mesh->indices);
}