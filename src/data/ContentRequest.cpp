#include "data/ContentRequest.h"

//PlanetElevationRequest stuff
PlanetElevationRequest::PlanetElevationRequest(Planet& p, PlanetFace& pf, glm::vec3 c):
	planet(p),
	coord(c)
{
	face=pf.getTptr();
	face->grab();
}

PlanetElevationRequest::~PlanetElevationRequest()
{}

void PlanetElevationRequest::process(void)
{
	// elevation=1.0+glm::simplex(glm::normalize(coord))*0.1f;
	elevation=1.0+fabs(glm::simplex(glm::normalize(coord*1000.0f)))*0.1f+fabs(glm::simplex(glm::normalize(coord*100.0f)))*0.01f+glm::simplex(glm::normalize(coord*10.0f))*0.001;
}

void PlanetElevationRequest::update(void)
{
	face->getPointer()->updateElevation(elevation);
	face->release();
}

//WorldChunkRequest stuff
WorldChunkRequest::WorldChunkRequest(Planet& p, Chunk& c, glm::vec3 o, glm::vec3 e, int x, int y, int z):
	planet(p),
	origin(o),
	end(e),
	px(x),
	py(y),
	pz(z)
{
	chunk=c.getTptr();
	chunk->grab();
}

WorldChunkRequest::~WorldChunkRequest()
{}

void WorldChunkRequest::process(void)
{
	//TEMP
	for(int i=0;i<CHUNK_N;i++)
	{
		for(int k=0;k<CHUNK_N;k++)
		{
			int h=(int)((glm::simplex(glm::vec2(px*CHUNK_N+i,pz*CHUNK_N+k)*0.01f)+1.0f)*32);
			for(int j=0;j<CHUNK_N;j++)
			{
				if(py*CHUNK_N+j<=h)data[i][j][k]=1;
				else data[i][j][k]=0;
			}
		}
	}
}

void WorldChunkRequest::update(void)
{
	chunk->getPointer()->updateData(data);
	chunk->release();
}
