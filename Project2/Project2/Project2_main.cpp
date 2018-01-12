#include "Project2_main.h"

#include "glm/gtx/color_space.hpp"

//---------------------
//----    SCENE    ----
//---------------------

// Geometry of the glass: one quad from four vertices
const int glass_quad_vertices_count = 4;
const float glass_size = 10.0f;		// Half of the glass size
const float glass_quad_vertices[glass_quad_vertices_count * 8] =
{
//       Position                             Normal                Tex Coord
	-glass_size, 0.0f, -glass_size,		 0.0f, 1.0f, 0.0f,		0.0f, 1.0f,
	-glass_size, 0.0f, glass_size,		 0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
	 glass_size, 0.0f, -glass_size,		 0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
	 glass_size, 0.0f, glass_size,		 0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
};

/// Reloads all shaders
void reload_shaders()
{
	// Program for objects without textures
	notexture_program.Init();
	notexture_program.AddVertexShader("Shaders/notexture_vertex.glsl");
	notexture_program.AddFragmentShader("Shaders/notexture_fragment.glsl");
	notexture_program.Link();

	// Program for objects with textures
	texture_program.Init();
	texture_program.AddVertexShader("Shaders/texture_vertex.glsl");
	texture_program.AddFragmentShader("Shaders/texture_fragment.glsl");
	texture_program.Link();

	// Program for displaying a texture on the screen
	display_texture_program.Init();
	display_texture_program.AddVertexShader("Shaders/fullscreen_quad_vertex.glsl");
	display_texture_program.AddFragmentShader("Shaders/display_texture_fragment.glsl");
	display_texture_program.Link();

	// Program for evaluating the G-buffer using a screen quad
	evaluate_lighting_program.Init();
	evaluate_lighting_program.AddVertexShader("Shaders/fullscreen_quad_vertex.glsl");
	evaluate_lighting_program.AddFragmentShader("Shaders/evaluate_lighting_fragment.glsl");
	evaluate_lighting_program.Link();

	evaluate_ssao_program.Init();
	evaluate_ssao_program.AddVertexShader("Shaders/fullscreen_quad_vertex.glsl");
	evaluate_ssao_program.AddFragmentShader("Shaders/evaluate_ssao_fragment.glsl");
	evaluate_ssao_program.Link();

	// Program for displaying shadow maps
	display_shadow_texture_program.Init();
	display_shadow_texture_program.AddVertexShader("Shaders/fullscreen_quad_vertex.glsl");
	display_shadow_texture_program.AddFragmentShader("Shaders/display_shadow_texture_fragment.glsl");
	display_shadow_texture_program.Link();

	// Program for generating shadow maps
	gen_shadow_program.Init();
	gen_shadow_program.AddVertexShader("Shaders/nolit_vertex.glsl");
	gen_shadow_program.AddFragmentShader("Shaders/nothing_fragment.glsl");
	gen_shadow_program.Link();

	// cel shading
	expand_program.Init();
	expand_program.AddVertexShader("Shaders/expand_vertex.glsl");
	expand_program.AddFragmentShader("Shaders/nolit_fragment.glsl");
	expand_program.Link();

	cout << "Shaders are reloaded" << endl;
}

