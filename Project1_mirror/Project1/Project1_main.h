// PV227, Project 1
#pragma once

#include "../../Framework/PV227.h"

using namespace std;
using namespace PV227;

//---------------------
//----    SCENE    ----
//---------------------

// Shader programs
ShaderProgram notexture_program;
ShaderProgram texture_program;
ShaderProgram glass_tex_program;
ShaderProgram glass_notex_program;
ShaderProgram fullscreen_program;
ShaderProgram fullscreen_horizontal_program;
ShaderProgram fullscreen_vertical_program;

// Geometries we use in this lecture
Geometry geom_cube;
Geometry geom_sphere;
Geometry geom_torus;
Geometry geom_cylinder;
Geometry geom_capsule;
Geometry geom_teapot;
Geometry geom_floor;
Geometry geom_glass;
Geometry geom_fullscreen_quad;
// List of geometries which we choose for our scene
std::vector<std::pair<Geometry *, glm::mat4> > Geometries;

// A camera that allows us to look at the object from different views
SimpleCamera the_camera;

// Textures
GLuint wood_tex;
GLuint lenna_tex;
GLuint mask_tex;
// List of textures which we choose for our scene
std::vector<GLuint> Textures;

// Framebuffer objects
GLuint fbo1_color_texture;
GLuint fbo1_depth_texture;
GLuint fbo1;

float gauss_array[128];
float gauss_sum;

// Data of our materials
MaterialData_UBO RedMaterial_ubo;
MaterialData_UBO GreenMaterial_ubo;
MaterialData_UBO BlueMaterial_ubo;
MaterialData_UBO CyanMaterial_ubo;
MaterialData_UBO MagentaMaterial_ubo;
MaterialData_UBO YellowMaterial_ubo;
MaterialData_UBO WhiteMaterial_ubo;
MaterialData_UBO FloorMaterial_ubo;
MaterialData_UBO GlassMaterial_ubo;
// List of materials which we choose for our scene
std::vector<MaterialData_UBO *> Colors_ubo;

// Data of our objects (their position)
std::vector<ModelData_UBO> Models_ubo;
ModelData_UBO FloorModel_ubo;
ModelData_UBO GlassModel_ubo;
glm::mat4 glass_model_matrix;

// List of objects in the scene. Each object is defined by shaders it uses, its material,
// model matrix, texture (if used), and geometry.
struct SceneObject
{
	ShaderProgram *program;				// Shader program
	MaterialData_UBO *material_ubo;		// Material of the object
	ModelData_UBO *model_ubo;			// Model matrix with the position of the object
	GLuint texture;						// Texture of the object (or 0 if no texture is used)
	Geometry *geometry;					// Geomety of the object
};
std::vector<SceneObject> ObjectsInScene;

// UBO with lights in the scene
PhongLightsData_UBO PhongLights_ubo;

// Data of our camera - view matrix, projection matrix, etc.
CameraData_UBO CameraData_ubo;

// OpenGL query object to get render time of one frame
GLuint RenderTimeQuery;

// Functions that works with scene objects
float compute_gauss_value(int index);
void reload_shaders();
void init_scene();
void update_scene(int app_time_diff_ms);
void prepare_framebuffer(GLuint framebuffer);
void enable_draw_to_stencil();
void disable_draw_to_stencil();
void copy_color_buffer(GLuint from, GLuint to);
void render_glass(bool with_tex);
void render_stuff();
void fullscreen_render(ShaderProgram program, GLuint input_texture);
void render_scene();
void resize_fullscreen_textures();

//-------------------
//----    GUI    ----
//-------------------

// Main AntTweakBar object
TwBar *the_gui;

// Variables that are changed with GUI
float render_time_ms;
float glass_transparency;
int blur_kernel_size;
const int max_blur_kernel_size = 128;

// Callbacks from the GUI
void TW_CALL reload(void *);

// Functions that works with GUI
void init_gui();

//---------------------------
//----    APPLICATION    ----

// Current window size
int win_width = 640;
int win_height = 480;

// Current time of the application in milliseconds, for animations
int app_time_ms = 0;
int last_glut_time = 0;
