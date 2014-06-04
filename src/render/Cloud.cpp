#include "Cloud.h"
#include "Application.h"
#include "utils/SphereManager.h"

Cloud::Cloud():
	shader(ShaderProgram::loadFromFile(
				"shader/cloud/cloud.vert",
				"shader/cloud/cloud.frag",
				"cloud"))
{

}

void Cloud::draw(Camera& c)
{
	// update time
    float delta = Application::getInstance().getFrameDeltaTime();
	time+=delta;

	// draw
	shader.use();
	shader.setUniform("time",time);

	SphereManager::getInstance().draw(c,shader,4);
	
}
