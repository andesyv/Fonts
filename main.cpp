#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <set>
#include <map>
#include <chrono>
#include "Math/math.h"
#include "shader.h"
#include "mesh.h"

// FreeType: Font building library
#include <ft2build.h>
#include FT_FREETYPE_H

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void checkForErrors();
void loadCharacters();
#ifdef _DEBUG
void setupDebugger();
void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message, const void* userParam);
#endif
float calculateFramerate(double deltaTime);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

bool noDebugger{ false };

struct Character {
	GLuint TextureID;	// ID handle of the glyph texture
	GLuint Width;		// Width of glyph
	GLuint Height;		// Height of glyph
	GLint BearingX;	// OffsetX from baseline to left of glyph
	GLint BearingY;	// OffsetY from baseline to top of glyph
	GLint Advance;		// Offset to advance to next glyph
};

std::map<GLchar, Character> Characters;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/* We're using OpenGL version 4.3 for this project.
	OpenGL version 4.3 came out in 2012, which is 7 years ago.
	Therefore it is safe to assume that computers that are 6 years old
	(probably) support OpenGL 4.3. I don't think many people are using
	computers with GPUs that are more than 7(6) years old.
	*/
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#ifdef _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // Debugging context: Comment out when in release
	#endif

	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
	#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	#ifdef _DEBUG
		setupDebugger();
	#endif

	// Setup shaders
	// -------------
	// PROJECT_DIR is a macro defined in the project settings for visual studio to point to the project base directory path
	ShaderManager::get().add(std::move(Shader{
		{ PROJECT_DIR + std::string{"Shaders\\default.vert" } },
		{ PROJECT_DIR + std::string{"Shaders\\default.frag" } }
		}.setName("default")));
	ShaderManager::get().add(std::move(Shader{
		{ PROJECT_DIR + std::string{"Shaders\\default.vert"} },
		{ PROJECT_DIR + std::string{"Shaders\\text.frag"} }
		}.setName("text")));



	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Fonts
	// ----------
	loadCharacters();



	// Timinigs
	// ----------
	// Time it took to render last frame
	double renderTime{ 0.0 };
	// Time between last frame.
	double deltaTime{ 0.0 };
	std::chrono::high_resolution_clock::time_point frameStart;
	std::chrono::high_resolution_clock::time_point frameStop;



	// Gameobjects
	// ----------------
	std::vector<std::shared_ptr<Mesh>> gameObjects{};

	std::vector<Vertex> vertices{
		{{-0.5f, -0.5f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 0.f} },
		{{0.5f, -0.5f, 0.f}, {0.f, 0.f, -1.f}, {1.f, 0.f} },
		{{0.5f, 0.5f, 0.f}, {0.f, 0.f, -1.f}, {1.f, 1.f} },
		{{-0.5f, -0.5f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 0.f} },
		{{0.5f, 0.5f, 0.f}, {0.f, 0.f, -1.f}, {1.f, 1.f} },
		{{-0.5f, 0.5f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 1.f} }
	};

	// Plane
	gameObjects.push_back(std::make_shared<Mesh>(vertices, Material{ "text" }));
	gameObjects.back()->mDrawMode = GL_TRIANGLES;
	gameObjects.back()->mMaterial.enableParams(Material::PARAM::TEXTURE | Material::PARAM::UVPOS | Material::PARAM::UVSCALE);

	struct comparitor {
		bool operator() (const std::weak_ptr<Mesh>& a, const std::weak_ptr<Mesh>& b)
		{
			if (a.expired() || b.expired())
				return false;
			return a.lock()->mMaterial < b.lock()->mMaterial;
		}
	};
	// Make a set that is sorted
	std::multiset<std::weak_ptr<Mesh>, comparitor> sortedGameobjects{ gameObjects.begin(), gameObjects.end() };
	/*[](const std::weak_ptr<Mesh>& a, const std::weak_ptr<Mesh>& b) -> bool
	{
		if (a.expired() || b.expired())
			return false;
		return a.lock()->mMaterial < b.lock()->mMaterial;
	}*/



	// main render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// The time point of the framestart
		frameStart = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(frameStart - frameStop).count() / 1000000000.0;

		// input
		// -----
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

 
		// Draw all gameobjects
		//for (const auto &ptr : sortedGameobjects) {
		//	if (ptr.expired()) {
		//		std::cout << "Skipped an item because it was expired!" << std::endl;
		//		// Should add some garbage collection on the expired pointers in the future
		//		// Probably just remake the set if a certain number of pointers are expired
		//		continue;
		//	}
		//	auto obj = ptr.lock();

		//	obj->mMaterial.use();
		//	obj->draw();
		//}

		auto object = gameObjects.back();
		object->mMaterial.mTextureID = Characters['g'].TextureID;
		object->mMaterial.mUVScale = gsl::vec2{2.f, 2.f};
		object->mMaterial.mUVPos = gsl::vec2{ 0.f, 0.f};
		object->mMaterial.use();
		object->draw();

		object->mMaterial.mTextureID = Characters['t'].TextureID;
		object->mMaterial.mUVScale = gsl::vec2{ 2.f, 2.f };
		object->mMaterial.mUVPos = gsl::vec2{ -1.f, 0.f };
		object->mMaterial.use();
		object->draw();

		// glUniform3fv(glGetUniformLocation(3, ""), 1, )
		// std::cout << "FPS: " << calculateFramerate(renderTime + deltaTime) << std::endl;


		// Use old debugging if debugger is disabled
		if (noDebugger)
			checkForErrors();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();

		// The time point of the framestart
		frameStop = std::chrono::high_resolution_clock::now();

		renderTime = std::chrono::duration_cast<std::chrono::nanoseconds>(frameStop - frameStart).count() / 1000000000.0;
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void loadCharacters()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, "Fonts/consola.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Font size
	FT_Set_Pixel_Sizes(face, 0, 48);


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		gsl::vec4 borderColor{ 0.f, 0.f, 0.f, 1.f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor.xP());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			face->glyph->bitmap_left,
			face->glyph->bitmap_top,
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void checkForErrors()
{
	// Check for errors (slow way for opengl < 4.3):
	// (Should be removed in release)
	while (GLenum err = glGetError() != GL_NO_ERROR) {
		std::string error{};
		switch (err) {
		case GL_INVALID_ENUM:					error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:					error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:				error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:					error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:	error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		default:								error = "UNKNOWN_ERROR"; break;
		}
		std::cout << "Error: " << error << std::endl;
	}
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

#ifdef _DEBUG
void setupDebugger()
{
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	else
	{
		noDebugger = true;
	}
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message, const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}
#endif

float calculateFramerate(double deltaTime)
{
	return static_cast<float>(1.0 / deltaTime);
}