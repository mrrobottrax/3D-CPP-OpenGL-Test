#include <timeManager.h>
#include <gl.h>

namespace timeManager {
	double dDeltaTime;
	float deltaTime;
	double lastTime;

	void update()
	{
		dDeltaTime = glfwGetTime() - lastTime;
		deltaTime = (float)dDeltaTime;
		lastTime = glfwGetTime();
	}
}
