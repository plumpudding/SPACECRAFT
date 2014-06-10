#include "noise/CaveGenerator.h"
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include "utils/dbg.h"
#include "utils/maths.h"

CaveGenerator::CaveGenerator() :
	seed(0),
	segmentCount(50),
	segmentLength(3),
	twistiness(4.f/256.f)
{
	posNoise.SetSeed(seed);
	// TODO noise for positions

	for (int i = 0; i < 3; i++)
	{
		rotNoise[i].SetSeed(seed+i);
		rotNoise[i].SetFrequency(1.0);
		rotNoise[i].SetLacunarity(2.375);
		rotNoise[i].SetOctaveCount(3);
		rotNoise[i].SetPersistence(0.5);
		rotNoise[i].SetNoiseQuality(noise::QUALITY_STD);
	}

	// fill every block by default
	for (uint32_t i = 0; i < CAVE_BLOCK_SIZE; ++i)
		blocks[i] = true;
}

// helper to transform angle from [-1, 1] to [-2π, 2π]
#define NOISE2RAD(X) (static_cast<float>((X) * 2.f * (M_PI)))
// helper [0, 1, 2] -> [x, y, z](axes)
static const glm::vec3 axes[] = {
	{1.f, 0.f, 0.f},
	{0.f, 1.f, 0.f},
	{0.f, 0.f, 1.f}
};

void CaveGenerator::generate()
{
	// first generate the points where the worms start
	// we are gong to push back every control point
	// that means there's at max segmentCount points in each vector
	std::vector<std::vector<glm::vec3> > conPoints(4); // XXX size must be changed
	conPoints[0].push_back(glm::vec3(16, 16+32*30, 16));
	conPoints[1].push_back(glm::vec3(16, 16+32*20, 16));
	conPoints[2].push_back(glm::vec3(16, 16+32*10, 16));
	conPoints[3].push_back(glm::vec3(16, 16+32*0, 16));
	float scale = 0.03;
	float yy = 0.f,
		  zz = 0.f; // used to get the noise
	for (auto points(conPoints.begin()); points != conPoints.end(); ++points) // for every starting point
	{
		for (int i = 1; i < segmentCount; i++, yy += 1.f) // first one is fixed
		{
			glm::vec3 rot;
			glm::quat qq;
			// z should be almost constant so that the cave goes down to the center of the world
			for (int k = 0; k < 3; k++)
			{
				rot[k] = rotNoise[k].GetValue((i * twistiness), yy*scale, zz);
				qq = glm::rotate(qq, NOISE2RAD(rot[k])/i, axes[k]);
				//debug("rot for %d(%f, %f, %f): %d(%f)", k, i*twistiness, yy*scale, zz, (int)RAD2DEG(rot[k]), rot[k]);
			}

			glm::vec3 v(1*i*segmentLength, 0, 0); // basic vector scaled with the segmentCount

			glm::vec3 res = (*points)[0] + glm::mat3_cast(qq)*v;
			// debug("res: %s", glm::to_string(res).c_str());

			// finally add it to the control points
			points->push_back(res);
		}
	}

	// cave in circles around the points
	// We must calculate the normal of the direction
	for (auto points(conPoints.begin()); points != conPoints.end(); ++points) // for every starting point
	{
		for (int i = 1; i < segmentCount; i++, yy += 1.f) // first one is fixed
		{
			glm::vec3 v = (*points)[i] - (*points)[i-1]; // this is the direction at point i-1
			glm::vec3 normal(-v.z, 0.f, v.x); // here is the normal
			digDisk((*points)[i-1], normal, glm::cross(normal, v));
			glm::i32vec3 p1 = glm::i32vec3((*points)[i+1]);
			glm::i32vec3 p2 = glm::i32vec3((*points)[i]);
			for(int i=-4;i<=4;++i)
			for(int j=-4;j<=4;++j)
			for(int k=-4;k<=4;++k)
			{
				glm::i32vec3 decal(i,j,k);
				digLine(p1+decal,p2+decal);
			}
		}
	}

	// XXX test
	//for(int i=0;i<3;++i)
	//for(int j=0;j<3;++j)
	//for(int k=0;k<3;++k)
	//digLine(glm::i32vec3(0+i, 0+j, 0+k), glm::i32vec3(CAVE_CHUNK_SIZE_X-10+i, CAVE_CHUNK_SIZE_Y-10+j, CAVE_CHUNK_SIZE_Y-10+k));
}

