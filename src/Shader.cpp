#include "Shader.h"
#include <vector>
#include <fstream>
#include <iostream>
#include "utils/dbg.h"

using namespace std;

// collection des shader déja créés.
map<string,Shader*> shaderMap;
// collection des programmes déjà créés.
map< pair<GLuint,GLuint> ,ShaderProgram*> shaderProgramMap;

// Lecture d'un fichier
bool getFileContents(const char *filename, vector<char>& buffer)
{
    ifstream file(filename, ios_base::binary);
    if (file)
    {
        file.seekg(0, ios_base::end);
        streamsize size = file.tellg();
        if (size > 0)
        {
            file.seekg(0, ios_base::beg);
            buffer.resize(static_cast<size_t>(size));
            file.read(&buffer[0], size);
        }
        buffer.push_back('\0');
        return true;
    }
    else
    {
        return false;
    }
}

// chargement d'un shader depuis un fichier
Shader& Shader::loadFromFile(const char* filename, ShaderType::T type)
{
    // test si le shader est déja chargé en mémoire.
    auto it = shaderMap.find(string(filename));
    if (it!=shaderMap.end())
        return *(it->second);

    // Sinon, on le charge
    Shader* s = new Shader();

    // chargement du fichier
    vector<char> fileContent;
    if (not getFileContents(filename,fileContent))
    {
        log_err("[Erreur] Fichier %s  introuvable.", filename);
        exit(EXIT_FAILURE);
    }

    // creation
    s->handle = glCreateShader(type);
    
    // assignation du code source
    glShaderSource(s->handle,1, (const GLchar**)&(fileContent[0]), NULL);

    // compilation
    glCompileShader(s->handle);

    // vérification de la compilation
    GLint compile_status;
    glGetShaderiv(s->handle, GL_COMPILE_STATUS, &compile_status);
    if(compile_status != GL_TRUE)
    {
        /* error text retreiving*/
        int logsize;
        glGetShaderiv(s->handle, GL_INFO_LOG_LENGTH, &logsize);
         
        char* log = new char[logsize+1];
        glGetShaderInfoLog(s->handle, logsize, &logsize, log);
        log[logsize]='\0';
         
        cerr<<"[erreur] Impossible de compiler le shader : "<<filename<<endl;
        cerr<<"============[Erreur]================="<<endl;
        cerr<<log<<endl;
        cerr<<"============[Erreur]================="<<endl;
        
        exit(EXIT_FAILURE);
    }

    // ajout du shader dans la map
    shaderMap[string(filename)]=s;

    return *s;
}

Shader::Shader()
{

}

GLuint Shader::getHandle()
{
    return handle;
}

Shader::Shader(const Shader& shader)
{
    handle = shader.handle;
}


ShaderProgram& ShaderProgram::loadFromFile(const char* vertexShader, const char* fragmentShader)
{
    return ShaderProgram::loadFromShader(
                Shader::loadFromFile(vertexShader,ShaderType::Vertex),
                Shader::loadFromFile(fragmentShader,ShaderType::Fragment)
            );
}

ShaderProgram& ShaderProgram::loadFromShader(Shader& vertexShader, Shader& fragmentShader)
{
    // Si le program existe déjà on le renvoie
    auto it = shaderProgramMap.find(
                make_pair<GLuint,GLuint>(
                    vertexShader.getHandle(),
                    fragmentShader.getHandle())
            );
    if (it != shaderProgramMap.end())
    {
        return *(it->second);
    }

    // programme creation
    ShaderProgram* p = new ShaderProgram();
	p->handle = glCreateProgram();
    if (not p->handle)
    {
        cerr<<"[Erreur] Impossible de créer un ShaderProgramme vierge"<<endl;
        exit(EXIT_FAILURE);
    }


    // attachement.
    glAttachShader(p->handle,vertexShader.getHandle());
    glAttachShader(p->handle,fragmentShader.getHandle());

    // linkage
    glLinkProgram(p->handle);
    GLint result;
    glGetProgramiv(p->handle,GL_LINK_STATUS, &result);
    if (result!=GL_TRUE)
    {
        cerr<<"[Erreur] Impossible de linker les shader";
        exit(EXIT_FAILURE);
    }
    return *p;
}

GLint ShaderProgram::uniform(const char* name)
{
    auto it = uniformsMap.find(name);
    if (it == uniformsMap.end())
    {
        // uniforme non référencé
        GLuint r = glGetUniformLocation(handle, name); 
        uniformsMap[name] = r;
        if ( r == GL_INVALID_OPERATION )
            log_err("L'identifiant %s  n'existe pas", name);

        return r;
    }
    else
        return it->second; 
}

ShaderProgram::ShaderProgram(const ShaderProgram& other)
{
    handle = other.handle;
    uniformsMap = other.uniformsMap;
}

void ShaderProgram::setUniform(const char *name,float x,float y,float z)
{
    glUniform3f(uniform(name), x, y, z);
}
void ShaderProgram::setUniform(const char *name, const glm::vec3 & v)
{
    glUniform3fv(uniform(name), 1, glm::value_ptr(v));
}
void ShaderProgram::setUniform(const char *name, const glm::vec4 & v)
{
    glUniform4fv(uniform(name), 1, glm::value_ptr(v));
}
void ShaderProgram::setUniform(const char *name, const glm::mat4 & m)
{
    glUniformMatrix4fv(uniform(name), 1, GL_FALSE, glm::value_ptr(m));
}
void ShaderProgram::setUniform(const char *name, const glm::mat3 & m)
{
    glUniformMatrix3fv(uniform(name), 1, GL_FALSE, glm::value_ptr(m));
}
void ShaderProgram::setUniform(const char *name, float val )
{
    glUniform1f(uniform(name), val);
}
void ShaderProgram::setUniform(const char *name, int val )
{
    glUniform1i(uniform(name), val);
}

ShaderProgram::ShaderProgram()
{
    
}

GLuint ShaderProgram::getHandle()
{
    return handle;
}
