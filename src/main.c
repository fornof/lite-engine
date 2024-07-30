#include "gl.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "blib/b_list.h"
B_LIST_IMPLEMENTATION
DECLARE_LIST(vector3_t)
DEFINE_LIST(vector3_t)
DECLARE_LIST(matrix4_t)
DEFINE_LIST(matrix4_t)
DECLARE_LIST(quaternion_t)
DEFINE_LIST(quaternion_t)

#include "blib/b_math.h"
#include "b_physics.h" 

#define ASSERT_UNIMPLEMENTED 0

typedef struct {
  matrix4_t modelMatrix;
  vector3_t position;
  quaternion_t rotation;
} transform_t;

static inline vector3_t transform_basis_forward(transform_t t,
                                                float magnitude) {
  return vector3_rotate(vector3_forward(magnitude), t.rotation);
}

static inline vector3_t transform_basis_up(transform_t t, float magnitude) {
  return vector3_rotate(vector3_up(magnitude), t.rotation);
}

static inline vector3_t transform_basis_right(transform_t t, float magnitude) {
  return vector3_rotate(vector3_right(magnitude), t.rotation);
}

static inline vector3_t transform_basis_back(transform_t t, float magnitude) {
  return vector3_rotate(vector3_back(magnitude), t.rotation);
}

static inline vector3_t transform_basis_down(transform_t t, float magnitude) {
  return vector3_rotate(vector3_down(magnitude), t.rotation);
}

static inline vector3_t transform_basis_left(transform_t t, float magnitude) {
  return vector3_rotate(vector3_left(magnitude), t.rotation);
}

DECLARE_LIST(transform_t)
DEFINE_LIST(transform_t)

static const unsigned int NUM_POINT_LIGHTS = 10;
typedef struct {
  list_transform_t transforms;
  list_vector3_t colors;
  mesh meshes;
} pointLight_t;

typedef struct {
  list_transform_t transforms;
  list_vector3_t colors;
  mesh meshes;
} cube_t;

typedef struct {
  transform_t transform;
  float lookSensitivity;
  float lastX;
  float lastY;
} camera_t;

#if 1
static void error_callback(int error, const char *description) {
  (void)error;
  fprintf(stderr, "Error: %s\n", description);
  {}
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  (void)scancode;
  (void)mods;

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

static void framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height) {
  (void)window;
  glViewport(0, 0, width, height);
}

static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
                                   GLenum severity, GLsizei length,
                                   const char *message, const void *userParam) {
  (void)length;
  (void)userParam;

  // ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204) {
    return;
  }

  printf("---------------\n");
  printf("Debug message (%d) %s\n", id, message);

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    printf("Source: API");
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    printf("Source: Window System");
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    printf("Source: Shader Compiler");
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    printf("Source: Third Party");
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    printf("Source: Application");
    break;
  case GL_DEBUG_SOURCE_OTHER:
    printf("Source: Other");
    break;
  }
  printf("\n");

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    printf("Type: Error");
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    printf("Type: Deprecated Behaviour");
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    printf("Type: Undefined Behaviour");
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    printf("Type: Portability");
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    printf("Type: Performance");
    break;
  case GL_DEBUG_TYPE_MARKER:
    printf("Type: Marker");
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    printf("Type: Push Group");
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    printf("Type: Pop Group");
    break;
  case GL_DEBUG_TYPE_OTHER:
    printf("Type: Other");
    break;
  }
  printf("\n");

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    printf("Severity: high");
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    printf("Severity: medium");
    break;
  case GL_DEBUG_SEVERITY_LOW:
    printf("Severity: low");
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    printf("Severity: notification");
    break;
  }
  printf("\n");
  printf("\n");
}
typedef enum { 
	ENGINE_RENDERER_API_GL, 
	ENGINE_RENDERER_API_NONE,
}engine_renderer_API_t;

static GLFWwindow*	engine_window;
static int					engine_window_size_x;
static int 					engine_window_size_y;
static char*				engine_window_title;
static float 				engine_time_current; 
static float 				engine_time_last; 
static float 				engine_time_delta; 
static float 				engine_FPS;
static engine_renderer_API_t engine_renderer_API;

void engine_renderer_set_API(engine_renderer_API_t renderingAPI) {
	engine_renderer_API = renderingAPI;
}

//set window resolution
void engine_window_set_resolution(int x, int y) {
	glfwSetWindowSize(engine_window, x, y);
}

//position window in the center of the screen
void engine_window_set_position(int x, int y) {
		glfwSetWindowPos(engine_window, x, y);
}

