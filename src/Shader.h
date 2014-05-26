#ifndef SHADER_F8X43H2W
#define SHADER_F8X43H2W

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <map>

class Shader;
class ShaderProgram;

namespace ShaderType
{
    enum T {
        Vertex,
        Fragment,
        Geometry
    };
};

class ShaderProgram
{
    public:
        // constructeur
        static ShaderProgram& loadFromFile(const char* vertexShader, const char* fragmentShader);
        static ShaderProgram& loadFromShader(Shader& vertexShader, Shader& fragmentShader);

        // Fournit l'identifiant du program
        GLuint getHandle();

        // Fournit localisation d'un uniform.
        GLuint uniform(const char* name);

        // parametrer le ShaderProgram
        void setUniform(const char *name,float x,float y,float z);
        void setUniform(const char *name, const glm::vec3 & v);
        void setUniform(const char *name, const glm::vec4 & v);
        void setUniform(const char *name, const glm::mat4 & m);
        void setUniform(const char *name, const glm::mat3 & m);
        void setUniform(const char *name, float val );
        void setUniform(const char *name, int val );
        void setUniform(const char *name, bool val );

        // parametrer les attributs
        //void bindAttribLocation( GLuint location,const char * name);
        //void bindFragDataLocation( GLuint location,const char * name );
    private:
        // constructeur (privé) . Utiliser loadFrom*
        ShaderProgram();

        // identifiant OpenGL
        GLuint handle;

        // identifiants des attributs.
        std::map<std::string,GLuint> uniformsMap;
};

class Shader
{
    public:
        // Chargement du shader depuis un fichier
        static Shader& loadFromFile(const char *filename,ShaderType::T type);


        // Fournit l'identifiant du shader
        GLuint getHandle();
        

    private:
        // constructeur privé, passer par Shader::fromFile(filename)
        Shader();

        // identifiant du shader OpenGL
        GLuint handle;

        // constructeur par copie (private)
        Shader(const Shader& shader);
};

#endif /* end of include guard: SHADER_F8X43H2W */
