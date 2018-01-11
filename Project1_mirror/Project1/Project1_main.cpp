#include "Project1_main.h"

//---------------------
//----    SCENE    ----
//---------------------

// Geometry of the floor: one quad from four vertices
const int floor_quad_vertices_count = 4;
const float floor_size = 35.0f;		// Half of the floor size
const float floor_repeat = 5.0f;	// Number of times the texture is repeated
const float floor_quad_vertices[floor_quad_vertices_count * 8] =
{
//       Position                             Normal                Tex Coord
	-floor_size, 0.0f, -floor_size,		 0.0f, 1.0f, 0.0f,		0.0f, floor_repeat,
	-floor_size, 0.0f, floor_size,		 0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
	 floor_size, 0.0f, -floor_size,		 0.0f, 1.0f, 0.0f,		floor_repeat, floor_repeat,
	 floor_size, 0.0f, floor_size,		 0.0f, 1.0f, 0.0f,		floor_repeat, 0.0f,
};

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

	// Shader for rendering the glass
	glass_tex_program.Init();
	glass_tex_program.AddVertexShader("Shaders/texture_vertex.glsl");
	glass_tex_program.AddFragmentShader("Shaders/glass_tex_fragment.glsl");
	glass_tex_program.Link();

	// Shader for rendering the glass
	glass_notex_program.Init();
	glass_notex_program.AddVertexShader("Shaders/glass_vertex.glsl");
	glass_notex_program.AddFragmentShader("Shaders/glass_notex_fragment.glsl");
	glass_notex_program.Link();

	// Postprocessing programs
	fullscreen_horizontal_program.Init();
	fullscreen_horizontal_program.AddVertexShader("Shaders/fullscreen_quad_vertex.glsl");
	fullscreen_horizontal_program.AddFragmentShader("Shaders/blur_horizontal_fragment.glsl");
	fullscreen_horizontal_program.Link();

	fullscreen_vertical_program.Init();
	fullscreen_vertical_program.AddVertexShader("Shaders/fullscreen_quad_vertex.glsl");
	fullscreen_vertical_program.AddFragmentShader("Shaders/blur_vertical_fragment.glsl");
	fullscreen_vertical_program.Link();

	fullscreen_program.Init();
	fullscreen_program.AddVertexShader("Shaders/fullscreen_quad_vertex.glsl");
	fullscreen_program.AddFragmentShader("Shaders/fullscreen_quad_fragment.glsl");
	fullscreen_program.Link();

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
	PhongLights_ubo.PhongLights.push_back(
		PhongLight::CreateDirectionalLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f), glm::vec3(0.5f), glm::vec3(0.5f)));
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
	RedMaterial_ubo.SetMaterial		(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 0.0f, 0.0f), true, 200.0f, 1.0f));
	GreenMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(0.0f, 1.0f, 0.0f), true, 200.0f, 1.0f));
	BlueMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(0.0f, 0.0f, 1.0f), true, 200.0f, 1.0f));
	CyanMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(0.0f, 1.0f, 1.0f), true, 200.0f, 1.0f));
	MagentaMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 0.0f, 1.0f), true, 200.0f, 1.0f));
	YellowMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 1.0f, 0.0f), true, 200.0f, 1.0f));
	WhiteMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 1.0f, 1.0f), true, 200.0f, 1.0f));
	FloorMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(0.7f, 0.7f, 0.7f), true, 200.0f, 1.0f));
	GlassMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 1.0f, 1.0f), false, 0.0f, 1.0f));
	RedMaterial_ubo.UpdateOpenGLData();
	GreenMaterial_ubo.UpdateOpenGLData();
	BlueMaterial_ubo.UpdateOpenGLData();
	CyanMaterial_ubo.UpdateOpenGLData();
	MagentaMaterial_ubo.UpdateOpenGLData();
	YellowMaterial_ubo.UpdateOpenGLData();
	WhiteMaterial_ubo.UpdateOpenGLData();
	FloorMaterial_ubo.UpdateOpenGLData();
	GlassMaterial_ubo.UpdateOpenGLData();

	// Add the materials into our list of materials (we do not add the material for the floor, only the basic colors)
	Colors_ubo.push_back(&RedMaterial_ubo);
	Colors_ubo.push_back(&GreenMaterial_ubo);
	Colors_ubo.push_back(&BlueMaterial_ubo);
	Colors_ubo.push_back(&CyanMaterial_ubo);
	Colors_ubo.push_back(&MagentaMaterial_ubo);
	Colors_ubo.push_back(&YellowMaterial_ubo);
	Colors_ubo.push_back(&WhiteMaterial_ubo);

	//----------------------------------------------
	//--  Prepare the camera

	the_camera.SetCenter(glm::vec3(0.0f, 1.0f, 0.0f));
	the_camera.SetCamera(1.25f, -0.5f, 50.0f);
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
	SetTexture2DParameters(GL_MIRRORED_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	lenna_tex = CreateAndLoadTexture2D(L"../../textures/Lenna.jpg");
	glBindTexture(GL_TEXTURE_2D, lenna_tex);
	SetTexture2DParameters(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	mask_tex = CreateAndLoadTexture2D(L"../../textures/Mask.png");
	glBindTexture(GL_TEXTURE_2D, mask_tex);
	SetTexture2DParameters(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Add the textures into our list of textures
	Textures.push_back(wood_tex);
	Textures.push_back(lenna_tex);

	//----------------------------------------------
	//--  Create a random scene. We create a grid of random objects with random materials.

	srand(12345);
	float x_start = -21.0f;
	float z_start = -21.0f;
	float x_spacing = 7.0f;
	float z_spacing = 7.0f;
	int x_grid_size = 7;
	int z_grid_size = 7;

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
			scene_object.program = &notexture_program;
			scene_object.material_ubo = Colors_ubo[material];
			scene_object.texture = 0;
		}
		else
		{
			// Object with a texture
			scene_object.program = &texture_program;
			scene_object.material_ubo = &WhiteMaterial_ubo;
			scene_object.texture = Textures[material - int(Colors_ubo.size())];
		}
		ObjectsInScene.push_back(scene_object);
	}

	//------------------------
	//--  Create the floor  --

	// Prepare the floor model matrix.
	FloorModel_ubo.Init();
	FloorModel_ubo.SetMatrix(glm::mat4(1.0f));
	FloorModel_ubo.UpdateOpenGLData();

	// Create floor geometry
	geom_floor.VertexBuffers.resize(1, 0);
	glGenBuffers(1, &geom_floor.VertexBuffers[0]);
	glBindBuffer(GL_ARRAY_BUFFER, geom_floor.VertexBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, floor_quad_vertices_count * sizeof(float) * 8, floor_quad_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create a vertex array object for floor geometry
	glGenVertexArrays(1, &geom_floor.VAO);

	// Set the parameters of the geometry
	glBindVertexArray(geom_floor.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, geom_floor.VertexBuffers[0]);
	glEnableVertexAttribArray(DEFAULT_POSITION_LOC);
	glVertexAttribPointer(DEFAULT_POSITION_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	glEnableVertexAttribArray(DEFAULT_NORMAL_LOC);
	glVertexAttribPointer(DEFAULT_NORMAL_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *)(sizeof(float) * 3));
	glEnableVertexAttribArray(DEFAULT_TEX_COORD_LOC);
	glVertexAttribPointer(DEFAULT_TEX_COORD_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *)(sizeof(float) * 6));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Set the Mode and the number of vertices to draw
	geom_floor.Mode = GL_TRIANGLE_STRIP;
	geom_floor.DrawArraysCount = 4;
	geom_floor.DrawElementsCount = 0;

	// Create a scene object from the floor
	SceneObject floor_scene_object;
	floor_scene_object.geometry = &geom_floor;
	floor_scene_object.model_ubo = &FloorModel_ubo;
	floor_scene_object.program = &texture_program;
	floor_scene_object.material_ubo = &FloorMaterial_ubo;
	floor_scene_object.texture = wood_tex;
	ObjectsInScene.push_back(floor_scene_object);

	//------------------------
	//--  Create the glass  --

	// Prepare the glass model matrix.
	glass_model_matrix =
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
	//--  Prepare framebuffers

	// Prepare FBO textures
	glGenTextures(1, &fbo1_color_texture);
	glGenTextures(1, &fbo1_depth_texture);
	resize_fullscreen_textures();

	glBindTexture(GL_TEXTURE_2D, fbo1_color_texture);
	SetTexture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, fbo1_depth_texture);
	SetTexture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Prepare FBOs
	glGenFramebuffers(1, &fbo1);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo1_color_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fbo1_depth_texture, 0);
	glDrawBuffers(1, DrawBuffersConstants);
	CheckFramebufferStatus("FBO 1");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//----------------------------------------------
	//--  Miscellaneous

	// Create the query object
	glGenQueries(1, &RenderTimeQuery);
}

