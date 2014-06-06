#include "blockProcessing.h"

//TODO pour computeChunkFaces et generateWorldData : NETTOYER (on peut clairement faire plus jolie et lisible)

// DOCUMENTATION:
//
// Valeurs des bornes:
// 0<=px<w
// 0<=py<h
// 0<=pz<d
// -1<=i<CHUNK_N+1
// -1<=j<CHUNK_N+1
// -1<=k<CHUNK_N+1

using namespace glm;

//TODO : optimiser pour éviter les multiplications à chaque fois
//(juste utiliser un pointeur à chaque fois...)
void computeChunkFaces(chunkVal* data,
		int w, int h, int d, //array sizes (in chunks)
		int sx, int sy, int sz, //chunk in array (in chunks)
		int px, int py, int pz, //chunk offset in world (in blocks)
		std::vector<GL_Vertex>& vArray) //output
{
	vArray.clear();
	chunkVal previous,current;
	// X
	for(int y=0;y<CHUNK_N;++y)
	for(int z=0;z<CHUNK_N;++z)
	{
		previous = accessArray(data,w,h,d,sx,sy,sz,-1,y,z);
		for(int x=0;x<CHUNK_N+1;++x)
		{
			current = accessArray(data,w,h,d,sx,sy,sz,x,y,z);
			if (blockShouldBeFace(current,previous)) {
				GL_Vertex v;
				v.facedir=2;
				v.tile = getBlockID(current,blockPlane::side);
				v.position=vec3(px+x,py+y,pz+z);
				vArray.push_back(v);
			}else if (blockShouldBeFace(previous,current)) {
				GL_Vertex v;
				v.facedir=3;
				v.tile = getBlockID(previous,blockPlane::side);
				v.position=vec3(px+x-1,py+y,pz+z);
				vArray.push_back(v);
			}
			previous=current;
		}
	}

	// Y
	for(int x=0;x<CHUNK_N;++x)
	for(int z=0;z<CHUNK_N;++z)
	{
		previous = accessArray(data,w,h,d,sx,sy,sz,x,-1,z);
		for(int y=0;y<CHUNK_N+1;++y)
		{
			current = accessArray(data,w,h,d,sx,sy,sz,x,y,z);
			if (blockShouldBeFace(current,previous)) {
				GL_Vertex v;
				v.facedir=0;
				v.tile = getBlockID(current,blockPlane::bottom);
				v.position=vec3(px+x,py+y,pz+z);
				vArray.push_back(v);
			} else if (blockShouldBeFace(previous,current)) {
					GL_Vertex v;
					v.facedir=1;
					v.tile = getBlockID(previous,blockPlane::top);
					v.position=vec3(px+x,py+y-1,pz+z);
					vArray.push_back(v);
			}
			previous=current;
		}
	}

	// Z
	for(int x=0;x<CHUNK_N;++x)
	for(int y=0;y<CHUNK_N;++y)
	{
		previous = accessArray(data,w,h,d,sx,sy,sz,x,y,-1);
		for(int z=0;z<CHUNK_N+1;++z)
		{
			current = accessArray(data,w,h,d,sx,sy,sz,x,y,z);
			if (blockShouldBeFace(current,previous)) {
				GL_Vertex v;
				v.facedir=4;
				v.tile = getBlockID(current,blockPlane::side);
				v.position=vec3(px+x,py+y,pz+z);
				vArray.push_back(v);
			}else if (blockShouldBeFace(previous,current)) {
				GL_Vertex v;
				v.facedir=5;
				v.tile = getBlockID(previous,blockPlane::side);
				v.position=vec3(px+x,py+y,pz-1+z);
				vArray.push_back(v);
			}
			previous=current;
		}
	}

	//sprites
	for(int x=0;x<CHUNK_N;++x)
	for(int y=0;y<CHUNK_N;++y)
	for(int z=0;z<CHUNK_N;++z)
	{
		current = accessArray(data,w,h,d,sx,sy,sz,x,y,z);
		if (blockStyleID[current] == blockStyle::sprite)
		{
			GL_Vertex v;
			v.facedir=6;
			v.tile=getBlockID(current,blockPlane::top);
			v.position=vec3(px+x,py+y,pz+z);
			vArray.push_back(v);

			v.facedir=7;
			v.tile=getBlockID(current,blockPlane::top);
			v.position=vec3(px+x,py+y,pz+z);
			vArray.push_back(v);

			v.facedir=8;
			v.tile=getBlockID(current,blockPlane::top);
			v.position=vec3(px+x,py+y,pz+z);
			vArray.push_back(v);

			v.facedir=9;
			v.tile=getBlockID(current,blockPlane::top);
			v.position=vec3(px+x,py+y,pz+z);
			vArray.push_back(v);
		}
	}
}
