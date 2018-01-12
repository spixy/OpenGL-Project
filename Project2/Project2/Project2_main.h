// PV227, Project 2
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
ShaderProgram display_texture_program;
ShaderProgram evaluate_lighting_program;
ShaderProgram evaluate_ssao_program;
ShaderProgram ignore_ssao_program;
ShaderProgram gen_shadow_program;
ShaderProgram display_shadow_texture_program;
ShaderProgram expand_program;

// Geometries we use in this lecture
Geometry geom_cube;
Geometry geom_sphere;
Geometry geom_torus;
Geometry geom_cylinder;
Geometry geom_capsule;
Geometry geom_teapot;
Geometry geom_glass;
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

// Shadow texture
GLuint ShadowFBO;			// Framebuffer object that is used when rendering into the shadow texture
GLuint ShadowTexture;		// Shadow texture
glm::mat4 ShadowMatrix;

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
MaterialData_UBO BlackMaterial_ubo;
// List of materials which we choose for our scene
std::vector<MaterialData_UBO *> Colors_ubo;

// Data of our objects (their position)
std::vector<ModelData_UBO> Models_ubo;
ModelData_UBO FloorModel_ubo;
ModelData_UBO GlassModel_ubo;

// UBO with random samples in the unit hemisphere for SSAO
GLuint SSAO_Samples_UBO;
// Texture with random tangents directions (in view space) for SSAO
GLuint SSAO_RandomTangentVS_Texture;

// List of objects in the scene. Each object is defined by shaders it uses, its material,
// model matrix, texture (if used), and geometry.
struct SceneObject
{
	ShaderProgram *shading_program;		// Shader program
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

// Data of the camera that is used when rendering from the position of the light
glm::mat4 LightCameraProjection;			// Projection matrix of the camera
glm::mat4 LightCameraView;					// View matrix of the camera
CameraData_UBO LightCameraData_ubo;			// UBO with the data

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
GLuint SSAO_Depth_Texture;

// OpenGL query object to get render time of one frame
GLuint RenderTimeQuery;

// Functions that works with scene objects
void reload_shaders();
void init_scene();
void update_scene(int app_time_diff_ms);
void render_scene();
void resize_fullscreen_textures();
void render_glass(bool blended);
void render_stuff_once(bool gen_shadows);
void enable_draw_to_stencil();
void disable_draw_to_stencil();
void render_cel_stuff();
void evaluate_ssao();
void render_ssao_final(bool shadow_toon_rendering);
void display_shadow_tex();

//-------------------
//----    GUI    ----
//-------------------

// Main AntTweakBar object
TwBar *the_gui;

// Variables that are changed with GUI
float light_pos;
float render_time_ms;

// Callbacks from the GUI
void TW_CALL reload(void *);

// Functions that works with GUI
void init_gui();

//---------------------------
//----    APPLICATION    ----

// Current window size
int win_width = 640*2;
int win_height = 480*2;

// Current time of the application in milliseconds, for animations
int app_time_ms = 0;
int last_glut_time = 0;
int what_to_display = 0;

const float SSAO_Radius = 0.4f;
const int ShadowTexSize = 1024;