/// Initializes all objects of our scene
void init_scene()
{
	//----------------------------------------------
	//--  Display some information about the OpenGL

	PrintOpenGLInfo();

	//----------------------------------------------
	//--  Load shaders

	reload_shaders();
	
	//----------------------------------------------
	//--  Prepare the lights
	
	PhongLights_ubo.Init();
	PhongLights_ubo.SetGlobalAmbient(glm::vec3(0.0f));
	PhongLights_ubo.PhongLights.push_back(PhongLight::CreateDirectionalLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.7f), glm::vec3(0.3f), glm::vec3(0.0f)));
	PhongLights_ubo.UpdateOpenGLData();

	//----------------------------------------------
	//--  Prepare materials

	RedMaterial_ubo.Init();
	GreenMaterial_ubo.Init();
	BlueMaterial_ubo.Init();
	CyanMaterial_ubo.Init();
	MagentaMaterial_ubo.Init();
	YellowMaterial_ubo.Init();
	WhiteMaterial_ubo.Init();
	FloorMaterial_ubo.Init();
	GlassMaterial_ubo.Init();
	BlackMaterial_ubo.Init();
	RedMaterial_ubo.SetMaterial		(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 0.0f, 0.0f), true, 200.0f, 1.0f));
	GreenMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(0.0f, 1.0f, 0.0f), true, 200.0f, 1.0f));
	BlueMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(0.0f, 0.0f, 1.0f), true, 200.0f, 1.0f));
	CyanMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(0.0f, 1.0f, 1.0f), true, 200.0f, 1.0f));
	MagentaMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 0.0f, 1.0f), true, 200.0f, 1.0f));
	YellowMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 1.0f, 0.0f), true, 200.0f, 1.0f));
	WhiteMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 1.0f, 1.0f), true, 200.0f, 1.0f));
	FloorMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(0.7f, 0.7f, 0.7f), true, 200.0f, 1.0f));
	GlassMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 1.0f, 1.0f), false, 0.0f, 0.5f));
	BlackMaterial_ubo.SetMaterial(PhongMaterial::CreateBasicMaterial(glm::vec3(0.0f), false, 0.0f, 1.0f));
	RedMaterial_ubo.UpdateOpenGLData();
	GreenMaterial_ubo.UpdateOpenGLData();
	BlueMaterial_ubo.UpdateOpenGLData();
	CyanMaterial_ubo.UpdateOpenGLData();
	MagentaMaterial_ubo.UpdateOpenGLData();
	YellowMaterial_ubo.UpdateOpenGLData();
	WhiteMaterial_ubo.UpdateOpenGLData();
	FloorMaterial_ubo.UpdateOpenGLData();
	GlassMaterial_ubo.UpdateOpenGLData();
	BlackMaterial_ubo.UpdateOpenGLData();

	// Add the materials into our list of materials (we do not add the material for the floor, only the basic colors)
	Colors_ubo.push_back(&RedMaterial_ubo);
	Colors_ubo.push_back(&GreenMaterial_ubo);
	Colors_ubo.push_back(&BlueMaterial_ubo);
	Colors_ubo.push_back(&CyanMaterial_ubo);
	Colors_ubo.push_back(&MagentaMaterial_ubo);
	Colors_ubo.push_back(&YellowMaterial_ubo);
	Colors_ubo.push_back(&WhiteMaterial_ubo);

	//----------------------------------------------
	//--  Prepare all cameras

	the_camera.SetCenter(glm::vec3(0.0f, 1.0f, 0.0f));
	the_camera.SetCamera(1.25f, -0.5f, 40.0f);
	CameraData_ubo.Init();

	LightCameraProjection = glm::perspective(glm::radians(80.0f), 1.0f, 2.0f, 30.0f);
	LightCameraView = glm::mat4(1.0f);
	LightCameraData_ubo.Init();
	LightCameraData_ubo.SetProjection(LightCameraProjection);
	LightCameraData_ubo.SetCamera(LightCameraView);
	LightCameraData_ubo.UpdateOpenGLData();

	//----------------------------------------------
	//--  Prepare geometries

	geom_cube = CreateCube();
	geom_sphere = CreateSphere();
	geom_torus = CreateTorus();
	geom_cylinder = CreateCylinder();
	geom_capsule = CreateCapsule();
	geom_teapot = CreateTeapot();
	geom_fullscreen_quad = CreateFullscreenQuad();

	// Add the geometries into our list of geometries. Also add matrices that describe
	// how to transform the objects so that they "sit" on xz plane.
	Geometries.push_back(make_pair(&geom_cube,
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f))));
	Geometries.push_back(make_pair(&geom_sphere,
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f))));
	Geometries.push_back(make_pair(&geom_torus,
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f))));
	Geometries.push_back(make_pair(&geom_cylinder,
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f))));
	Geometries.push_back(make_pair(&geom_capsule,
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f))));
	Geometries.push_back(make_pair(&geom_teapot,
		glm::mat4(1.0f)));

	//----------------------------------------------
	//--  Prepare textures

	wood_tex = CreateAndLoadTexture2D(L"../../textures/wood.jpg");
	glBindTexture(GL_TEXTURE_2D, wood_tex);
	SetTexture2DParameters(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	lenna_tex = CreateAndLoadTexture2D(L"../../textures/Lenna.jpg");
	glBindTexture(GL_TEXTURE_2D, lenna_tex);
	SetTexture2DParameters(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// Add the textures into our list of textures
	Textures.push_back(wood_tex);
	Textures.push_back(lenna_tex);

	// Create the shadow texture, allocate its memory, and set its basic parameters
	glGenTextures(1, &ShadowTexture);
	glBindTexture(GL_TEXTURE_2D, ShadowTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, ShadowTexSize, ShadowTexSize, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
	SetTexture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	//----------------------------------------------
	//--  Prepare objects in the scene

	srand(12345);
	float x_start = -18.0f;
	float z_start = -18.0f;
	float x_spacing = 3.5f;
	float z_spacing = 3.5f;
	int x_grid_size = 11;
	int z_grid_size = 11;

	Models_ubo.resize(x_grid_size * z_grid_size);

	for (int x = 0; x < x_grid_size; x++)
	for (int z = 0; z < z_grid_size; z++)
	{
		// Choose the geometry randomly
		int geometry = rand() % int(Geometries.size());

		// Rotate the object randomly around the y axis
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), float(rand()) / float(RAND_MAX) * float(M_PI) * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));

		// Place the object at the proper place in the grid
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(x_start + float(x) * x_spacing, 0.0f, z_start + float(z) * z_spacing));

		// Choose the material randomly, choose randomly between the colors and the textures
		int material = rand() % int(Colors_ubo.size() + Textures.size());

		// Compute the model matrix
		Models_ubo[z*x_grid_size + x].Init();
		Models_ubo[z*x_grid_size + x].SetMatrix(translation * rotation * Geometries[geometry].second);
		Models_ubo[z*x_grid_size + x].UpdateOpenGLData();

		// Create the scene object and add it into the list
		SceneObject scene_object;
		scene_object.geometry = Geometries[geometry].first;
		scene_object.model_ubo = &Models_ubo[z*x_grid_size + x];
		if (material < int(Colors_ubo.size()))
		{
			// Object with a color without textures
			scene_object.shading_program = &notexture_program;
			scene_object.material_ubo = Colors_ubo[material];
			scene_object.texture = 0;
		}
		else
		{
			// Object with a texture
			scene_object.shading_program = &texture_program;
			scene_object.material_ubo = &WhiteMaterial_ubo;
			scene_object.texture = Textures[material - int(Colors_ubo.size())];
		}
		ObjectsInScene.push_back(scene_object);
	}

	// Prepare the floor model matrix. Its size corresponds to the size of the scene
	FloorModel_ubo.Init();
	FloorModel_ubo.SetMatrix(
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.1f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(x_spacing * float(x_grid_size) / 2.0f + 5.0f, 0.1f, z_spacing * float(z_grid_size) / 2.0f + 5.0f)));
	FloorModel_ubo.UpdateOpenGLData();

	// Add the floor into the list of scene objects
	SceneObject floor_scene_object;
	floor_scene_object.geometry = &geom_cube;
	floor_scene_object.model_ubo = &FloorModel_ubo;
	floor_scene_object.shading_program = &notexture_program;
	floor_scene_object.material_ubo = &FloorMaterial_ubo;
	floor_scene_object.texture = 0;
	ObjectsInScene.push_back(floor_scene_object);

	//----------------------------------------------
	//--  Compute the random positions of samples for SSAO

	std::vector<glm::vec4> SSAOSamples(64);
	for (size_t i = 0; i < SSAOSamples.size(); i++)
	{
		// Create a uniform point on a hemisphere (unit sphere, cut by xy plane, hemisphere in +z direction)
		float alpha = float(rand()) / float(RAND_MAX) * 2.0f * float(M_PI);
		float beta = asinf(float(rand()) / float(RAND_MAX));
		glm::vec3 point_on_hemisphere = glm::vec3(cosf(alpha) * cosf(beta), sinf(alpha) * cosf(beta), sinf(beta));

		// Place the point on the hemisphere inside the hemisphere, so that the points in the hemisphere were uniformly
		// distributed in the hemisphere. This is true when the distance of the points from the center of the hemisphere
		// is the cube root of a uniformly distributed random number from interval [0,1].
		float radius = powf(float(rand()) / float(RAND_MAX), 1.0f / 3.0f);
		glm::vec3 point_in_hemisphere = point_on_hemisphere * radius;

		SSAOSamples[i] = glm::vec4(point_in_hemisphere, 0.0f);
	}
	// Create a UBO for these random positions
	glGenBuffers(1, &SSAO_Samples_UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, SSAO_Samples_UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 4 * SSAOSamples.size(), SSAOSamples.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//----------------------------------------------
	//--  Compute random tangent directions for SSAO

	std::vector<glm::vec3> SSAORandomTangent(16);
	for (int i = 0; i < 16; i++)
	{
		// The tangent is a random direction in xy plane
		float angle = float(rand()) / float(RAND_MAX) * 2.0f * float(M_PI);
		SSAORandomTangent[i] = glm::vec3(sinf(angle), cosf(angle), 0.0f);
	}
	// Create a 4x4 texture for these random directions
	glGenTextures(1, &SSAO_RandomTangentVS_Texture);
	glBindTexture(GL_TEXTURE_2D, SSAO_RandomTangentVS_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, SSAORandomTangent.data());
	SetTexture2DParameters(GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);


	//----------------------------------------------
	//--  Prepare framebuffers

	glGenTextures(1, &Gbuffer_PositionWS_Texture);
	glGenTextures(1, &Gbuffer_PositionVS_Texture);
	glGenTextures(1, &Gbuffer_NormalWS_Texture);
	glGenTextures(1, &Gbuffer_NormalVS_Texture);
	glGenTextures(1, &Gbuffer_Albedo_Texture);
	glGenTextures(1, &Gbuffer_Depth_Texture);
	glGenTextures(1, &SSAO_Occlusion_Texture);
	glBindTexture(GL_TEXTURE_2D, Gbuffer_PositionWS_Texture);
	SetTexture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, Gbuffer_PositionVS_Texture);
	SetTexture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, Gbuffer_NormalWS_Texture);
	SetTexture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, Gbuffer_NormalVS_Texture);
	SetTexture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, Gbuffer_Albedo_Texture);
	SetTexture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, Gbuffer_Depth_Texture);
	SetTexture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, SSAO_Occlusion_Texture);
	SetTexture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Allocate the memory of the textures
	resize_fullscreen_textures();

	// Create the G-buffer framebuffer
	glGenFramebuffers(1, &Gbuffer_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, Gbuffer_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Gbuffer_PositionWS_Texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, Gbuffer_PositionVS_Texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, Gbuffer_NormalWS_Texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, Gbuffer_NormalVS_Texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, Gbuffer_Albedo_Texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, Gbuffer_Depth_Texture, 0);
	glDrawBuffers(5, DrawBuffersConstants);
	CheckFramebufferStatus("Gbuffer");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Create the SSAO_Evaluation_FBO framebuffer
	glGenFramebuffers(1, &SSAO_Evaluation_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAO_Evaluation_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSAO_Occlusion_Texture, 0);
	glDrawBuffers(1, DrawBuffersConstants);
	CheckFramebufferStatus("SSAO_Evaluation");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Create the framebuffer for rendering into the shadow texture, and set the shadow texture to it
	glGenFramebuffers(1, &ShadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ShadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ShadowTexture, 0);
	CheckFramebufferStatus("ShadowFBO");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//------------------------
	//--  Create the glass  --

	// Prepare the glass model matrix.
	glm::mat4 glass_model_matrix =
		glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 10.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
	GlassModel_ubo.Init();
	GlassModel_ubo.SetMatrix(glass_model_matrix);
	GlassModel_ubo.UpdateOpenGLData();

	// Create glass geometry
	geom_glass.VertexBuffers.resize(1, 0);
	glGenBuffers(1, &geom_glass.VertexBuffers[0]);
	glBindBuffer(GL_ARRAY_BUFFER, geom_glass.VertexBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, glass_quad_vertices_count * sizeof(float) * 8, glass_quad_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create a vertex array object for glass geometry
	glGenVertexArrays(1, &geom_glass.VAO);

	// Set the parameters of the geometry
	glBindVertexArray(geom_glass.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, geom_glass.VertexBuffers[0]);
	glEnableVertexAttribArray(DEFAULT_POSITION_LOC);
	glVertexAttribPointer(DEFAULT_POSITION_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	glEnableVertexAttribArray(DEFAULT_NORMAL_LOC);
	glVertexAttribPointer(DEFAULT_NORMAL_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *)(sizeof(float) * 3));
	glEnableVertexAttribArray(DEFAULT_TEX_COORD_LOC);
	glVertexAttribPointer(DEFAULT_TEX_COORD_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *)(sizeof(float) * 6));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Set the Mode and the number of vertices to draw
	geom_glass.Mode = GL_TRIANGLE_STRIP;
	geom_glass.DrawArraysCount = 4;
	geom_glass.DrawElementsCount = 0;

	//----------------------------------------------
	//--  Miscellaneous

	// Data of the camera that is used when rendering from the light
	LightCameraView = glm::lookAt(
		glm::vec3(PhongLights_ubo.PhongLights[0].position),
		glm::vec3(PhongLights_ubo.PhongLights[0].position) + PhongLights_ubo.PhongLights[0].spot_direction,
		glm::vec3(0.0f, 1.0f, 0.0f));
	LightCameraData_ubo.SetCamera(LightCameraView);
	LightCameraData_ubo.UpdateOpenGLData();

	ShadowMatrix =
		glm::translate(glm::mat4(1.0f), glm::vec3(0.5f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)) *
		LightCameraProjection *
		LightCameraView;

	// Create the query object
	glGenQueries(1, &RenderTimeQuery);
}

