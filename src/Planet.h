#ifndef PLANET_H
#define PLANET_H

#include <vector>
#include <list>
#include "utils/glm.h"
#include "utils/TrackerPointer.h"
#include "noise/PlanetGenerator.h"
#include "render/Shader.h"
#include "render/Camera.h"
#include "render/Cloud.h"
#include "render/Atmosphere.h"
#include "world/BlockType.h"
#include "PlanetInfo.h"

#define PLANET_ADDED_DETAIL (4)
// #define PFBH_MAXSIZE (1024*16)
#define PFBH_MAXSIZE (1024*512)

typedef struct
{
	float pos[3];
	float elevation;
	float minElevation;
	float size;
	int topTile;
	int sideTile;
	float repeat;
}faceBufferEntry_s;

class Planet;
class PlanetFace;

// Vertex position
//      0-5-1
//      |\ /|
//      8-4-6
//      |/ \|
//      3-7-2

// Sons position
//      [0][1]
//      [2][3]

class PlanetFaceBufferHandler
{
	public:
		PlanetFaceBufferHandler(PlanetFace& pf, int ms, glm::vec3 v1, glm::vec3 v2);
		~PlanetFaceBufferHandler();
	
		void addFace(PlanetFace* pf);
		void deleteFace(PlanetFace* pf);
		void changeFace(PlanetFace* pf, int i);
		void draw(Camera& c, glm::vec3 lightdir);

	private:
		ShaderProgram &shader;
		PlanetFace& planetFace;
		std::vector<PlanetFace*> faces;
		faceBufferEntry_s* buffer;
		int maxSize, curSize;
		GLuint vbo, vao;
		glm::vec3 v1, v2;
};

class PlanetFace
{
	friend class PlanetFaceBufferHandler;
	friend class MiniWorld;
	friend class Chunk;
	public:
		PlanetFace(Planet* planet, glm::vec3 v[4]);
		PlanetFace(Planet* planet, PlanetFace* father, uint8_t id);
		~PlanetFace();
		
		void deletePlanetFace(PlanetFaceBufferHandler* b);
		// mise a jour de l'elevation, de la température et de l'humidité
		void updateElevation(float e, float t, float h);
		bool shouldHaveMiniworld(Camera& c);
		bool isDetailedEnough(Camera& c);
		void processLevelOfDetail(Camera& c, PlanetFaceBufferHandler* b);
		void createMiniWorld(void);
		void removeMiniWorld(void);

		TrackerPointer<PlanetFace>* getTptr(void);

		//TEMP
		void testFullGeneration(int depth, PlanetFaceBufferHandler* b);

	private:
		void finalize(void);

		Planet* planet;
		PlanetFace* father; //father == NULL <=> toplevel face
		PlanetFace* toplevel;
		PlanetFace* sons[4];

		class MiniWorld* miniworld;


		glm::vec3 vertex[9];
		glm::vec3 uvertex[9];

		TrackerPointer<PlanetFace>* tptr;

		bool elevated;
		int x, z;
		int bufferID;
		float elevation;
		float temperature;
		float humidity;
		float minElevation;
		
		uint8_t id;
		int depth;
		int childrenDepth;
		inline bool isDrawingFace() {return bufferID>=0;}
		bool isDisplayOk;
};

class Planet
{
	friend class PlanetFaceBufferHandler;
	friend class PlanetFace;
	friend class Chunk;
	public:
		Planet(PlanetInfo &pi, class ContentHandler& ch);
		~Planet(); // TODO faire tous les free
		
		void processLevelOfDetail(Camera& c);
		void draw(Camera& c);
		void update(float time);

		int numMiniWorlds(void);
		void addMiniWorld(MiniWorld* mw);
		void removeMiniWorld(MiniWorld* mw);

		bool collidePoint(glm::dvec3 p, glm::dvec3 v, glm::dvec3& out);
		class Chunk* selectBlock(glm::dvec3 p, glm::dvec3 v, glm::i32vec3& out, int& dir);

		void changeBlock(glm::i32vec3 p, blockTypes::T v);
		void deleteBlock(glm::i32vec3 p);

		const PlanetInfo planetInfo; //read only
		class ContentHandler& handler;

		glm::dvec3 getGravityVector(glm::dvec3 p);
		glm::vec3 getPosition(void);
		glm::mat3 getModel(void);
		glm::vec3 getCameraRelativePosition(Camera& c);
		glm::dvec3 getCameraRelativeDoublePosition(Camera& c);
		inline float getElevation(int id, const glm::vec3 &coord)
		{
			return generators[id]->getElevation(coord);
		}
		
		void setSunPosition(glm::vec3 p);

		//TEMP
		void testFullGeneration(int depth, PlanetFaceBufferHandler* b);

		// Donne la température en fonction de la position
		// Prend en compte
		// 		-l'axe de rotation de la planete
		//		-perturbation local par un bruit.
		// 		-la distance de la planete par rapport au soleil (TODO)
		//		-valeur intrinsèque de la planete (composition) (TODO)
		// Le résultat est à valeur dans [-1,1]
		// La position est une position dans le référentiel de la planète.
		float getTemperature(const glm::vec3& pos);

		// Donne l'humidité en fonction de la position
		// Prend en compte:
		//		-perturbation local par un bruit
		//		-valeur intrinsèque de la planete (composition) (TODO)
		// Le résultat est à valeur dans [-1,1]
		// La position est une position dans le référentiel de la planète.
		float getHumidity(const glm::vec3& pos);
	private:
		std::vector<PlanetGenerator*> generators;
		std::list<MiniWorld*> miniWorldList;

		PlanetFace* faces[6];
		PlanetFaceBufferHandler* faceBuffers[6];

		glm::vec3 lightdir;
		glm::vec3 sunPosition;
		glm::vec3 position;
		glm::vec3 axis;
		glm::mat3 model, invModel;

		float angle;

		Cloud cloud;
		Atmosphere atmosphere;
};

#endif
