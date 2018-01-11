// PV227, Lesson 5: SSAO, Depth of Field
#pragma once

#include "../../Framework/PV227.h"

using namespace std;
using namespace PV227;

//---------------------
//----    SCENE    ----
//---------------------

// Shader programs
ShaderProgram notexture_deferred_program;
ShaderProgram texture_deferred_program;
ShaderProgram display_texture_program;
ShaderProgram evaluate_lighting_program;
ShaderProgram evaluate_ssao_program;
ShaderProgram dof_program;

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

// Data of our objects (their position)
std::vector<ModelData_UBO> Models_ubo;
ModelData_UBO FloorModel_ubo;

// List of objects in the scene. Each object is defined by shaders it uses, its material,
// model matrix, texture (if used), and geometry.
struct SceneObject
{
	ShaderProgram *deferred_shading_program;	// Shader program that is used for deferred shading
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

// UBO with random samples in the unit hemisphere for SSAO
GLuint SSAO_Samples_UBO;
// Texture with random tangents directions (in view space) for SSAO
GLuint SSAO_RandomTangentVS_Texture;

// FBO for G-buffer for deferred shading
GLuint Gbuffer_FBO;					// Framebuffer object that is used when rendering into the G-buffer
GLuint Gbuffer_PositionWS_Texture;	// Texture with positions in world space
GLuint Gbuffer_PositionVS_Texture;	// Texture with positions in view space
GLuint Gbuffer_NormalWS_Texture;	// Texture with normals in world space
GLuint Gbuffer_NormalVS_Texture;	// Texture with normals in view space
GLuint Gbuffer_Albedo_Texture;		// Texture with albedo (i.e. diffuse color)
GLuint Gbuffer_Depth_Texture;		// Texture with depths for depth test

// FBO for evaluation of the SSAO
GLuint SSAO_Evaluation_FBO;					// Framebuffer object that is used to evaluate SSAO
GLuint SSAO_Occlusion_Texture;				// Texture with ambient occlusion

// FBO for blurring SSAO texture
GLuint SSAO_Blurring_FBO;					// Framebuffer object that is used to blur ambient occlusion texture
GLuint SSAO_BlurredOcclusion_Texture;		// Texture with blurred ambient occlusion

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

const int DISPLAY_POSITION = 0;
const int DISPLAY_NORMAL = 1;
const int DISPLAY_ALBEDO = 2;
const int DISPLAY_OCCLUSION = 3;
const int DISPLAY_FINAL_IMAGE = 4;

// Variables that are changed with GUI
float render_time_ms;
int what_to_display;
bool use_SSAO;
float SSAO_Radius;
float DoF_FocusDistance;

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