/// Updates the scene: performs animations, updates the data of the buffers, etc.
void update_scene(int app_time_diff_ms)
{
	// Data of the main camera
	CameraData_ubo.SetProjection(glm::perspective(glm::radians(45.0f), float(win_width) / float(win_height), 0.5f, 1000.0f));
	CameraData_ubo.SetCamera(the_camera);
	CameraData_ubo.UpdateOpenGLData();

	glm::vec3 light_position = glm::vec3(
		cosf(light_pos / 6.0f) * sinf(light_pos),
		sinf(light_pos / 6.0f),
		cosf(light_pos / 6.0f) * cosf(light_pos));

	// Data of the lights
	PhongLights_ubo.PhongLights.clear();
	PhongLights_ubo.PhongLights.push_back(PhongLight::CreateDirectionalLight(light_position, glm::vec3(0.7f), glm::vec3(0.3f), glm::vec3(0.0f)));
	PhongLights_ubo.UpdateOpenGLData();

	LightCameraView = glm::lookAt(
		light_position,
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	LightCameraData_ubo.SetCamera(LightCameraView);
	LightCameraData_ubo.UpdateOpenGLData();

	ShadowMatrix =
		glm::translate(glm::mat4(1.0f), glm::vec3(0.5f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)) *
		LightCameraProjection *
		LightCameraView;
}

void render_glass()
{
	if (notexture_program.IsValid())
	{
		// Set up depth test and blending
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Set the data of the material
		GlassMaterial_ubo.BindBuffer(DEFAULT_MATERIAL_BINDING);
		// Set the data of the object
		GlassModel_ubo.BindBuffer(DEFAULT_OBJECT_BINDING);

		// Use the proper program and set its uniform variables
		notexture_program.Use();

		// Render the glass quad
		geom_glass.BindVAO();
		geom_glass.Draw();

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}
}

void enable_draw_to_stencil()
{
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glStencilFunc(GL_NEVER, 1, 0xFF);
	glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);
}

