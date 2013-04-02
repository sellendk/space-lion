#include "renderHub.h"


renderHub::renderHub(void)
{
}

renderHub::~renderHub(void)
{
}


bool renderHub::init()
{
	//	Initialize GLFW
	if(!glfwInit())
	{
		return false;
	}
	std::cout<<"Initializing GLFW\n";

	//#ifdef _WIN32
	//	//	Get highest openGL Version (doesn't work on linux right now)
	//	//	The glfwGetGLVersion function seems somewhat broken on windows too. When certain, seemingly random
	//	//	conditions are met.
	//
	//	int maj, min, rev;
	//    glfwGetGLVersion(&maj, &min, &rev);
	//
	//	//	Open a glfw window
	//	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, maj);
	//	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, min);
	//	//glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//	//glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//#else
	//	//	Better be save than sorry and choose a low GL version on linux
	//	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
	//	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 0);
	//#endif

	#ifdef _WIN32
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	#else
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 0);
	#endif

	if(!glfwOpenWindow(1200,675,8,8,8,8,32,0,GLFW_WINDOW))
	{
		std::cout<<"-----\n"
				<<"The time is out of joint - O cursed spite,\n"
				<<"That ever I was born to set it right!\n"
				<<"-----\n"
				<<"Error: Couldn't open glfw window";
		return false;
	}

	const GLubyte *version = glGetString(GL_VERSION);
	std::cout<<"Using OpenGL Version: "<<version<<"\n\n";

	//	Initialize glew
	GLenum error = glewInit();
	if( GLEW_OK != error)
	{
		std::cout<<"-----\n"
				<<"The time is out of joint - O cursed spite,\n"
				<<"That ever I was born to set it right!\n"
				<<"-----\n"
				<<"Error: "<<glewGetErrorString(error);
		return false;
	}

	return true;
}

bool renderHub::addScene()
{
	sceneList.push_back(scene());

	return true;
}

bool renderHub::deleteScene()
{
	return false;
}

bool renderHub::setSceneParameters()
{
	return false;
}

scene* renderHub::getScene(const int index)
{
	std::list<scene>::iterator iter = sceneList.begin();
	for( int i=0; i < index; ++i) {++iter;}
	return &(*iter);
}

void renderHub::setActiveScene(const int index)
{
	std::list<scene>::iterator iter = sceneList.begin();
	for( int i=0; i < index; ++i) {++iter;}
	activeScene = &(*iter);
}

void renderHub::run()
{
	/*	
	/	Just for testing and debug purposes I am ignoring the event-queue concept I want to take up later
	/	and manually add entities to the active scene
	*/
	if(!(activeScene->createStaticSceneObject(0,glm::vec3(0.0,0.0,0.0),glm::quat(),"../resources/materials/demoMaterial.slmtl",1)))
	{
		std::cout<<"Failed to create scene object"
				<<"\n";
	}
	if(!(activeScene->createStaticSceneObject(1,glm::vec3(0.0,0.0,-2.0),glm::quat(),"../resources/materials/demoMaterial.slmtl",1)))
	{
		std::cout<<"Failed to create scene object"
				<<"\n";
	}
	if(!(activeScene->createStaticSceneObject(2,glm::vec3(-2.0,0.0,0.0),glm::quat(),"../resources/materials/demoMaterial.slmtl",1)))
	{
		std::cout<<"Failed to create scene object"
				<<"\n";
	}
	//for(int i=2;i<10000;i++)
	//{
	//if(!(activeScene->createStaticSceneObject(i,glm::vec3(-i*2.0,-i*1.5,-i*2.0),glm::quat(),"../resources/materials/demoMaterial.slmtl",1)))
	//{
	//	std::cout<<"Failed to create scene object"
	//			<<"\n";
	//}
	//}


	if(!(activeScene->createSceneCamera(0,glm::vec3(1.0,2.0,1.0),glm::quat(),16.0f/9.0f,55.0f)))
	{
		std::cout<<"Failed to create camera"
				<<"\n";
	}
	if(!(activeScene->createSceneLight(0,glm::vec3(0.0,2.0,0.0),glm::vec4(1.0,1.0,1.0,1.0))))
	{
		std::cout<<"Failed to create light"
				<<"\n";
	}

	activeScene->setActiveCamera(0);
	
	activeScene->testing();


	running = true;
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glEnable (GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	while(running && glfwGetWindowParam(GLFW_OPENED))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		activeScene->render();
		glfwSwapBuffers();
		//glfwSleep(0.01);
	}
}

void renderHub::runPoissonImageEditing()
{
	//	This is all just experimental stuff
	running = true;
	glClearColor(0.0f,0.0f,0.0f,1.0f);

	GLuint placeholder;


	glGenTextures(1, &placeholder);
	glBindTexture(GL_TEXTURE_2D, placeholder);

	glfwLoadTexture2D("../resources/textures/textest.tga",0);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D,0);


	poissonImageProcessor pIp;
	if(!pIp.init())
	{
		std::cout<<"Failed to create poission image processor"
				<<"\n";
	}
	
	while(running && glfwGetWindowParam(GLFW_OPENED))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pIp.render(placeholder);
		glfwSwapBuffers();
	}
}