float compute_gauss_value(int index)
{
	int v = (index + 1);
	return v * v;
}

/// Updates the scene: performs animations, updates the data of the buffers, etc.
void update_scene(int app_time_diff_ms)
{
	// Data of the main camera
	CameraData_ubo.SetProjection(
		glm::perspective(glm::radians(45.0f), float(win_width) / float(win_height), 0.5f, 1000.0f));
	CameraData_ubo.SetCamera(the_camera);
	CameraData_ubo.UpdateOpenGLData();

	// Gauss values
	int mid = blur_kernel_size / 2;
	gauss_sum = 0.0;
	if (blur_kernel_size % 2 == 0)
	{
		for (int i = 0; i < mid; ++i)
		{
			float value = compute_gauss_value(i);
			gauss_array[i] = value;
			gauss_sum += value;
		}
		for (int i = mid + 1; i < blur_kernel_size; ++i)
		{
			int index = blur_kernel_size - i - 1;
			float value = gauss_array[index];
			gauss_array[i] = value;
			gauss_sum += value;
		}
	}
	else
	{
		for (int i = 0; i < mid; ++i)
		{
			float value = compute_gauss_value(i);
			gauss_array[i] = value;
			gauss_sum += value;
		}
		float value = compute_gauss_value(mid);
		gauss_array[mid] = value;
		gauss_sum += value;
		for (int i = mid + 1; i < blur_kernel_size; ++i)
		{
			int index = blur_kernel_size - i - 1;
			float value = gauss_array[index];
			gauss_array[i] = value;
			gauss_sum += value;
		}
	}

	//gauss_sum *= gauss_sum;
}

