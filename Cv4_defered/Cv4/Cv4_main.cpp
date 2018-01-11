#include "Cv4_main.h"

#include "glm/gtx/color_space.hpp"

//---------------------
//----    SCENE    ----
//---------------------

/// Reloads all shaders
void reload_shaders()
{
	// Program for objects without textures for forward shading
	notexture_forward_program.Init();
	notexture_forward_program.AddVertexShader("Shaders/notexture_vertex.glsl");
	notexture_forward_program.AddFragmentShader("Shaders/notexture_forward_fragment.glsl");
	notexture_forward_program.Link();

	// Program for objects without textures for deferred shading
	notexture_deferred_program.Init();
	notexture_deferred_program.AddVertexShader("Shaders/notexture_vertex.glsl");
	notexture_deferred_program.AddFragmentShader("Shaders/notexture_deferred_fragment.glsl");
	notexture_deferred_program.Link();

	// Program for objects with textures for forward shading
	texture_forward_program.Init();
	texture_forward_program.AddVertexShader("Shaders/texture_vertex.glsl");
	texture_forward_program.AddFragmentShader("Shaders/texture_forward_fragment.glsl");
	texture_forward_program.Link();

	// Program for objects with textures for deferred shading
	texture_deferred_program.Init();
	texture_deferred_program.AddVertexShader("Shaders/texture_vertex.glsl");
	texture_deferred_program.AddFragmentShader("Shaders/texture_deferred_fragment.glsl");
	texture_deferred_program.Link();

	// Program for displaying a texture on the screen
	display_texture_program.Init();
	display_texture_program.AddVertexShader("Shaders/fullscreen_quad_vertex.glsl");
	display_texture_program.AddFragmentShader("Shaders/display_texture_fragment.glsl");
	display_texture_program.Link();

	// Program for evaluating the G-buffer using a screen quad
	evaluate_quad_program.Init();
	evaluate_quad_program.AddVertexShader("Shaders/fullscreen_quad_vertex.glsl");
	evaluate_quad_program.AddFragmentShader("Shaders/evaluate_quad_fragment.glsl");
	evaluate_quad_program.Link();

	// Program for evaluating the G-buffer using spheres
	evaluate_sphere_program.Init();
	evaluate_sphere_program.AddVertexShader("Shaders/evaluate_sphere_vertex.glsl");
	evaluate_sphere_program.AddFragmentShader("Shaders/evaluate_sphere_fragment.glsl");
	evaluate_sphere_program.Link();

	// Setup the shader maps
	notexture_program_map.RemoveAll();
	notexture_program_map.AddProgram(FORWARD_SHADING_SHADERS, &notexture_forward_program);
	notexture_program_map.AddProgram(DEFERRED_SHADING_SHADERS, &notexture_deferred_program);

	texture_program_map.RemoveAll();
	texture_program_map.AddProgram(FORWARD_SHADING_SHADERS, &texture_forward_program);
	texture_program_map.AddProgram(DEFERRED_SHADING_SHADERS, &texture_deferred_program);

	cout << "Shaders are reloaded" << endl;
}

