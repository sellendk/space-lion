#ifndef renderHub_h
#define renderHub_h
/*
/	In this class, all parts directly related to the rendering done on a single device come together.
/
/	OpenGL context, window management, scene management, aswell as communication with other engine modules
/	is handled here.
*/

#include <vector>
#include "scene.h"
#include "postProcessor.h"
#include "framebufferObject.h"
#include "GL/glfw.h"

//pragmas seem to be only necessary in windows
#ifdef _WIN32
	#pragma comment(lib,"GLFW.lib")
	#pragma comment(lib,"opengl32.lib")
#endif

class renderHub
{
public:
	renderHub(void);
	~renderHub(void);

	//	Initialize OpenGL context and create a window
	bool init();

	//	Scene handling
	bool addScene();
	bool deleteScene();
	bool setSceneParameters();
	scene* getScene(const int index);
	void setActiveScene(const int index);
	scene* getActiveScene();

	//	Render a frame of the active scene and check event-queue
	void run();
	//	Run poisson image editing on an input famebuffer
	void runPoissonImageEditing();

private:
	std::vector<framebufferObject> framebufferList;
	std::list<scene> sceneList;

	framebufferObject *activeFramebuffer;
	scene *activeScene;

	bool running;
};

#endif
