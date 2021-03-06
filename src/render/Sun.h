#ifndef SUN_Z7QDXCRK
#define SUN_Z7QDXCRK

#include "render/Shader.h"
#include "render/Camera.h"
#include "utils/glm.h"

// false value
#define EARTH_SUN (8.0)
// real value
//#define EARTH_SUN (23400.0)
//#define EARTH_SUN (110)

class Sun
{
	public:
		Sun(glm::vec3 position, float size, float color);

		void draw(Camera& c);
		void drawGlow(Camera& c);
		void setPosition(glm::vec3 position);

		glm::vec3 getPosition(void);

	private:
		ShaderProgram &shader;
		ShaderProgram &shaderGlow;
		
		glm::vec3 position, primaryColor, secondaryColor;
		float spectrum;
		float time;
		float size;
		int lod;

};

#endif /* end of include guard: SUN_Z7QDXCRK */
