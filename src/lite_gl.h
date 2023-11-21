#ifndef LITE_GL_H
#define LITE_GL_H

#include "glad/glad.h"
#include "lite.h"
#include "blib_file.h"
#include "blib_math.h"
#include "stb_image.h"
#include "lite_gl_transform.h"

typedef struct lite_gl_mesh_t lite_gl_mesh_t;
struct lite_gl_mesh_t {
	GLfloat* vertexData;
	GLuint numVertices;
	GLuint* indexData;
	GLuint numIndices;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};

typedef struct lite_gl_camera_t lite_gl_camera_t;
struct lite_gl_camera_t {
	lite_gl_transform_t transform;
	blib_vec3f_t U;
	blib_vec3f_t V;
	blib_vec3f_t N;
	blib_mat4_t projectionMatrix;
	blib_mat4_t viewMatrix;
	float fov;
};

typedef struct lite_gl_gameObject_t lite_gl_gameObject_t;
struct lite_gl_gameObject_t {
	lite_gl_transform_t transform;
	lite_gl_mesh_t mesh;
	GLuint shader;
	GLuint texture;
	bool active;
};

/*TODO GET RID OF THESE!!*/
#define _TEST_vertexDataLength 64
#define _TEST_indexDataLength 36
extern float _TEST_vertexData[_TEST_vertexDataLength];
extern GLuint _TEST_indexData[_TEST_indexDataLength];
extern lite_gl_gameObject_t TESTgameObject;
extern lite_gl_camera_t TESTcamera;

void lite_gl_initialize(lite_engine_instance_t* instance);
GLuint lite_gl_pipeline_create();
lite_gl_mesh_t lite_gl_mesh_create(
		GLuint numIndices, GLuint numVertices, GLuint* indexData, 
		GLfloat* vertexData);
lite_gl_gameObject_t lite_gl_gameObject_create();
GLuint lite_gl_texture_create(const char* imageFile);
lite_gl_camera_t lite_gl_camera_create(float fov);

#endif  /*LITE_GL_H*/