/// Initializes all objects of our scene
void init_scene()
{
	// Parameters of out random scene. We create a grid of random objects with random materials.
	float x_start = -50.0f;
	float z_start = -50.0f;
	float x_spacing = 5.0f;
	float z_spacing = 5.0f;
	int x_grid_size = 21;
	int z_grid_size = 21;

	//----------------------------------------------
	//--  Display some information about the OpenGL

	PrintOpenGLInfo();

	//----------------------------------------------
	//--  Load shaders

	reload_shaders();
	
	//----------------------------------------------
	//--  Prepare the lights
	
	// Create all lights, including those that are not used.
	AllLights.resize(LIGHTS_MAX_COUNT);
	glm::vec3 lights_start = glm::vec3(x_start - x_spacing * 0.5f, 1.0f, z_start - z_spacing * 0.5f);
	glm::vec3 lights_size = glm::vec3(x_spacing * float(x_grid_size), 2.0f, z_spacing * float(z_grid_size));
	for (int i = 0; i < LIGHTS_MAX_COUNT; i++)
	{
		// Place the light at a random place into the scene
		glm::vec3 light_position = lights_start + lights_size * glm::vec3(float(rand()), float(rand()), float(rand())) / float(RAND_MAX);

		// Give the light a random color. Thanks to the fact that GLM has functions for HSV -> RGB conversion,
		// we choose hue randomly, use 1.0 for saturation and value, and convert it to RGB, thus obtaining
		// a uniformly random color.
		glm::vec3 light_color = glm::rgbColor(glm::vec3(float(rand()) / float(RAND_MAX) * 360.0f, 1.0f, 1.0f));

		// We use black ambient (the scene contains hundreds of lights, non-black ambient would cause "overexposure"),
		// and also we use attenuation (again, without attenuation, the objects would be "overexposed").
		AllLights[i] = PhongLight::CreatePointLight(light_position, glm::vec3(0.0f), light_color, light_color, 0.2f, 0.0f, 1.0f);
	}

	PhongLights_ubo.Init(LIGHTS_MAX_COUNT, LIGHTS_STORAGE);
	PhongLights_ubo.SetGlobalAmbient(glm::vec3(0.0f));
	PhongLights_ubo.PhongLights = AllLights;
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
	RedMaterial_ubo.SetMaterial		(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 0.0f, 0.0f), true, 200.0f, 1.0f));
	GreenMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(0.0f, 1.0f, 0.0f), true, 200.0f, 1.0f));
	BlueMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(0.0f, 0.0f, 1.0f), true, 200.0f, 1.0f));
	CyanMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(0.0f, 1.0f, 1.0f), true, 200.0f, 1.0f));
	MagentaMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 0.0f, 1.0f), true, 200.0f, 1.0f));
	YellowMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 1.0f, 0.0f), true, 200.0f, 1.0f));
	WhiteMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 1.0f, 1.0f), true, 200.0f, 1.0f));
	FloorMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(0.7f, 0.7f, 0.7f), true, 200.0f, 1.0f));
	RedMaterial_ubo.UpdateOpenGLData();
	GreenMaterial_ubo.UpdateOpenGLData();
	BlueMaterial_ubo.UpdateOpenGLData();
	CyanMaterial_ubo.UpdateOpenGLData();
	MagentaMaterial_ubo.UpdateOpenGLData();
	YellowMaterial_ubo.UpdateOpenGLData();
	WhiteMaterial_ubo.UpdateOpenGLData();
	FloorMaterial_ubo.UpdateOpenGLData();

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

	// Prepare the camera
	the_camera.SetCenter(glm::vec3(0.0f, 5.0f, 0.0f));
	the_camera.SetCamera(-0.5f, -0.5f, 15.0f);
	CameraData_ubo.Init();

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
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f))));
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

	// Load textures and set their parameters
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

	//----------------------------------------------
	//--  Prepare objects in the scene

	// Create a random scene. We create a grid of random objects with random materials.
	srand(12345);
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
			scene_object.programs = &notexture_program_map;
			scene_object.material_ubo = Colors_ubo[material];
			scene_object.texture = 0;
		}
		else
		{
			// Object with a texture
			scene_object.programs = &texture_program_map;
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
	floor_scene_object.programs = &notexture_program_map;
	floor_scene_object.material_ubo = &FloorMaterial_ubo;
	floor_scene_object.texture = 0;
	ObjectsInScene.push_back(floor_scene_object);

	//----------------------------------------------
	//--  Prepare framebuffers

	// Create textures for G-buffer and for evaluation of the G-buffer, and set their basic parameters
	glGenTextures(1, &GbufferPositionTexture);
	glGenTextures(1, &GbufferNormalTexture);
	glGenTextures(1, &GbufferAlbedoTexture);
	glGenTextures(1, &GbufferDepthTexture);
	glGenTextures(1, &EvaluateGbufferColorTexture);
	glBindTexture(GL_TEXTURE_2D, GbufferPositionTexture);
	SetTexture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, GbufferNormalTexture);
	SetTexture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, GbufferAlbedoTexture);
	SetTexture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, GbufferDepthTexture);
	SetTexture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, EvaluateGbufferColorTexture);
	SetTexture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Allocate the memory of the textures
	resize_fullscreen_textures();

	// Create the framebuffer for rendering into the G-buffer, and set the textures to it
	glGenFramebuffers(1, &GbufferFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, GbufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GbufferPositionTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, GbufferNormalTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, GbufferAlbedoTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, GbufferDepthTexture, 0);
	glDrawBuffers(3, DrawBuffersConstants);
	CheckFramebufferStatus("G-buffer");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Create the framebuffer for evaluation of the G-buffer, and set the textures to it. Use the same depth texture as the G-buffer
	glGenFramebuffers(1, &EvaluateGbufferFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, EvaluateGbufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, EvaluateGbufferColorTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, GbufferDepthTexture, 0);
	glDrawBuffers(1, DrawBuffersConstants);
	CheckFramebufferStatus("EvaluateGbuffer");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//----------------------------------------------
	//--  Miscellaneous

	// Create the query object
	glGenQueries(1, &RenderTimeQuery);
}

