#include "data/ContentRequest.h"
#include "MiniWorld.h"

#include <cstdio>

using namespace glm;

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

//temp
float getElevation(glm::vec3 v)
{
	glm::vec3 c=glm::normalize(v);
	return 1.0+(fabs(glm::simplex(c))+fabs(glm::simplex(c*100.0f+glm::vec3(1.0,0.0,0.0)))*0.01f)*0.0001;
}

void PlanetElevationRequest::process(void)
{
	// elevation=1.0+glm::simplex(glm::normalize(coord))*0.1f;
	elevation=getElevation(coord);
}

void PlanetElevationRequest::update(void)
{
	face->getPointer()->updateElevation(elevation);
	face->release();
}

//WorldChunkRequest stuff
WorldChunkRequest::WorldChunkRequest(Planet& p, Chunk& c, float elevation, glm::vec3 o, glm::vec3 v1, glm::vec3 v2, int x, int y, int z):
	planet(p),
	origin(o),
	elevation(elevation),
	v1(v1),
	v2(v2),
	px(x),
	py(y),
	pz(z)
{
	chunk=c.getTptr();
	chunk->grab();
}

WorldChunkRequest::~WorldChunkRequest()
{}

//TEMP ! faire un vrai système de blocs

const glm::vec2 topCoord[]={glm::vec2(0,0)/16.0f,glm::vec2(0,0)/16.0f};
const glm::vec2 sideCoord[]={glm::vec2(3,0)/16.0f,glm::vec2(2,0)/16.0f};

void WorldChunkRequest::computeChunk(void)
{
    vArray.clear();

	// X
	for(int y=0;y<CHUNK_N;++y)
	for(int z=0;z<CHUNK_N;++z)
	for(int x=1;x<CHUNK_N;++x)
	{
		if(data[x][y][z])
		{
			if (!data[x-1][y][z])
			{
				GL_Vertex v;
				v.facedir=2;
				v.texcoord=sideCoord[int(data[x][y][z]-1)];
				v.position=vec3(px+x,py+y,pz+z);
				vArray.push_back(v);
			}
		}else{
			if (data[x-1][y][z])
			{
				GL_Vertex v;
				v.facedir=3;
				v.texcoord=sideCoord[int(data[x-1][y][z]-1)];
				v.position=vec3(px+x-1,py+y,pz+z);
				vArray.push_back(v);
			}
		}
	}

	// Y
	for(int x=0;x<CHUNK_N;++x)
	for(int z=0;z<CHUNK_N;++z)
	for(int y=1;y<CHUNK_N;++y)
	{
		if(data[x][y][z])
		{
			if (!data[x][y-1][z])
			{
				GL_Vertex v;
				v.facedir=0;
				v.texcoord=topCoord[int(data[x][y][z]-1)];
				v.position=vec3(px+x,py+y,pz+z);
				vArray.push_back(v);
			}
		}else{
			if (data[x][y-1][z])
			{
				GL_Vertex v;
				v.facedir=1;
				v.texcoord=topCoord[int(data[x][y-1][z]-1)];
				v.position=vec3(px+x,py+y-1,pz+z);
				vArray.push_back(v);
			}
		}
	}

	// Z
	for(int x=0;x<CHUNK_N;++x)
	for(int y=0;y<CHUNK_N;++y)
	for(int z=1;z<CHUNK_N;++z)
	{
		if(data[x][y][z])
		{
			if (!data[x][y][z-1])
			{
				GL_Vertex v;
				v.facedir=4;
				v.texcoord=sideCoord[int(data[x][y][z]-1)];
				v.position=vec3(px+x,py+y,pz+z);
				vArray.push_back(v);
			}
		}else{
			if (data[x][y][z-1])
			{
				GL_Vertex v;
				v.facedir=5;
				v.texcoord=sideCoord[int(data[x][y][z-1]-1)];
				v.position=vec3(px+x,py+y,pz-1+z);
				vArray.push_back(v);
			}
		}
	}
}

void WorldChunkRequest::process(void)
{
	//TEMP
	for(int i=0;i<CHUNK_N;i++)
	{
		for(int k=0;k<CHUNK_N;k++)
		{
			// const int h=(int)((glm::simplex(glm::vec2(px*CHUNK_N+i,pz*CHUNK_N+k)*0.01f)+1.0f)*32);
			// const glm::vec3 v=glm::normalize(origin+(float(px+i)/MINIWORLD_N)*(v1)+(float(pz+k)/MINIWORLD_N)*(v2))*1000.0f;
			// const int h=(int)((glm::simplex(v)+1.0f)*32);
			const glm::vec3 pos=origin+((v1*float(px+i))+(v2*float(pz+k)))/float(PLANETFACE_BLOCKS);
			const float val=((getElevation(pos)+(glm::simplex(pos*10000.0f)+1.0f)*0.1f)-1.0f)*10.0f;
			const int h=(val)*16;
			// printf("%f\n",val);
			// const int h=(int)((getElevation(v)-elevation)*320);
			// printf("h %f\n",f);
			// const int h=CHUNK_N/2;
			// const int h=2;
			for(int j=0;j<CHUNK_N;j++)
			{
				if(py*CHUNK_N+j==h)data[i][j][k]=1;
				else if(py*CHUNK_N+j<h)data[i][j][k]=2;
				else data[i][j][k]=0;
			}
		}
	}

	computeChunk();
}

void WorldChunkRequest::update(void)
{
	chunk->getPointer()->updateData(data, vArray);
	chunk->release();
}
