#include "Galaxy.h"
#include <list>
#include "utils/dbg.h"
#include "SolarSystem.h"
#include "render/Shader.h"


using namespace glm;
using namespace std;

/////////////////////////////////////////////////////////
// paramètres de la galaxie
const double GALAXY_WIDTH = 1000.0;
const dvec3 GALAXY_CENTER(0.0,0.0,0.0);
const double SOLARSYSTEM_MIN_DISTANCE = 0.1; 

/////////////////////////////////////////////////////////

Galaxy::Galaxy():
	galaxyTree(NULL),
	vbo(0),
	vao(0),
	program(ShaderProgram::loadFromFile(
		"shader/galaxy/galaxy.vert",
		"shader/galaxy/galaxy.frag",
		"galaxy")),
	isVBOGenerated(false)
{
	
}

void Galaxy::pushSolarSystem(SolarSystem* s)
{
	solarPosition.push_back(vec3(s->getPosition()));
	if (galaxyTree)
	{
		galaxyTree->pushSolarSystem(s);
	}
	else
	{
		galaxyTree = new GalaxyTree(s,GALAXY_CENTER, GALAXY_WIDTH);
	}
}


GalaxySolarResponse Galaxy::getClosestSolarSystem(const dvec3& pos , double maxDist)
{
	if (galaxyTree)
		return galaxyTree->getClosestSolarSystem(pos,maxDist);
	else
		return {NULL,2.0*maxDist};
}

void Galaxy::generateVBO()
{
	isVBOGenerated = true;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*solarPosition.size(), &solarPosition[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindFragDataLocation(program.getHandle(), 0, "outColor");
	program.setAttribute("position", 3, GL_FALSE, 0, 0);

	program.setBuffers(vao, vbo, 0);
	program.use();

}

void Galaxy::draw(Camera& camera)
{
	if (not isVBOGenerated) generateVBO();
    program.use();

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    camera.updateCamera(program);

    glDrawArrays(GL_POINTS, 0, solarPosition.size());
}

////////////////////////////////////////////////////////

GalaxyTree::GalaxyTree(SolarSystem* s, const glm::dvec3& c, double w):
	isSubdivised(false),
	solarSystem(s),
	center(c),
	width(w)
{
	
}

dvec3 cubeDecalage[2][2][2]=
{
	{
		{
			dvec3(-1.0,-1.0,-1.0),
			dvec3(-1.0,-1.0,+1.0),
		},
		{
			dvec3(-1.0,+1.0,-1.0),
			dvec3(-1.0,+1.0,+1.0),
		}
	},
	{
		{
			dvec3(+1.0,-1.0,-1.0),
			dvec3(+1.0,-1.0,+1.0),
		},
		{
			dvec3(+1.0,+1.0,-1.0),
			dvec3(+1.0,+1.0,+1.0),
		}
	},
};

void GalaxyTree::pushSolarSystem(SolarSystem* s)
{
	// sauvegarde du solarSystem si il est présent
	SolarSystem* previousSolarSystem = NULL;
	if (not isSubdivised)
	{
		previousSolarSystem = solarSystem;

		// suppression des systèmes solaires trop proches
		if (glm::distance(solarSystem->getPosition(),s->getPosition()) < SOLARSYSTEM_MIN_DISTANCE)
		{
			previousSolarSystem->deleteSolarSystem();
			previousSolarSystem = NULL;
		}

		isSubdivised = true;

		children[0][0][0]=NULL;
		children[0][0][1]=NULL;
		children[0][1][0]=NULL;
		children[0][1][1]=NULL;
		children[1][0][0]=NULL;
		children[1][0][1]=NULL;
		children[1][1][0]=NULL;
		children[1][1][1]=NULL;
	}

	// calcul dans quel branche poser s
	const dvec3 sPos = s->getPosition();
	int xx = (sPos.x>center.x)? 1 : 0;
	int yy = (sPos.y>center.y)? 1 : 0;
	int zz = (sPos.z>center.z)? 1 : 0;

	// insertion
	if (children[xx][yy][zz])
	{
		children[xx][yy][zz]->pushSolarSystem(s);
	}
	else
	{
		dvec3 c = center + cubeDecalage[xx][yy][zz]*width*0.5;
		children[xx][yy][zz] =
			new GalaxyTree(
					s,
					c,
					width*0.5
			);
	}

	
	// réinsertion du précédent solar system si il est là
	if (previousSolarSystem)
		pushSolarSystem(previousSolarSystem);
}

GalaxySolarResponse GalaxyTree::getClosestSolarSystem(const glm::dvec3& pos, double maxDist)
{
	if (isSubdivised)
	{
		dvec3 p = pos - center;
		bool xValid[2], yValid[2], zValid[2];

		xValid[0] = (p.x + maxDist > 0.0);
		xValid[1] = (p.x - maxDist > 0.0);
		yValid[0] = (p.y + maxDist > 0.0);
		yValid[1] = (p.y - maxDist > 0.0);
		zValid[0] = (p.z + maxDist > 0.0);
		zValid[1] = (p.z - maxDist > 0.0);
		list<GalaxySolarResponse> l;
		for(int x=0;x<2;++x)
		for(int y=0;y<2;++y)
		for(int z=0;z<2;++z)
		{
			if (children[x][y][z] and xValid[x] and yValid[y] and zValid[z])
			{
				l.push_back(children[x][y][z]->getClosestSolarSystem(pos,maxDist));
			}
		}
		l.sort();
		return l.front(); // TODO protection (assert)
	}
	else
	{
		return {solarSystem,glm::distance(solarSystem->getPosition(),pos)};
	}
}


//////////////////////////////////////////////////////////////////////////////

bool GalaxySolarResponse::operator<(const GalaxySolarResponse& other) const
{
	return distance<other.distance;
}