void engine_start(void) {
  if (!glfwInit()) {
    printf("[ERROR_GLFW] Failed to initialize GLFW");
  }

  glfwSetErrorCallback(error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,
                 GLFW_TRUE); // comment to toggle debug mode

  engine_window =
      glfwCreateWindow(engine_window_size_x, engine_window_size_y, engine_window_title, NULL, NULL);
  if (!engine_window) {
    printf("[ERROR_GLFW] Failed to create GLFW window\n");
  }

  glfwMakeContextCurrent(engine_window);
  glfwSetKeyCallback(engine_window, key_callback);
  glfwSetFramebufferSizeCallback(engine_window, framebuffer_size_callback);

  if (!gladLoadGL()) {
    printf("[ERROR_GL] Failed to initialize GLAD\n");
  }

  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (!(flags & GL_CONTEXT_FLAG_DEBUG_BIT)) {
    printf("[ERROR_GL] Failed to set debug context flag\n");
  } else {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                          GL_TRUE);
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  int width, height;
  glfwGetFramebufferSize(engine_window, &width, &height);
  glViewport(0, 0, width, height);

  glfwSwapInterval(0);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
#endif
int point_light_create(void) {
//	list_int components = list_int_alloc();
//	list_int_add(components, COMPONENT_BASELINE);
//	list_int_add(components, COMPONENT_LIGHT_POINT);
//	entity e = entity_create();
//	entity_add_component(ENT_LIGHT_POINT);
	assert(ASSERT_UNIMPLEMENTED);
}

int main(void) {
  printf("Rev up those fryers!\n");

#if 1
	engine_renderer_set_API(ENGINE_RENDERER_API_GL); //choose your renderer
	engine_window_set_resolution(1280, 720); //set window resolution
	engine_window_set_position(0, 0); //position window in the center of the screen
	engine_start();

  GLuint diffuseShader = shader_create("res/shaders/diffuse.vs.glsl",
                                       "res/shaders/diffuse.fs.glsl");
  GLuint unlitShader =
      shader_create("res/shaders/unlit.vs.glsl", "res/shaders/unlit.fs.glsl");

  GLuint containerDiffuse = texture_create("res/textures/container2.png");
  GLuint lampDiffuse = texture_create("res/textures/glowstone.png");
  GLuint containerSpecular =
      texture_create("res/textures/container2_specular.png");

	for (int i = 0; i < list_cube.length; i++)
		cube_create();

	for (int i = 0; i < NUM_POINT_LIGHTS; i++)
		point_light_create();

	camera_t camera;
#else
  window windowData = window_create();
  glfwSetInputMode(windowData.glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


  camera_t cam = {
      .transform.modelMatrix = matrix4_identity(),
      .transform.position = vector3_zero(),
      .transform.rotation = quaternion_identity(),
      .lookSensitivity = 10,
      .lastX = 0,
      .lastY = 0,
  };

  cam.transform.position = (vector3_t){4, 2, -10};

  // cubes
  cube_t cubes;
  cubes.transforms = list_transform_t_alloc();
  cubes.colors = list_vector3_t_alloc();

  for (size_t i = 0; i < cubes.length; i++) {
    mesh_allocCube(&cubes.meshes);

    transform_t t = (transform_t){
        .position = (vector3_t){i * 2, 0, 0},
        .rotation = quaternion_identity(),
    };

    list_transform_t_add(&cubes.transforms, t);
    list_vector3_t_add(&cubes.colors, vector3_one(1.0f));
  }

  // lights
  pointLight_t pointLights;
  pointLights.transforms = list_transform_t_alloc();
  pointLights.colors = list_vector3_t_alloc();

  for (size_t i = 0; i < NUM_POINT_LIGHTS; i++) {
    mesh_allocCube(&pointLights.meshes);

    vector3_t color = vector3_one(0.5f);

    transform_t t = (transform_t){
        .position = (vector3_t){i * 16, -2, -2},
        .rotation = quaternion_identity(),
        .modelMatrix = matrix4_identity(),
    };

    list_vector3_t_add(&pointLights.colors, color);
    list_transform_t_add(&pointLights.transforms, t);
  }

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

#endif

  float aspect;
  matrix4_t projection = matrix4_identity();
  vector3_t ambientLight = vector3_one(0.1f);

  vector3_t look = vector3_zero();
  while (!glfwWindowShouldClose(engine_window)) {
    { // TIME
      engine_time_current = glfwGetTime();
      engine_time_delta = engine_time_current - engine_time_last;
      engine_time_last = engine_time_current;

      engine_FPS = 1 / engine_time_delta;
      // printf("============FRAME=START==============\n");
      // printf("delta %f : FPS %f\n", engine_time_delta, engine_FPS);
    }

    {   // INPUT
      { // mouse look
        static bool firstMouse = true;
        double mouseX, mouseY;
        glfwGetCursorPos(engine_window, &mouseX, &mouseY);

        if (firstMouse) {
          camera.lastX = mouseX;
          camera.lastY = mouseY;
          firstMouse = false;
        }

        float xoffset = mouseX - camera.lastX;
        float yoffset = mouseY - camera.lastY;
        camera.lastX = mouseX;
        camera.lastY = mouseY;
        float xangle = xoffset * engine_time_delta * camera.lookSensitivity;
        float yangle = yoffset * engine_time_delta * camera.lookSensitivity;

        look.x += yangle;
        look.y += xangle;

        look.y = loop(look.y, 2 * PI);
        look.x = clamp(look.x, -PI * 0.5, PI * 0.5);

        camera.transform.rotation = quaternion_from_euler(look);
      }

      { // movement
        float cameraSpeed = 15 * engine_time_delta;
        vector3_t movement = vector3_zero();

        movement.x = glfwGetKey(engine_window, GLFW_KEY_D) -
                     glfwGetKey(engine_window, GLFW_KEY_A);
        movement.y = glfwGetKey(engine_window, GLFW_KEY_SPACE) -
                     glfwGetKey(engine_window, GLFW_KEY_LEFT_SHIFT);
        movement.z = glfwGetKey(engine_window, GLFW_KEY_W) -
                     glfwGetKey(engine_window, GLFW_KEY_S);

        movement = vector3_normalize(movement);
        movement = vector3_scale(movement, cameraSpeed);
        movement = vector3_rotate(movement, camera.transform.rotation);

        camera.transform.position = vector3_add(camera.transform.position, movement);
      }
    }

    glfwGetWindowSize(engine_window, &engine_window_size_x,
                      &engine_window_size_y);
    aspect = (float)engine_window_size_x / (float)engine_window_size_y;
    projection = matrix4_perspective(deg2rad(90), aspect, 0.1f, 1000.0f);

    { // draw
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // view matrix
      camera.transform.modelMatrix =
          matrix4_translation(vector3_negate(camera.transform.position));
      camera.transform.modelMatrix = matrix4_multiply(
          camera.transform.modelMatrix,
          quaternion_to_matrix4(quaternion_conjugate(camera.transform.rotation)));
      // matrix4_print(camera.transform.modelMatrix, "view");

      glUseProgram(diffuseShader);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, containerDiffuse);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, containerSpecular);

      // camera
      shader_setUniformV3(diffuseShader, "u_cameraPos", camera.transform.position);

      // directional light
      shader_setUniformV3(diffuseShader, "u_dirLight.direction",
                          (vector3_t){-0.2f, -1.0f, -0.3f});
      shader_setUniformV3(diffuseShader, "u_dirLight.ambient", ambientLight);
      shader_setUniformV3(diffuseShader, "u_dirLight.diffuse",
                          (vector3_t){0.4f, 0.4f, 0.4f});
      shader_setUniformV3(diffuseShader, "u_dirLight.specular",
                          (vector3_t){0.5f, 0.5f, 0.5f});

      // point light 0
      shader_setUniformV3(diffuseShader, "u_pointLights[0].position",
                          pointLights.transforms.data[0].position);
      shader_setUniformV3(diffuseShader, "u_pointLights[0].ambient",
                          ambientLight);
      shader_setUniformV3(diffuseShader, "u_pointLights[0].diffuse",
                          (vector3_t){0.8f, 0.8f, 0.8f});
      shader_setUniformV3(diffuseShader, "u_pointLights[0].specular",
                          (vector3_t){1.0f, 1.0f, 1.0f});
      shader_setUniformFloat(diffuseShader, "u_pointLights[0].constant", 1.0f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[0].linear", 0.09f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[0].quadratic",
                             0.032f);

      // spot light
      shader_setUniformV3(diffuseShader, "u_spotLight.position",
                          camera.transform.position);
      shader_setUniformV3(diffuseShader, "u_spotLight.direction",
                          transform_basis_back(camera.transform, 1.0));

      shader_setUniformV3(diffuseShader, "u_spotLight.ambient", ambientLight);
      shader_setUniformV3(diffuseShader, "u_spotLight.diffuse",
                          (vector3_t){1.0f, 1.0f, 1.0f});
      shader_setUniformV3(diffuseShader, "u_spotLight.specular",
                          (vector3_t){1.0f, 1.0f, 1.0f});
      shader_setUniformFloat(diffuseShader, "u_spotLight.constant", 1.0f);
      shader_setUniformFloat(diffuseShader, "u_spotLight.linear", 0.09f);
      shader_setUniformFloat(diffuseShader, "u_spotLight.quadratic", 0.032f);
      shader_setUniformFloat(diffuseShader, "u_spotLight.cutOff", PI / 4.0);
      shader_setUniformFloat(diffuseShader, "u_spotLight.outerCutOff",
                             PI / 4.4);

      // point light 1
      shader_setUniformV3(diffuseShader, "u_pointLights[1].position",
                          pointLights.transforms.data[1].position);
      shader_setUniformV3(diffuseShader, "u_pointLights[1].ambient",
                          ambientLight);
      shader_setUniformV3(diffuseShader, "u_pointLights[1].diffuse",
                          (vector3_t){0.8f, 0.8f, 0.8f});
      shader_setUniformV3(diffuseShader, "u_pointLights[1].specular",
                          (vector3_t){1.0f, 1.0f, 1.0f});
      shader_setUniformFloat(diffuseShader, "u_pointLights[1].constant", 1.0f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[1].linear", 0.09f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[1].quadratic",
                             0.032f);

      // point light 2
      shader_setUniformV3(diffuseShader, "u_pointLights[2].position",
                          pointLights.transforms.data[2].position);
      shader_setUniformV3(diffuseShader, "u_pointLights[2].ambient",
                          ambientLight);
      shader_setUniformV3(diffuseShader, "u_pointLights[2].diffuse",
                          (vector3_t){0.8f, 0.8f, 0.8f});
      shader_setUniformV3(diffuseShader, "u_pointLights[2].specular",
                          (vector3_t){1.0f, 1.0f, 1.0f});
      shader_setUniformFloat(diffuseShader, "u_pointLights[2].constant", 1.0f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[2].linear", 0.09f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[2].quadratic",
                             0.032f);

      // point light 3
      shader_setUniformV3(diffuseShader, "u_pointLights[3].position",
                          pointLights.transforms.data[3].position);
      shader_setUniformV3(diffuseShader, "u_pointLights[3].ambient",
                          ambientLight);
      shader_setUniformV3(diffuseShader, "u_pointLights[3].diffuse",
                          (vector3_t){0.8f, 0.8f, 0.8f});
      shader_setUniformV3(diffuseShader, "u_pointLights[3].specular",
                          (vector3_t){1.0f, 1.0f, 1.0f});
      shader_setUniformFloat(diffuseShader, "u_pointLights[3].constant", 1.0f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[3].linear", 0.09f);
      shader_setUniformFloat(diffuseShader, "u_pointLights[3].quadratic",
                             0.032f);

      // material
      shader_setUniformInt(diffuseShader, "u_material.diffuse", 0);
      shader_setUniformInt(diffuseShader, "u_material.specular", 1);
      shader_setUniformFloat(diffuseShader, "u_material.shininess", 32.0f);

      // cubes
      for (unsigned int i = 0; i < cubes.length; i++) {
        // projection matrix
        shader_setUniformM4(diffuseShader, "u_projectionMatrix", &projection);

        // view matrix
        shader_setUniformM4(diffuseShader, "u_viewMatrix",
                            &camera.transform.modelMatrix);

        // model
        cubes.transforms.data[i].modelMatrix =
            matrix4_translation(cubes.transforms.data[i].position);
        shader_setUniformM4(diffuseShader, "u_modelMatrix",
                            &cubes.transforms.data[i].modelMatrix);

        glBindVertexArray(cubes.meshes.VAOs.data[0]);
        glDrawElements(GL_TRIANGLES, MESH_CUBE_NUM_INDICES, GL_UNSIGNED_INT, 0);
      }

      for (int i = 0; i < 4; i++) {
        glUseProgram(unlitShader);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lampDiffuse);

        // projection matrix
        shader_setUniformM4(unlitShader, "u_projectionMatrix", &projection);

        // view matrix
        shader_setUniformM4(unlitShader, "u_viewMatrix",
                            &camera.transform.modelMatrix);

        // model matrix
        pointLights.transforms.data[i].modelMatrix = matrix4_identity();
        pointLights.transforms.data[i].modelMatrix =
            matrix4_translation(pointLights.transforms.data[i].position);

        shader_setUniformM4(unlitShader, "u_modelMatrix",
                            &pointLights.transforms.data[i].modelMatrix);

        // color
        shader_setUniformV3(unlitShader, "u_color", pointLights.colors.data[i]);

        glBindVertexArray(pointLights.meshes.VAOs.data[1]);
        glDrawElements(GL_TRIANGLES, MESH_CUBE_NUM_INDICES, GL_UNSIGNED_INT, 0);
      }

      glfwSwapBuffers(engine_window.glfwWindow);
      glfwPollEvents();
    }
  }

  // lights
  list_transform_t_free(&pointLights.transforms);
  list_vector3_t_free(&pointLights.colors);

  mesh_free(&pointLights.meshes);

  // cubes
  list_transform_t_free(&cubes.transforms);
  list_vector3_t_free(&cubes.colors);

  mesh_free(&cubes.meshes);

  glfwTerminate();
  return 0;
}
