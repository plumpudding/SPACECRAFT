#include "render/camera/Camera.h"
#include "Application.h"
#include "utils/maths.h"
#include "manager/CameraManager.h"

Camera::Camera():
	view(1.f),
	proj(glm::perspective(DEG2RAD(45.f), Application::getInstance().getWindowRatio(), 0.1f, 1000.f)),
    cameraManager(NULL)
{

}

void Camera::updateCamera(ShaderProgram &prog)
{
	prog.setUniform("view", view);
	prog.setUniform("proj", proj);
}

void Camera::updateFrustum(void)
{
	final=proj*view;

	//near
	frustumPlane[0]=
		glm::vec4(final[2][0]+final[3][0],
				final[2][1]+final[3][1],
				final[2][2]+final[3][2],
				final[2][3]+final[3][3]);

	//far
	frustumPlane[1]=
		glm::vec4(-final[2][0]+final[3][0],
				-final[2][1]+final[3][1],
				-final[2][2]+final[3][2],
				final[2][3]+final[3][3]);

	//left
	frustumPlane[2]=
		glm::vec4(final[0][0]+final[3][0],
				final[0][1]+final[3][1],
				final[0][2]+final[3][2],
				final[0][3]+final[3][3]);

	//right
	frustumPlane[3]=
		glm::vec4(-final[0][0]+final[3][0],
				-final[0][1]+final[3][1],
				-final[0][2]+final[3][2],
				final[0][3]+final[3][3]);

	//bottom
	frustumPlane[4]=
		glm::vec4(final[1][0]+final[3][0],
				final[1][1]+final[3][1],
				final[1][2]+final[3][2],
				final[1][3]+final[3][3]);

	//top
	frustumPlane[5]=
		glm::vec4(-final[1][0]+final[3][0],
				-final[1][1]+final[3][1],
				-final[1][2]+final[3][2],
				final[1][3]+final[3][3]);	
}

//a verifier (pour le signe et tout ça)
bool Camera::isPointInFrustum(glm::vec3 p)
{
	for(int i=0;i<6;i++)if(glm::dot(glm::vec4(p,1.0f),frustumPlane[i])>0.0f)return false;
	return true;
}

void Camera::setCameraManager(CameraManager* c)
{
    cameraManager=c;
}
    
void Camera::update()
{
    if (cameraManager)
    {
        cameraManager->update(*this);
    }
}