void disable_draw_to_stencil()
{
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glStencilMask(0x00);
}

void render_cel_stuff()
{
	if (!expand_program.IsValid())
	{
		return;
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// Render all objects in the scene
	for (auto iter = ObjectsInScene.begin(); iter != ObjectsInScene.end(); ++iter)
	{
		expand_program.Use();
		BlackMaterial_ubo.BindBuffer(DEFAULT_MATERIAL_BINDING);

		// Set the data of the object
		if (iter->model_ubo)
			iter->model_ubo->BindBuffer(DEFAULT_OBJECT_BINDING);

		// Set the texture
		glActiveTexture(GL_TEXTURE1);	glBindTexture(GL_TEXTURE_2D, iter->texture);

		// Render the object
		if (iter->geometry)
		{
			iter->geometry->BindVAO();
			iter->geometry->Draw();
		}
	}

	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
}

void render_stuff_once(bool gen_shadows)
{
	if (gen_shadows)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}
	else
	{
		glActiveTexture(GL_TEXTURE0);	glBindTexture(GL_TEXTURE_2D, ShadowTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	}

	// Render all objects in the scene
	for (auto iter = ObjectsInScene.begin(); iter != ObjectsInScene.end(); ++iter)
	{
		if (gen_shadows)
		{
			if (gen_shadow_program.IsValid())
				gen_shadow_program.Use();
			else
				continue;
		}
		else
		{
			if (iter->shading_program && iter->shading_program->IsValid())
			{
				iter->shading_program->Use();
				iter->shading_program->UniformMatrix4fv("shadow_matrix", 1, GL_FALSE, glm::value_ptr(ShadowMatrix));
			}
			else
			{
				continue;
			}
		}

		// Set the data of the material
		if (iter->material_ubo)
			iter->material_ubo->BindBuffer(DEFAULT_MATERIAL_BINDING);
		// Set the data of the object
		if (iter->model_ubo)
			iter->model_ubo->BindBuffer(DEFAULT_OBJECT_BINDING);

		// Set the texture
		glActiveTexture(GL_TEXTURE1);	glBindTexture(GL_TEXTURE_2D, iter->texture);

		// Render the object
		if (iter->geometry)
		{
			iter->geometry->BindVAO();
			iter->geometry->Draw();
		}
	}

	if (gen_shadows)
	{
		glDisable(GL_CULL_FACE);
	}
}

void evaluate_ssao()
{
	if (evaluate_ssao_program.IsValid())
	{
		// Bind the proper framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, SSAO_Evaluation_FBO);
		glViewport(0, 0, win_width, win_height);

		// Bind all textures that we need
		glActiveTexture(GL_TEXTURE0);	glBindTexture(GL_TEXTURE_2D, Gbuffer_PositionVS_Texture);
		glActiveTexture(GL_TEXTURE1);	glBindTexture(GL_TEXTURE_2D, Gbuffer_NormalVS_Texture);
		glActiveTexture(GL_TEXTURE2);	glBindTexture(GL_TEXTURE_2D, SSAO_RandomTangentVS_Texture);

		// Bind all UBOs that we need
		CameraData_ubo.BindBuffer(DEFAULT_CAMERA_BINDING);
		// Although the binding point 1 is usually used by the data of the lights, we do not need the lights here, so we may use this binding point
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, SSAO_Samples_UBO);

		// Use the proper program and set its uniform variables
		evaluate_ssao_program.Use();
		evaluate_ssao_program.Uniform1f("SSAO_Radius", SSAO_Radius);

		// Render the fullscreen quad to evaluate every pixel
		geom_fullscreen_quad.BindVAO();
		geom_fullscreen_quad.Draw();
	}
}

