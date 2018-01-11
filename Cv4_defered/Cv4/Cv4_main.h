// PV227, Lesson 4: Deferred shading
#pragma once

#include "../../Framework/PV227.h"

using namespace std;
using namespace PV227;

//---------------------
//----    SCENE    ----
//---------------------

// Shader programs
ShaderProgram notexture_forward_program;
ShaderProgram notexture_deferred_program;
ShaderProgram texture_forward_program;
ShaderProgram texture_deferred_program;
ShaderProgram display_texture_program;
ShaderProgram evaluate_quad_program;
ShaderProgram evaluate_sphere_program;

// Shader program maps and map keys
ShaderProgramMap notexture_program_map;
ShaderProgramMap texture_program_map;
const int FORWARD_SHADING_SHADERS = 0;
const int DEFERRED_SHADING_SHADERS = 1;

// Geometries we use in this lecture
Geometry geom_cube;
Geometry geom_sphere;
Geometry geom_torus;
Geometry geom_cylinder;
Geometry geom_capsule;
Geometry geom_teapot;
Geometry geom_fullscreen_quad;
// List of geometries which we choose for our scene
std::vector<std::pair<Geometry *, glm::mat4> > Geometries;

// A camera that allows us to look at the object from different views
SimpleCamera the_camera;

// Textures
GLuint wood_tex;
GLuint lenna_tex;
// List of textures which we choose for our scene
std::vector<GLuint> Textures;

// Data of our materials
MaterialData_UBO RedMaterial_ubo;
MaterialData_UBO GreenMaterial_ubo;
MaterialData_UBO BlueMaterial_ubo;
MaterialData_UBO CyanMaterial_ubo;
MaterialData_UBO MagentaMaterial_ubo;
MaterialData_UBO YellowMaterial_ubo;
MaterialData_UBO WhiteMaterial_ubo;
MaterialData_UBO FloorMaterial_ubo;
// List of materials which we choose for our scene
std::vector<MaterialData_UBO *> Colors_ubo;

// Data of the our objects (their position)
std::vector<ModelData_UBO> Models_ubo;
ModelData_UBO FloorModel_ubo;

// List of objects in the scene. Each object is defined by shaders it uses, its material,
// model matrix, texture (if used), and geometry.
struct SceneObject
{
	ShaderProgramMap *programs;			// Map of shader programs to use
	Geometry *geometry;					// Geomety of the object
	ModelData_UBO *model_ubo;			// Model matrix with the position of the object
	MaterialData_UBO *material_ubo;		// Material of the object
	GLuint texture;						// Texture of the object (or 0 if no texture is used)
};
std::vector<SceneObject> ObjectsInScene;

// Data of all our lights - positions, colors, etc.
#define LIGHTS_MAX_COUNT			2000
#define LIGHTS_STORAGE				GL_SHADER_STORAGE_BUFFER		// Choose between GL_UNIFORM_BUFFER and GL_SHADER_STORAGE_BUFFER
std::vector<PhongLight> AllLights;
// UBO with a subset of the lights (the number of lights is chosen from GUI)
PhongLightsData_UBO PhongLights_ubo;

// Data of our camera - view matrix, projection matrix, etc.
CameraData_UBO CameraData_ubo;

// OpenGL query object to get render time of one frame
GLuint RenderTimeQuery;

// FBO for G-buffer for deferred shading
GLuint GbufferFBO;					// Framebuffer object that is used when rendering into the G-buffer
GLuint GbufferPositionTexture;		// Texture with positions
GLuint GbufferNormalTexture;		// Texture with normals
GLuint GbufferAlbedoTexture;		// Texture with albedo (i.e. diffuse color)
GLuint GbufferDepthTexture;			// Texture with depths for depth test

// FBO for evaluation of the G-buffer
GLuint EvaluateGbufferFBO;				// Framebuffer object that is used when evaluating the G-buffer
GLuint EvaluateGbufferColorTexture;		// Texture with final color

// Functions that works with scene objects
void reload_shaders();
void init_scene();
void update_scene(int app_time_diff_ms);
void render_scene();
void resize_fullscreen_textures();

//-------------------
//----    GUI    ----
//-------------------

// Main AntTweakBar object
TwBar *the_gui;

const int USE_FORWARD_SHADING = 0;
const int USE_DEFERRED_SHADING_DISPLAY_POSITION = 1;
const int USE_DEFERRED_SHADING_DISPLAY_NORMAL = 2;
const int USE_DEFERRED_SHADING_DISPLAY_ALBEDO = 3;
const int USE_DEFERRED_SHADING_EVALUATE_QUAD = 4;
const int USE_DEFERRED_SHADING_EVALUATE_SPHERE = 5;

// Variables that are changed with GUI
int lights_count;
float render_time_ms;		// Read-only
int current_shading_technique;

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