// helper to get the 1D index in a flattened 3D array, using a vec3
#define TPOS(V) ((V).x + CAVE_CHUNK_SIZE_X * ((V).y + (V).z * CAVE_CHUNK_SIZE_Y))

// make sure you can access the array with v
static inline bool correct(glm::i32vec3 &v)
{
	bool changed = false;
	if (v.x < 0) v.x = 0, changed = true;
	else if (v.x >= CAVE_CHUNK_SIZE_X) v.x = CAVE_CHUNK_SIZE_X-1, changed = true;
	if (v.y < 0) v.y = 0, changed = true;
	else if (v.y >= CAVE_CHUNK_SIZE_Y) v.y = CAVE_CHUNK_SIZE_Y-1, changed = true;
	if (v.z < 0) v.z = 0, changed = true;
	else if (v.z >= CAVE_CHUNK_SIZE_Z) v.z = CAVE_CHUNK_SIZE_Z-1, changed = true;
	return changed;
}

// cave lines from top to bot
//     ***     1
//   *******   2
//  *********  3
//  *********  4
//   *******   5
//     ***     6

void CaveGenerator::digDisk(const glm::vec3 &pos, const glm::vec3 &normal, const glm::vec3 &dir)
{
	glm::i32vec3 a, b; // left and right of the lines
}

void CaveGenerator::digLine(const glm::i32vec3 &a, const glm::i32vec3 &b)
{
	int i, err_1, err_2;
	glm::i32vec3 pixel,
				 d, // replace the usual dx, dy, dz
				 d2,
				 inc,
				 absinc; // l, m, n

	pixel = a;
	d = b - a;
	for (int i = 0; i < 3; i++)
	{
		inc[i] = (d[i] < 0) ? -1 : 1;
		absinc[i] = ABS(d[i]);
		d2[i] = absinc[i] << 1;
	}

	if ((absinc.x >= absinc.y) && (absinc.x >= absinc.z)) {
		err_1 = d2.y - absinc.x;
		err_2 = d2.z - absinc.x;
		for (i = 0; i < absinc.x; i++) {
			if (correct(pixel))
				return; // ouside ouf the tab
			blocks[TPOS(pixel)] = false; // air
			if (err_1 > 0) {
				pixel[1] += inc.y;
				err_1 -= d2.x;
			}
			if (err_2 > 0) {
				pixel[2] += inc.z;
				err_2 -= d2.x;
			}
			err_1 += d2.y;
			err_2 += d2.z;
			pixel[0] += inc.x;
		}
	} else if ((absinc.y >= absinc.x) && (absinc.y >= absinc.z)) {
		err_1 = d2.x - absinc.y;
		err_2 = d2.z - absinc.y;
		for (i = 0; i < absinc.y; i++) {
			if (correct(pixel))
				return; // ouside ouf the tab
			blocks[TPOS(pixel)] = false; // air
			if (err_1 > 0) {
				pixel[0] += inc.x;
				err_1 -= d2.y;
			}
			if (err_2 > 0) {
				pixel[2] += inc.z;
				err_2 -= d2.y;
			}
			err_1 += d2.x;
			err_2 += d2.z;
			pixel[1] += inc.y;
		}
	} else {
		err_1 = d2.y - absinc.z;
		err_2 = d2.x - absinc.z;
		for (i = 0; i < absinc.z; i++) {
			if (correct(pixel))
				return; // ouside ouf the tab
			blocks[TPOS(pixel)] = false; // air
			if (err_1 > 0) {
				pixel[1] += inc.y;
				err_1 -= d2.z;
			}
			if (err_2 > 0) {
				pixel[0] += inc.x;
				err_2 -= d2.z;
			}
			err_1 += d2.y;
			err_2 += d2.x;
			pixel[2] += inc.z;
		}
	}
	if (correct(pixel))
		return; // ouside ouf the tab
	blocks[TPOS(pixel)] = false; // air
}

CaveGenerator::~CaveGenerator()
{}