void render_ssao_final(bool toon_rendering)
{
	if (evaluate_lighting_program.IsValid())
	{
		// Bind all textures that we need
		glActiveTexture(GL_TEXTURE0);	glBindTexture(GL_TEXTURE_2D, Gbuffer_PositionWS_Texture);
		glActiveTexture(GL_TEXTURE1);	glBindTexture(GL_TEXTURE_2D, Gbuffer_NormalWS_Texture);
		glActiveTexture(GL_TEXTURE2);	glBindTexture(GL_TEXTURE_2D, Gbuffer_Albedo_Texture);
		glActiveTexture(GL_TEXTURE3);	glBindTexture(GL_TEXTURE_2D, SSAO_Occlusion_Texture);

		// Bind all UBOs that we need
		CameraData_ubo.BindBuffer(DEFAULT_CAMERA_BINDING);
		PhongLights_ubo.BindBuffer(DEFAULT_LIGHTS_BINDING);
		WhiteMaterial_ubo.BindBuffer(DEFAULT_MATERIAL_BINDING);		// Bind the data with the specular color and shininess (all materials have the same)

		evaluate_lighting_program.Use();
		evaluate_lighting_program.Uniform1i("use_toon", toon_rendering ? 1 : 0);

		// Render the fullscreen quad to evaluate every pixel
		geom_fullscreen_quad.BindVAO();
		geom_fullscreen_quad.Draw();
	}
}

