#include "Planet.h"
#include "data/ContentHandler.h"
#include "Application.h"

//0-5-1
//|\ /|
//8-4-6
//|/ \|
//3-7-2

//[0][1]
//[2][3]

PlanetFace::PlanetFace(Planet* planet, glm::vec3 v[4]):
	planet(planet),
	sons({NULL, NULL, NULL, NULL}),
	elevation(1.0f)
{
	vertex[0]=v[0]; vertex[1]=v[1];
	vertex[2]=v[2];	vertex[3]=v[3];
	depth=0;
	finalize();
}

PlanetFace::PlanetFace(Planet* planet, PlanetFace* father, uint8_t id):
	planet(planet),
	father(father),
	sons({NULL, NULL, NULL, NULL}),
	elevation(1.0f)
{
	//TODO : exception ?
	// if(!father);
	switch(id)
	{
		case 0:
			vertex[0]=father->vertex[0];
			vertex[1]=father->vertex[5];
			vertex[2]=father->vertex[4];
			vertex[3]=father->vertex[8];
			break;
		case 1:
			vertex[0]=father->vertex[5];
			vertex[1]=father->vertex[1];
			vertex[2]=father->vertex[6];
			vertex[3]=father->vertex[4];
			break;
		case 2:
			vertex[0]=father->vertex[8];
			vertex[1]=father->vertex[4];
			vertex[2]=father->vertex[7];
			vertex[3]=father->vertex[3];
			break;
		case 3:
			vertex[0]=father->vertex[4];
			vertex[1]=father->vertex[6];
			vertex[2]=father->vertex[2];
			vertex[3]=father->vertex[7];
			break;
		default:
			//TODO : exception ?
			break;
	}
	
	depth=father->depth+1;
	finalize();
}

void PlanetFace::finalize(void)
{
	vertex[4]=(vertex[0]+vertex[1]+vertex[2]+vertex[3])*0.25f;
	vertex[5]=(vertex[0]+vertex[1])*0.5f;
	vertex[6]=(vertex[1]+vertex[2])*0.5f;
	vertex[7]=(vertex[2]+vertex[3])*0.5f;
	vertex[8]=(vertex[3]+vertex[0])*0.5f;

	for(int i=0;i<9;i++)vertex[i]=glm::normalize(vertex[i]);

	planet->handler.requestContent(new PlanetElevationRequest(*planet, *this, vertex[4]));
}

void PlanetFace::updateElevation(float e)
{
	elevation=e;
}

glm::vec3 cubeArray[6][4]=
		{{glm::vec3(-1.0,-1.0,-1.0),glm::vec3(-1.0,-1.0,1.0),glm::vec3(1.0,-1.0,1.0),glm::vec3(1.0,-1.0,-1.0)}, //bottom
		{glm::vec3(1.0,1.0,1.0),glm::vec3(-1.0,1.0,1.0),glm::vec3(-1.0,1.0,-1.0),glm::vec3(1.0,1.0,-1.0)}, //top
		{glm::vec3(-1.0,1.0,1.0),glm::vec3(-1.0,-1.0,1.0),glm::vec3(-1.0,-1.0,-1.0),glm::vec3(-1.0,1.0,-1.0)}, //left
		{glm::vec3(1.0,-1.0,-1.0),glm::vec3(1.0,-1.0,1.0),glm::vec3(1.0,1.0,1.0),glm::vec3(1.0,1.0,-1.0)}, //right
		{glm::vec3(1.0,1.0,-1.0),glm::vec3(-1.0,1.0,-1.0),glm::vec3(-1.0,-1.0,-1.0),glm::vec3(1.0,-1.0,-1.0)}, //near
		{glm::vec3(-1.0,-1.0,1.0),glm::vec3(-1.0,1.0,1.0),glm::vec3(1.0,1.0,1.0),glm::vec3(1.0,-1.0,1.0)}}; //far


static GLfloat vertices[] = {
    //     POSITION    |      COLOR           |     NORMAL
    // x positif
    +0.0 , -0.5 , -0.5 , 1.0 , 0.0 , 0.0 , 1.f, +1.0 , 0.0 , 0.0 ,
    +0.0 , +0.5 , -0.5 , 1.0 , 0.0 , 0.0 , 1.f, +1.0 , 0.0 , 0.0 ,
    +0.0 , +0.5 , +0.5 , 1.0 , 0.0 , 0.0 , 1.f, +1.0 , 0.0 , 0.0 ,
    +0.0 , -0.5 , +0.5 , 1.0 , 0.0 , 0.0 , 1.f, +1.0 , 0.0 , 0.0 ,
};

static GLuint elements[2*3] = {
    0,1,2,      0,2,3, // face 1
};

Planet::Planet(planetInfo_s pi, ContentHandler& ch):
	programBasic(ShaderProgram::loadFromFile("shader/basic/basic.vert", "shader/basic/basic.frag")),
	planetInfo(pi),
	handler(ch)
{
	for(int i=0;i<6;i++)faces[i]=new PlanetFace(this, cubeArray[i]);

	//TEMP pour drawDirect
	    glGenBuffers(1, &vbo);
	    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	    glGenBuffers(1, &ebo);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	    // basic programme configuration
	    {
	        glGenVertexArrays(1, &vaoBasic);
	        glBindVertexArray(vaoBasic);

	        programBasic.use();
	        glBindFragDataLocation(programBasic.getHandle(), 0, "outColor");
	        programBasic.setAttribute("position", 3, GL_FALSE, 10, 0);
	        programBasic.setAttribute("color", 4, GL_FALSE, 10, 3);
	        // programBasic.setAttribute("texcoord", 2, GL_FALSE, 10, 0); // XXX pas de texcoord
	        
	        programBasic.setUniform("overrideColor", glm::vec4(1.f));

	        programBasic.setUniform("model", glm::mat4(1.0f));
	    }
}

void PlanetFace::testFullGeneration(int depth)
{
	if(depth<=0)return;
	for(int i=0;i<4;i++)
	{
		if(!sons[i])sons[i]=new PlanetFace(planet,this,i);
		sons[i]->testFullGeneration(depth-1);
	}
}

void Planet::testFullGeneration(int depth)
{
	for(int i=0;i<6;i++)faces[i]->testFullGeneration(depth);
}

void PlanetFace::drawDirect(void)
{
	glm::quat q(glm::vec3(1.0,0.0,0.0),vertex[4]);
	if(sons[0])
	{
		for(int i=0;i<4;i++)
		{
			if(sons[i])sons[i]->drawDirect();
		}
	}else{
		float v=2.0f/(1<<depth);
	    planet->programBasic.setUniform("model", glm::translate(glm::mat4(1.0f),vertex[4]*elevation)*glm::scale(glm::mat4_cast(q),glm::vec3(v)));
	    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}

void Planet::drawDirect(void)
{
    Camera &cam(Application::getInstance().getCamera());
    // basic program
    {
        programBasic.use();
        glBindVertexArray(vaoBasic);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        cam.updateCamera(programBasic);

        // glm::mat4 model = glm::translate(glm::mat4(1.0),-glm::vec3(-1.0,0.0,0.0));
        // // model = glm::rotate(model,1.0f,glm::vec3(1.0,0.0,1.0));
        // programBasic.setUniform("model", model);

        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    	for(int i=0;i<6;i++)faces[i]->drawDirect();
    }

}