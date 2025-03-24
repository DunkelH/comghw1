#include <Windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>

#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <vector>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace glm;

// -------------------------------------------------
// Global Variables
// -------------------------------------------------
int Width = 1280;
int Height = 720;
std::vector<float> OutputImage;

class Ray {
public:
	vec3 origin;
	vec3 direction;
	Ray(const vec3& o, const vec3& d) : origin(o), direction(normalize(d)) {}
};

class Camera {
public:
	vec3 eye;
	float l, r, b, t, d;
	Camera(vec3 e, float left, float right, float bottom, float top, float depth)
		: eye(e), l(left), r(right), b(bottom), t(top), d(depth) {}

	Ray generateRay(int i, int j, int width, int height) const {
		float u = l + (r - l) * (i + 0.5f) / width;
		float v = b + (t - b) * (j + 0.5f) / height;
		return Ray(eye, vec3(u, v, -d));
	}
};

class Surface {
public:
	virtual bool intersect(const Ray& ray) const = 0;
};

class Sphere : public Surface {
public:
	vec3 center;
	float radius;
	Sphere(const vec3& c, float r) : center(c), radius(r) {}
	bool intersect(const Ray& ray) const override {
		vec3 oc = ray.origin - center;
		float a = dot(ray.direction, ray.direction);
		float b = 2.0f * dot(oc, ray.direction);
		float c = dot(oc, oc) - radius * radius;
		float discriminant = b * b - 4 * a * c;
		return (discriminant > 0);
	}
};

class Plane : public Surface {
public:
	float y;
	Plane(float yLevel) : y(yLevel) {}
	bool intersect(const Ray& ray) const override {
		if (ray.direction.y == 0) return false;
		float t = (y - ray.origin.y) / ray.direction.y;
		return (t > 0);
	}
};

class Scene {
public:
	std::vector<Surface*> objects;
	void addObject(Surface* obj) {
		objects.push_back(obj);
	}
	bool intersect(const Ray& ray) const {
		for (const auto& obj : objects) {
			if (obj->intersect(ray)) {
				return true;
			}
		}
		return false;
	}
};
// -------------------------------------------------



void render()
{
	//Create our image. We don't want to do this in 
	//the main loop since this may be too slow and we 
	//want a responsive display of our beautiful image.
	//Instead we draw to another buffer and copy this to the 
	//framebuffer using glDrawPixels(...) every refresh
	OutputImage.clear();
	Camera camera(vec3(0, 0, 0), -0.1f, 0.1f, -0.1f, 0.1f, 0.1f);
	Scene scene;
	scene.addObject(new Sphere(vec3(-4, 0, -7), 1));
	scene.addObject(new Sphere(vec3(0, 0, -7), 2));
	scene.addObject(new Sphere(vec3(4, 0, -7), 1));
	scene.addObject(new Plane(-2));

	for (int j = 0; j < Height; ++j) 
	{
		for (int i = 0; i < Width; ++i) 
		{
			// ---------------------------------------------------
			// --- Implement your code here to generate the image
			// ---------------------------------------------------

			// draw a red rectangle in the center of the image
			Ray ray = camera.generateRay(i, j, Width, Height);
			bool hit = scene.intersect(ray);
			vec3 color = hit ? vec3(1.0f, 1.0f, 1.0f) : vec3(0.0f, 0.0f, 0.0f);

			OutputImage.push_back(color.x);
			OutputImage.push_back(color.y);
			OutputImage.push_back(color.z);
		}
	}
}


void resize_callback(GLFWwindow*, int nw, int nh) 
{
	//This is called in response to the window resizing.
	//The new width and height are passed in so we make 
	//any necessary changes:
	Width = nw;
	Height = nh;
	//Tell the viewport to use all of our screen estate
	glViewport(0, 0, nw, nh);

	//This is not necessary, we're just working in 2d so
	//why not let our spaces reflect it?
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, static_cast<double>(Width)
		, 0.0, static_cast<double>(Height)
		, 1.0, -1.0);

	//Reserve memory for our render so that we don't do 
	//excessive allocations and render the image
	OutputImage.reserve(Width * Height * 3);
	render();
}


int main(int argc, char* argv[])
{
	// -------------------------------------------------
	// Initialize Window
	// -------------------------------------------------

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(Width, Height, "OpenGL Viewer", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	//We have an opengl context now. Everything from here on out 
	//is just managing our window or opengl directly.

	//Tell the opengl state machine we don't want it to make 
	//any assumptions about how pixels are aligned in memory 
	//during transfers between host and device (like glDrawPixels(...) )
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	//We call our resize function once to set everything up initially
	//after registering it as a callback with glfw
	glfwSetFramebufferSizeCallback(window, resize_callback);
	resize_callback(NULL, Width, Height);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// -------------------------------------------------------------
		//Rendering begins!
		glDrawPixels(Width, Height, GL_RGB, GL_FLOAT, &OutputImage[0]);
		//and ends.
		// -------------------------------------------------------------

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		//Close when the user hits 'q' or escape
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
			|| glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