/// Renders the whole frame
void render_scene()
{
	// Start measuring the elapsed time
	glBeginQuery(GL_TIME_ELAPSED, RenderTimeQuery);

	//--------------------------------------
	//--  Render into the shadow texture  --

	//glBindFramebuffer(GL_FRAMEBUFFER, ShadowFBO);
	//glViewport(0, 0, ShadowTexSize, ShadowTexSize);

	//// Clear the framebuffer, clear only the depth (there is no color)
	//glClearDepth(1.0);
	//glClear(GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_DEPTH_TEST);

	//LightCameraData_ubo.BindBuffer(DEFAULT_CAMERA_BINDING);

	//render_stuff_once(true);

	//----------------------------------------------
	//--  First, render the scene into the G-buffer

	glBindFramebuffer(GL_FRAMEBUFFER, Gbuffer_FBO);
	glViewport(0, 0, win_width, win_height);

	// Clear the G-buffer, i.e., clear all textures to (0,0,0,0) and depth to 1.0
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	CameraData_ubo.BindBuffer(DEFAULT_CAMERA_BINDING);
	PhongLights_ubo.BindBuffer(DEFAULT_LIGHTS_BINDING);

	// sklo do stencil bufferu
	glEnable(GL_STENCIL_TEST);
	enable_draw_to_stencil();
	render_glass();
	disable_draw_to_stencil();

	// mimo glass
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	render_cel_stuff();
	glDisable(GL_STENCIL_TEST);

	render_stuff_once(false);
	
	glDisable(GL_DEPTH_TEST);

	//----------------------------------------------
	//--  Evaluate the SSAO (when necessary)

	// niekde ma byt glass
	render_glass();

	evaluate_ssao();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, win_width, win_height);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// sklo do stencil bufferu
	glEnable(GL_STENCIL_TEST);
	enable_draw_to_stencil();
	render_glass();
	disable_draw_to_stencil();

	// mimo glass
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	render_ssao_final(true);

	// vnutri glass
	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
	render_ssao_final(false);

	glDisable(GL_STENCIL_TEST);

	//----------------------------------------------

	// Reset the VAO and the program
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	// Stop measuring the elapsed time
	glEndQuery(GL_TIME_ELAPSED);

	// Evaluate the query
	glFinish();					// Wait for OpenGL, don't forget OpenGL is asynchronous
	GLuint64 render_time;
	glGetQueryObjectui64v(RenderTimeQuery, GL_QUERY_RESULT, &render_time);
	render_time_ms = float(render_time) * 1e-6f;
}

