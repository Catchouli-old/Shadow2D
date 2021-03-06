#include "app/Application.h"

#include <stdio.h>

namespace s2d
{
	// Constructor
	// Create and initialise window and renderer
	Application::Application(int32_t width, int32_t height, const char* title, bool hidden)
		: _running(true), _lastUpdate(glfwGetTime()), _lastFPSUpdate(glfwGetTime()),
		_fps(0), _frames(0), _window(nullptr)
	{
		// Init glfw
		if (!glfwInit())
		{
			// Emit error
			fprintf(stderr, "Failed to initialise glfw\n");

			// End execution of application
			end();

			// Return from constructor
			return;
		}

		// Hide window if enabled
		if (hidden)
		{
			glfwWindowHint(GLFW_VISIBLE, 0);
		}

		// Create window
		_window = glfwCreateWindow(width, height, title, NULL, NULL);

		if (_window == nullptr)
		{
			fprintf(stderr, "Failed to create window\n");

			// End execution of application
			end();

			// Return from constructor
			return;
		}

		// Set user pointer to this
		glfwSetWindowUserPointer(_window, this);

		// Make opengl context current
		glfwMakeContextCurrent(_window);

		// Initialise glew
		if (glewInit() != GLEW_OK)
		{
			fprintf(stderr, "Failed to initialise glew\n");

			// End execution of application
			end();

			// Return from constructor
			return;
		}

		// Set default key callback
		glfwSetKeyCallback(_window, _default_key_callback);
	}

	Application::~Application()
	{
		// Destroy window
		if (_window != nullptr)
		{
			glfwDestroyWindow(_window);
			_window = nullptr;
		}

		// Terminate glfw
		glfwTerminate();
	}

	// Handle message loop and call user defined update & render
	bool Application::run()
	{
		// Handle events
		glfwPollEvents();

		// Calculate delta time
		double time = glfwGetTime();
		double dt = time - _lastUpdate;
		double fpsDt = time - _lastFPSUpdate;

		// Update timers
		_lastUpdate = time;

		// Update frame counter
		_frames++;

		// Update FPS if time > 1 second
		if (fpsDt >= 1.0)
		{
			// Update FPS
			_fps = _frames;
			_frames = 0;
			_lastFPSUpdate = time;
		}

		// Call application update
		update(dt);

		// Call application render
		render();

		// Flip buffers
		glfwSwapBuffers(_window);

		// Check for errors
		checkGlError();

		return isRunning();
	}

	/* Protected */

	// Set window title
	void Application::setTitle(const char* title)
	{
		if (_window != nullptr)
			glfwSetWindowTitle(_window, title);
	}

	// Exit application
	void Application::end()
	{
		_running = false;
	}

	void Application::checkGlError()
	{
		GLenum error = glGetError();

		if (error != GL_NO_ERROR)
		{
			fprintf(stderr, "OpenGL error: %s\nPress return to continue\n", gluErrorString(error));

			getchar();

			_running = false;
		}
	}

	/* Private */

	// Exit on esc
	void Application::_default_key_callback(GLFWwindow* window, int32_t key,
		int32_t scancode, int32_t action, int32_t mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}
	}
}