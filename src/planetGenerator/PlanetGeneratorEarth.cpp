#include "PlanetGeneratorEarth.h"
#include "PlanetInfo.h" 
#include "MiniWorld.h"
#include "utils/dbg.h"
#include "utils/positionMath.h"
#include "noise/CaveGenerator.h" // XXX debug
#include "noise/Tree.h"
#include <list>

// XXX temp
extern CaveGenerator caves;
using namespace std;
using namespace glm;

PlanetGeneratorEarth::PlanetGeneratorEarth(int nbThread):
	PlanetGenerator(nbThread),
	treePool(20)
{
	for (unsigned int i = 0; i < treePool.size(); i++)
	{
		treePool[i].generate(i);
		treePool[i].generateList();
	}
}

void PlanetGeneratorEarth::initGenerators()
{
	for(int i=0;i<nbThread;++i)
		generators.push_back(new PlanetNoiseGenerator(*((PlanetInfoEarth*)planetInfo)));
}

PlanetGeneratorEarth::~PlanetGeneratorEarth()
{
}

void PlanetGeneratorEarth::setPlanetInfo(PlanetInfo* p)
{
	planetInfo = p;
	initGenerators();
}

void PlanetGeneratorEarth::generateWorldData(int threadId,
			chunkVal* data,
			int w, int h, int d, // array sizes (in chunks)
			int px, int py, int pz, // offset in world
			glm::vec3 origin, glm::vec3 v1, glm::vec3 v2) // toplevelCharacteristic
{
	/*
		i CHUNK_N
		j CHUNK_N
		k CHUNK_N
		px w
		py h
		pz d
	*/
	int pxPos,pzPos,xPos,zPos,pyPos,yPos;
	pxPos=0;

	caves.generate();

	//unsigned int treeIndex = threadId;

	
	// génération de la map des hauteurs
	const int offset = 10;
	const int heightMapX = MINIWORLD_W*CHUNK_N+2*offset;
	const int heightMapZ = MINIWORLD_D*CHUNK_N+2*offset;
	int          heightMap[heightMapX][heightMapZ];
	blockTypes::T  tileMap[heightMapX][heightMapZ];

	for(int x=0;x<heightMapX;++x)
	{
		for(int z=0;z<heightMapZ;++z)
		{
			const glm::vec3 pos=origin+((v1*float(px+x-offset))+(v2*float(pz+z-offset)))/float(planetInfo->numBlocks);
			const auto blockReponse=getCharacteristic(threadId, pos);
			heightMap[x][z] = elevationToBlockHeight(blockReponse.elevation, planetInfo->numBlocks);
			tileMap[x][z]   = blockReponse.tile;
		}
	}

	list<i32vec3> treePositions;

	// génération des blocs
	for(int cx=0;cx<w;cx++)
	{
		pzPos=pxPos;
		const int vx=cx*(CHUNK_N+2);
		for(int cz=0;cz<d;cz++)
		{
			xPos=pzPos;
			const int vz=cz*(CHUNK_N+2);
			for(int i=0;i<(CHUNK_N+2);i++)
			{
				zPos=xPos;
				for(int k=0;k<(CHUNK_N+2);k++)
				{
					pyPos=zPos;
					
					const auto height=heightMap[i+(CHUNK_N)*cx+offset][k+(CHUNK_N)*cz+offset];
					const auto tile  =tileMap[i+(CHUNK_N)*cx+offset][k+(CHUNK_N)*cz+offset];

					int rx = (px + i+(CHUNK_N)*cx);
					int ry = (pz + k+(CHUNK_N)*cz);
					int randomSource = ((rx+ry)^rx + 10)^ry;

					// ajout des arbres
					if (
							randomSource % 140 == 0 and
							tile == blockTypes::grass
					)
					treePositions.push_back(glm::i32vec3(i+(CHUNK_N)*cx,height,k+(CHUNK_N)*cz));


					//TEMP (pour tester)
					const int waterHeight=planetInfo->waterLevelBlock;
					int caveHeightMin;
					int caveHeightMax;
					if(height<waterHeight)
					{
						caveHeightMax = height-25;
						caveHeightMin = caveHeightMax-100;
						//UNDER THE SEAAAAAA
						for(int cy=0;cy<h;cy++)
						{
							yPos=pyPos;
							const int vy=cy*CHUNK_N;
							for(int j=0;j<(CHUNK_N+2);j++)
							{
								if (vy+py+j <= height) data[yPos]=blockTypes::sand;
								else if (vy+py+j <= waterHeight) data[yPos]=blockTypes::water;
								else data[yPos]=blockTypes::air;
								yPos+=(CHUNK_N+2);
							}
							pyPos+=(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*w;
						}
					}else{
						caveHeightMax = height+50;
						caveHeightMin = caveHeightMax-100;
						for(int cy=0;cy<h;cy++)
						{
							yPos=pyPos;
							const int vy=cy*CHUNK_N;
							for(int j=0;j<(CHUNK_N+2);j++)
							{
								if (vy+py+j == height) data[yPos]=tile;
								else if (vy+py+j < height) data[yPos] = blockTypes::dirt;
								else if (vy+py+j == height+1 && rand()%100 == 1) data[yPos]=blockTypes::flower_red;
								else data[yPos]=blockTypes::air;

								yPos+=(CHUNK_N+2);
							}
							pyPos+=(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*w;
						}
					}
					// cave
					auto holes = caves.getHolesList(px+i+cx*(CHUNK_N),pz+k+cz*(CHUNK_N));
					for(auto it = holes.begin(); it!=holes.end();++it)
					{
						if (it->first  < caveHeightMin) continue;
						if (it->second > caveHeightMax) break;

						for(int i=it->first;i<=it->second;++i)
						{
							int y = (i % CHUNK_N)+1;
							int cy = i / CHUNK_N;
							data[zPos+y*(CHUNK_N+2)+cy*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*w] = blockTypes::air;
							
							if ( y == (CHUNK_N) and cy != (MINIWORLD_H-1))
							{
								data[zPos+(0)*(CHUNK_N+2)+(cy+1)*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*w] = blockTypes::air;
							}
							if ( y==1 and cy!=0)
							{
								data[zPos+(CHUNK_N+1)*(CHUNK_N+2)+(cy-1)*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*w] = blockTypes::air;
							}
						}
					}
					zPos+=(CHUNK_N+2)*(CHUNK_N+2);
				}
				xPos+=1;
			}
			pzPos+=(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*w*h;
		}
		pxPos+=(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*1;
	}

	// list positions ou poser des arbres
	for(auto tPos = treePositions.begin(); tPos != treePositions.end(); ++tPos)
	{
		Tree& t = treePool[0];
		auto& tList = t.generateList();

		for( auto it = tList.begin(); it!=tList.end(); ++it)
		{
			const glm::i32vec3& v = it->first;

			putBlock(data,
					tPos->x+v.x,
					tPos->y+v.y,
					tPos->z+v.z,
					it->second);
		}
	}
	
	//for(int x=0;x<16;++x)
	//for(int y=0;y<16;++y)
	//for(int z=0;z<100;++z)
	//{
		//putBlock(data,x,y,z,blockTypes::snow);
	//}
	
}

inline double max(double x, double y)
{
	return (x>y)? x : y;
}

inline float min(float a, float b)
{
	return (a<b)?a:b;
}

PlanetGeneratorResponse PlanetGeneratorEarth::getCharacteristic(int threadId, const glm::vec3& pos)
{
	glm::vec3 posn = glm::normalize(pos);
	float temperature = getTemperature(posn);
	float humidity = getHumidity(posn);
	float elevation = getElevation(threadId,posn);
	double block_height=delevationToBlockHeight(elevation, planetInfo->numBlocks);

	if (block_height>planetInfo->waterLevelBlock) //  terre
	{
		// e > 0
		float e = (elevation - 1.001) * 1000.f ;

		float sandCoef  = 3.0*temperature + 0.4*e - humidity;
		float snowCoef  = -2.0*temperature+ 1.4*e + 0.3*humidity;
		float stoneCoef = snowCoef+0.01;
		float grassCoef = 0.5 + 0.5*abs(humidity);

		// inihibition
		// (pas de sable près de l'eau)
		//if (e<0.05) sandCoef = 0.0;
		// (A partir d'un moment la neige recouvre les cailloux
		stoneCoef = min(stoneCoef,1.0);


	
		// on choisit le plus grand
		int imax=0;int vmax=sandCoef;
		if (vmax<snowCoef) {vmax=snowCoef;imax=1;}
		if (vmax<stoneCoef) {vmax=stoneCoef;imax=2;}
		if (vmax<grassCoef) {vmax=grassCoef;imax=3;}
		
		
		switch(imax)
		{
			case 0: return {elevation,blockTypes::sand};
			case 1: return {elevation,blockTypes::snow};
			case 2: return {elevation,blockTypes::stone};
			case 3: return {elevation,blockTypes::grass};
			default : return {1.0,blockTypes::grass};
		}

	}else{
		// mer
		return {elevation,blockTypes::sand};
	}
}

float PlanetGeneratorEarth::getElevation(int threadId, const glm::vec3 &coord)
{
	float elevation = (generators[threadId]->getElevation(glm::normalize(coord))+1.0)/2.0f;
	return blockHeightToElevation(elevation*float(CHUNK_N*MINIWORLD_H), planetInfo->numBlocks);
}

float PlanetGeneratorEarth::getTemperature(const glm::vec3& pos)
{
	float distanceToEquatorFactor = 1.0-2.0*fabs(
			glm::dot(
				glm::normalize(pos),
				glm::normalize(planetInfo->axis)
		 ));
	float noise1 = glm::simplex(pos*10.f) * 0.1;
	float noise2 = glm::simplex(pos*100.f) * 0.01;

	return glm::clamp(distanceToEquatorFactor+noise1+noise2,-1.f,1.f);
}

float PlanetGeneratorEarth::getHumidity(const glm::vec3& pos)
{
	float noise = glm::simplex(pos*2.f) *  1.4;
	
	return glm::clamp(noise,-1.f,1.f);
}