void resize_fullscreen_textures()
{
	// Resize G-buffer textures to match the window
	glBindTexture(GL_TEXTURE_2D, Gbuffer_PositionWS_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, win_width, win_height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, Gbuffer_PositionVS_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, win_width, win_height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, Gbuffer_NormalWS_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, win_width, win_height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, Gbuffer_NormalVS_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, win_width, win_height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, Gbuffer_Albedo_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, win_width, win_height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, Gbuffer_Depth_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, win_width, win_height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
	glBindTexture(GL_TEXTURE_2D, SSAO_Occlusion_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, win_width, win_height, 0, GL_RED, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//-------------------
//----    GUI    ----
//-------------------

/// Callback when user clicks on Reload button
void TW_CALL reload(void *)
{
	reload_shaders();
}

void init_gui()
{
	// Initial values
	light_pos = 4.0f;

	// Initialize AntTweakBar
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(win_width, win_height);
	TwGLUTModifiersFunc(glutGetModifiers);

	// Initialize GUI
	the_gui = TwNewBar("Parameters");
	TwAddButton(the_gui, "Reload", reload, nullptr, nullptr);
	TwAddVarRW(the_gui, "Light direction", TW_TYPE_FLOAT, &light_pos, "min=1 max=8 step=0.03");

	TwAddVarRO(the_gui, "Render time (ms)", TW_TYPE_FLOAT, &render_time_ms, nullptr);

	TwEnumVal displays[6] =
	{
		{ 0, "Final Image" },
		{ 1, "Position" },
		{ 2, "Normal" },
		{ 3, "Albedo" },
		{ 4, "Occlusion" },
		{ 5, "ShadowTex" }
	};

	TwType displayType = TwDefineEnum("WhatToDisplay", displays, 6);
	TwAddVarRW(the_gui, "Display", displayType, &what_to_display, nullptr);
}

//---------------------------
//----    APPLICATION    ----
//---------------------------

/// GLUT callback - when the user presses a key. See glutKeyboardFunc for more info
void on_keyboard_func(unsigned char key, int x, int y)
{
	// Inform AntTweakBar
	if (TwEventKeyboardGLUT(key, x, y))
		return;		// Already handled by AntTweakBar

	switch (key)
	{
		case 27:		// Escape key
			exit(0);
		default: ;
	}
}

/// GLUT callback - when the user presses a special key (like F1 key). See glutSpecialFunc for more info
void on_special_func(int key, int x, int y)
{
	// Inform AntTweakBar
	if (TwEventSpecialGLUT(key, x, y))
		return;		// Already handled by AntTweakBar
}

/// GLUT callback - when the user presses or releases a mouse button. See glutMouseFunc for more info
void on_mouse_func(int button, int state, int x, int y)
{
	// Inform AntTweakBar
	if (TwEventMouseButtonGLUT(button, state, x, y))
		return;		// Already handled by AntTweakBar

	the_camera.OnMouseFunc(button, state, x, y);
}

/// GLUT callback - when the user moves the mouse when holding a botton. See glutMotionFunc for more info
void on_motion_func(int x, int y)
{
	// Inform AntTweakBar
	if (TwEventMouseMotionGLUT(x, y))
		return;		// Already handled by AntTweakBar

	the_camera.OnMotionFunc(x, y);
}

/// GLUT callback - when the user moves the mouse when not holding a botton. See glutPassiveMotionFunc for more info
void on_passive_motion_func(int x, int y)
{
	// Inform AntTweakBar
	if (TwEventMouseMotionGLUT(x, y))
		return;		// Already handled by AntTweakBar
}

/// GLUT callback - when the window is resized. See glutReshapeFunc for more info
void on_reshape(int width, int height)
{
	win_width = width;
	win_height = height;
	
	glViewport(0, 0, win_width, win_height);
	resize_fullscreen_textures();
	
	// Inform AntTweakBar
	TwWindowSize(win_width, win_height);
}

/// GLUT callback - when our animation timer requests a new frame. See glutTimerFunc for more info
void on_timer(int)
{
	// Register the timer to be called again in 20 ms, and rerender the scene
	glutTimerFunc(20, on_timer, 0);
	glutPostRedisplay();
}

/// GLUT callback - when the window needs to be redrawn. See glutDisplayFunc for more info
void on_display()
{
	//--  Update all the data

	// Update the application time
	int current_glut_time = glutGet(GLUT_ELAPSED_TIME);
	int app_time_diff_ms = current_glut_time - last_glut_time;
	app_time_ms += app_time_diff_ms;
	last_glut_time = current_glut_time;

	// Update the scene
	update_scene(app_time_diff_ms);

	//--  Render the scene

	// Render the scene
	render_scene();
	
	// Render the GUI
	TwDraw();

	// Swaps the front and back buffer (double-buffering)
	glutSwapBuffers();
}

/// Callback function to be called when we make an error in OpenGL
void GLAPIENTRY simple_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
	switch (type)			// Report only some errors
	{
	case GL_DEBUG_TYPE_ERROR:
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << message << endl;		// Put the breakpoint here
		return;
	default:
		return;
	}
}

/// C main function :-)
int main(int argc, char ** argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);		// Return from the main loop

	// Set OpenGL Context parameters
	glutInitContextVersion(4, 3);				// Use OpenGL 4.3
	glutInitContextProfile(GLUT_CORE_PROFILE);	// Use OpenGL core profile
	glutInitContextFlags(GLUT_DEBUG);			// Use OpenGL debug context

	// Initialize window
	glutInitWindowSize(win_width, win_height);
	glutCreateWindow("PV227 - Project 2 - 2017");

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	// Initialize DevIL library
	InitDevIL();

	// Set the OpenGL debug callback
	SetDebugCallback(simple_debug_callback);

	// Register GLUT callbacks
	glutDisplayFunc(on_display);
	glutReshapeFunc(on_reshape);
	glutTimerFunc(20, on_timer, 0);

	glutKeyboardFunc(on_keyboard_func);
	glutSpecialFunc(on_special_func);
	glutMouseFunc(on_mouse_func);
	glutMotionFunc(on_motion_func);
	glutPassiveMotionFunc(on_passive_motion_func);

	// Initialize OpenGL stuff
	init_gui();
	init_scene();

	// Everything is loaded and prepared, reset the application time
	last_glut_time = glutGet(GLUT_ELAPSED_TIME);
	app_time_ms = 0;

	// Run the main loop
	glutMainLoop();

	// Unload AntTweakBar
	TwTerminate();

	return 0;
}