/// Updates the scene: performs animations, updates the data of the buffers, etc.
void update_scene(int app_time_diff_ms)
{
	// Update the list of lights
	PhongLights_ubo.PhongLights.assign(AllLights.begin(), AllLights.begin() + lights_count);
	PhongLights_ubo.UpdateOpenGLData();

	// Data of the main camera
	CameraData_ubo.SetProjection(
		glm::perspective(glm::radians(45.0f), float(win_width) / float(win_height), 0.1f, 1000.0f));
	CameraData_ubo.SetCamera(the_camera);
	CameraData_ubo.UpdateOpenGLData();
}

/// Render all objects in the scene using a proper set of shaders (FORWARD_SHADING_SHADERS or DEFERRED_SHADING_SHADERS)
void render_all_objects(int shader_set)
{
	for (auto iter = ObjectsInScene.begin(); iter != ObjectsInScene.end(); ++iter)
	{
		if (!iter->programs->UseProgram(shader_set))
			continue;		// The program is not ready, skip this object and render another one

		if (iter->material_ubo)
			iter->material_ubo->BindBuffer(DEFAULT_MATERIAL_BINDING);
		if (iter->model_ubo)
			iter->model_ubo->BindBuffer(DEFAULT_OBJECT_BINDING);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, iter->texture);

		if (iter->geometry)
		{
			iter->geometry->BindVAO();
			iter->geometry->Draw();
		}
	}
}