void render_stuff()
{
	// Render all objects in the scene
	for (auto iter = ObjectsInScene.begin(); iter != ObjectsInScene.end(); ++iter)
	{
		if (iter->program && iter->program->IsValid())
			iter->program->Use();
		else continue;		// The program is not ready, skip this object and render another one

		// Set the data of the material
		if (iter->material_ubo)
			iter->material_ubo->BindBuffer(DEFAULT_MATERIAL_BINDING);
		// Set the data of the object
		if (iter->model_ubo)
			iter->model_ubo->BindBuffer(DEFAULT_OBJECT_BINDING);

		// Set the texture
		glActiveTexture(GL_TEXTURE0);	glBindTexture(GL_TEXTURE_2D, iter->texture);

		// Render the object
		if (iter->geometry)
		{
			iter->geometry->BindVAO();
			iter->geometry->Draw();
		}
	}
}

void render_glass(bool with_tex)
{
	ShaderProgram glass_program = with_tex ? glass_tex_program : glass_notex_program;

	// Render the glass
	if (glass_program.IsValid())
	{
		glass_program.Use();

		if (with_tex)
		{
			glActiveTexture(GL_TEXTURE0);	glBindTexture(GL_TEXTURE_2D, mask_tex);
		}
		else
		{
			glass_program.Uniform1f("alpha", 1.0 - glass_transparency);
		}

		GlassMaterial_ubo.BindBuffer(DEFAULT_MATERIAL_BINDING);
		GlassModel_ubo.BindBuffer(DEFAULT_OBJECT_BINDING);

		geom_glass.BindVAO();
		geom_glass.Draw();
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

void fullscreen_render(ShaderProgram program, GLuint input_texture)
{
	if (program.IsValid())
	{
		glActiveTexture(GL_TEXTURE0);	glBindTexture(GL_TEXTURE_2D, input_texture);

		program.Use();
		program.Uniform1i("blur_kernel_size", blur_kernel_size);
		program.Uniform1fv("gauss_array", 128, gauss_array);
		program.Uniform1f("gauss_sum", gauss_sum);

		geom_fullscreen_quad.BindVAO();
		geom_fullscreen_quad.Draw();
	}
}

void prepare_framebuffer(GLuint framebuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void copy_color_buffer(GLuint from, GLuint to)
{
	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, from);
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, to);
	glBlitFramebufferEXT(0, 0, win_width, win_height,
						 0, 0, win_width, win_height,
						 GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

/// Renders the whole frame
void render_scene()
{
	// Start measuring the elapsed time
	glBeginQuery(GL_TIME_ELAPSED, RenderTimeQuery);

	//---------------------------------------------------
	//--  Render the final scene into the main window  --

	glEnable(GL_DEPTH_TEST);

	// Set the data of the camera and the lights
	CameraData_ubo.BindBuffer(DEFAULT_CAMERA_BINDING);
	PhongLights_ubo.BindBuffer(DEFAULT_LIGHTS_BINDING);

	// predmety sa vyrenderuju do FBO1
	prepare_framebuffer(fbo1);
	render_stuff();

	// prepnem FB
	prepare_framebuffer(0);
	copy_color_buffer(fbo1, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// sklo do stencil bufferu
	glEnable(GL_STENCIL_TEST);
	enable_draw_to_stencil();
	render_glass(true);
	disable_draw_to_stencil();

	// blur fullscreenu podla stencil buffru
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glDisable(GL_DEPTH_TEST);

	fullscreen_render(fullscreen_horizontal_program, fbo1_color_texture);

	// kopia color bufferu z 0 do FBO1
	copy_color_buffer(0, fbo1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	fullscreen_render(fullscreen_vertical_program, fbo1_color_texture);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	// render skla
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	render_glass(false);
	glDisable(GL_BLEND);

	//----------------------------------------------

	// Reset the VAO, the program, and the framebuffer
	glBindVertexArray(0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
	glBindTexture(GL_TEXTURE_2D, fbo1_color_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, win_width, win_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D, fbo1_depth_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, win_width, win_height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

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
	glass_transparency = 0.3f;
	blur_kernel_size = 20;

	// Initialize AntTweakBar
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(win_width, win_height);
	TwGLUTModifiersFunc(glutGetModifiers);

	// Initialize GUI
	the_gui = TwNewBar("Parameters");
	TwAddButton(the_gui, "Reload", reload, nullptr, nullptr);

	TwAddVarRO(the_gui, "Render time (ms)", TW_TYPE_FLOAT, &render_time_ms, nullptr);

	TwAddVarRW(the_gui, "Glass Transparency", TW_TYPE_FLOAT, &glass_transparency, "min=0.0 max=1.0 step=0.01");

	char buffer[256];
	sprintf_s(buffer, 256, "min=1 max=%i step=1", max_blur_kernel_size);
	TwAddVarRW(the_gui, "Blur kernel size", TW_TYPE_INT32, &blur_kernel_size, buffer);
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
	glutCreateWindow("PV227 - Project 1");

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