#ifndef __TEXTUREMANAGER_H__
#define __TEXTUREMANAGER_H__

#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <map>
#include <string>
#include "utils/Singleton.h"

class TextureManager : public Singleton<TextureManager> {
	private:
		struct tex {
			GLuint id;
			int width, height;
		};
		std::map<std::string, tex> textures;
		friend class Singleton<TextureManager>;
	public:
		~TextureManager();

		GLuint loadTexture(const std::string& filename);
		void getTextureSize(const std::string& filename, int *width, int *height);
		inline void bind(const std::string& filename, GLuint activetexture = GL_TEXTURE0)
		{
			glActiveTexture(activetexture);
			glBindTexture(GL_TEXTURE_2D, loadTexture(filename));
		}
		inline void bind(GLuint tex, GLuint activetexture = 0)
		{
			glActiveTexture(activetexture);
			glBindTexture(GL_TEXTURE_2D, tex);
		}
};


#endif