/// Renders the whole frame
void render_scene()
{
	// Start measuring the elapsed time
	glBeginQuery(GL_TIME_ELAPSED, RenderTimeQuery);

	if (current_shading_technique == USE_FORWARD_SHADING)
	{
		// Render into the main window
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, win_width, win_height);

		// Clear the framebuffer, clear the color and depth
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// Set the data of the camera and the lights
		CameraData_ubo.BindBuffer(DEFAULT_CAMERA_BINDING);
		PhongLights_ubo.BindBuffer(DEFAULT_LIGHTS_BINDING);
		
		// Render all objects in the scene, use forward shading program
		render_all_objects(FORWARD_SHADING_SHADERS);
	}
	else
	{
		// First, render into the G-buffer
		glBindFramebuffer(GL_FRAMEBUFFER, GbufferFBO);
		glViewport(0, 0, win_width, win_height);

		// Clear the G-buffer, clear all textures to (0,0,0,0) and depth to 1.0
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// Set the data of the camera and the lights
		CameraData_ubo.BindBuffer(DEFAULT_CAMERA_BINDING);
		PhongLights_ubo.BindBuffer(DEFAULT_LIGHTS_BINDING);

		// Render all objects in the scene, use deferred shading program
		render_all_objects(DEFERRED_SHADING_SHADERS);

		// Use the G-buffer
		if ((current_shading_technique == USE_DEFERRED_SHADING_DISPLAY_POSITION) ||
			(current_shading_technique == USE_DEFERRED_SHADING_DISPLAY_NORMAL) ||
			(current_shading_technique == USE_DEFERRED_SHADING_DISPLAY_ALBEDO))
		{
			// Display the position texture, normal texture, or albedo texture

			// Render into the main window
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, win_width, win_height);

			// Clear the framebuffer, clear the color. We do not need depth and depth test
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			if (current_shading_technique == USE_DEFERRED_SHADING_DISPLAY_POSITION)
			{
				display_texture_program.Use();
				display_texture_program.Uniform1f("scale", 0.02f);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, GbufferPositionTexture);
			}
			else if (current_shading_technique == USE_DEFERRED_SHADING_DISPLAY_NORMAL)
			{
				display_texture_program.Use();
				display_texture_program.Uniform1f("scale", 1.0f);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, GbufferNormalTexture);
			}
			else if (current_shading_technique == USE_DEFERRED_SHADING_DISPLAY_ALBEDO)
			{
				display_texture_program.Use();
				display_texture_program.Uniform1f("scale", 1.0f);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, GbufferAlbedoTexture);
			}

			// Render the fullscreen quad
			geom_fullscreen_quad.BindVAO();
			geom_fullscreen_quad.Draw();
		}
		else if (current_shading_technique == USE_DEFERRED_SHADING_EVALUATE_QUAD)
		{
			// Evaluate the lighting using a fullscreen quad

			// Render into the main window
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, win_width, win_height);

			// Clear the framebuffer, clear the color. We do not need depth and depth test
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			// Use the proper program
			evaluate_quad_program.Use();

			// Bind all three textures
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, GbufferPositionTexture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, GbufferNormalTexture);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, GbufferAlbedoTexture);

			// Bind the data with the camera and the lights
			CameraData_ubo.BindBuffer(DEFAULT_CAMERA_BINDING);
			PhongLights_ubo.BindBuffer(DEFAULT_LIGHTS_BINDING);

			// Bind the data with specular and shininess (all materials have the same)
			WhiteMaterial_ubo.BindBuffer(DEFAULT_MATERIAL_BINDING);

			// Render the fullscreen quad
			geom_fullscreen_quad.BindVAO();
			geom_fullscreen_quad.Draw();
		}
		else if (current_shading_technique == USE_DEFERRED_SHADING_EVALUATE_SPHERE)
		{
			// Evaluate the lighting using a spheres

			glBindFramebuffer(GL_FRAMEBUFFER, EvaluateGbufferFBO);
			glViewport(0, 0, win_width, win_height);

			// Clear the framebuffer, clear the color, do not clear the depth
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// Task 3: Configurate the depth test, blending, and culling
			//			Hint: glEnable/glDisable(GL_DEPTH_TEST/GL_BLEND/GL_CULL_FACE), glDepthMask(GL_TRUE/GL_FALSE),
			//				glCullFace(GL_FRONT/GL_BACK), glBlendFunc(GL_ZERO/GL_ONE/GL_SRC_ALPHA/GL_ONE_MINUS_SRC_ALPHA)
			// Use depth test, do not render into the depth buffer
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			// Use blending, i.e. enable blending and set the blend function
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			// Cull away back faces
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			// Use the proper program
			evaluate_sphere_program.Use();

			// Bind all three textures
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, GbufferPositionTexture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, GbufferNormalTexture);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, GbufferAlbedoTexture);

			// Bind the data with the camera and the lights
			CameraData_ubo.BindBuffer(DEFAULT_CAMERA_BINDING);
			PhongLights_ubo.BindBuffer(DEFAULT_LIGHTS_BINDING);

			// Bind the data with specular and shininess (all materials have the same)
			WhiteMaterial_ubo.BindBuffer(DEFAULT_MATERIAL_BINDING);

			// Render spheres using instancing, one sphere for each active light
			geom_sphere.BindVAO();
			geom_sphere.DrawInstanced(PhongLights_ubo.PhongLights.size());

			// Task 3: Reset the depth test, blending, and culling
			//			Hint: disable blending and culling, enable writing into the depth buffer
			// Reset back the state, i.e. disable blending, enable writing into the depth texture, and disable culling
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
			glDisable(GL_CULL_FACE);

			// Copy the result into the main window
			glBindFramebuffer(GL_READ_FRAMEBUFFER, EvaluateGbufferFBO);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBlitFramebuffer(0, 0, win_width, win_height, 0, 0, win_width, win_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}
	}

	// Reset the VAO and the program
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
	glBindTexture(GL_TEXTURE_2D, GbufferPositionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, win_width, win_height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, GbufferNormalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, win_width, win_height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, GbufferAlbedoTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, win_width, win_height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, GbufferDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, win_width, win_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
	glBindTexture(GL_TEXTURE_2D, EvaluateGbufferColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, win_width, win_height, 0, GL_RGBA, GL_FLOAT, nullptr);
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
	lights_count = 1;
	current_shading_technique = USE_FORWARD_SHADING;

	// Initialize AntTweakBar
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(win_width, win_height);
	TwGLUTModifiersFunc(glutGetModifiers);

	// Initialize GUI
	the_gui = TwNewBar("Parameters");
	TwAddButton(the_gui, "Reload", reload, nullptr, nullptr);

	char buffer[256];
	sprintf_s(buffer, 256, "min=1 max=%i", LIGHTS_MAX_COUNT);
	TwAddVarRW(the_gui, "Lights count", TW_TYPE_INT32, &lights_count, buffer);

	TwAddVarRO(the_gui, "Render time (ms)", TW_TYPE_FLOAT, &render_time_ms, nullptr);

	TwEnumVal techniques[6] = {
		{USE_FORWARD_SHADING, "Forward shading"},
		{USE_DEFERRED_SHADING_DISPLAY_POSITION, "Positions"},
		{USE_DEFERRED_SHADING_DISPLAY_NORMAL, "Normals"},
		{USE_DEFERRED_SHADING_DISPLAY_ALBEDO, "Albedo"},
		{USE_DEFERRED_SHADING_EVALUATE_QUAD, "Deferred shading - quad"},
		{USE_DEFERRED_SHADING_EVALUATE_SPHERE, "Deferred shading - sphere"}
	};

	TwType techniqueType = TwDefineEnum("ShadingTechniques", techniques, 6);
	TwAddVarRW(the_gui, "Shading Technique", techniqueType, &current_shading_technique, nullptr);
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
		break;
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
	glutCreateWindow("PV227 - Cv4: Deferred shading");

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