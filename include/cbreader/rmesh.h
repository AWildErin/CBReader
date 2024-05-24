#pragma once

#include "cbreader/types.h"

#include <vector>
#include <string>

class BufferStream;

constexpr std::uint32_t MAX_SOUND_EMITTERS = 16;

struct Header
{
	// Length prefixed string
	std::uint32_t header_length = 0;
	std::string header;

	bool hasTriggerBox = false;
	bool hasNoColl = false;
};

struct Texture
{
	std::byte blendType;

	std::uint32_t textureNameLength = 0;
	std::string textureName;
};

struct Vertex
{
	Vector3 vertex;
	Vector2 uv;

	float unk1;
	float unk2;

	std::byte r;
	std::byte g;
	std::byte b;
};

struct Surface
{
	Texture textures[2];

	std::uint32_t vertexCount = 0;
	std::vector<Vertex> vertices;

	std::uint32_t trianglesCount = 0;
	std::vector<Triangle> triangles;
};

struct Mesh
{
	std::int32_t surfaceCount;
	std::vector<Surface> surfaces;

	std::uint32_t nameLength = 0;
	std::string name;
};

/**
* @class Entity
* 
* Root entity class embedded in RMesh files
*/
struct Entity
{
	std::uint32_t classNameLength = 0;
	std::string className;

	/** This is read per entity. It would be ideal to read it in Entity, but EntityModel is special in that it has the model file name THEN the position. */
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;

	virtual void Read(BufferStream stream, std::uint32_t _classNameLength, std::string _className);
};

struct EntityScreen : Entity
{
	std::uint32_t imgPathLength = 0;
	std::string imgPath;

	virtual void Read(BufferStream stream, std::uint32_t _classNameLength, std::string _className);
};

struct EntityWaypoint : Entity
{
	virtual void Read(BufferStream stream, std::uint32_t _classNameLength, std::string _className);
};

struct EntityLight : Entity
{
	/** SCP:CB calculates this as range/2000.0 in the RMesh code*/
	float range = 0.f;

	/** Space delimited color string. Color is 3 ints, and multiplied by intensity when reading. */
	std::uint32_t colorLength = 0;
	std::string color;

	/** SCP:CB calculates this as Min(intensity*0.8,1)*/
	float intensity = 0.f;

	virtual void Read(BufferStream stream, std::uint32_t _classNameLength, std::string _className);
};

struct EntitySpotLight : EntityLight
{
	std::uint32_t anglesLength = 0;
	std::string angles;

	std::uint32_t innerConeAngle = 0;
	std::uint32_t outerConeAngle = 0;

	virtual void Read(BufferStream stream, std::uint32_t _classNameLength, std::string _className);
};

struct EntitySoundEmitter : Entity
{
	virtual void Read(BufferStream stream, std::uint32_t _classNameLength, std::string _className);
};

struct EntityPlayerStart : Entity
{
	std::uint32_t anglesLength = 0;
	std::string angles;

	virtual void Read(BufferStream stream, std::uint32_t _classNameLength, std::string _className);
};

struct EntityModel : Entity
{
	std::uint32_t pathLength = 0;
	std::string path;

	void Read(BufferStream stream, std::uint32_t _classNameLength, std::string _className) override;
};

/**
* @class RMesh
* RMesh is the main room mesh format in SCP: Containment Breach
* 
* @todo Maybe move Read to the constructor instead
*/
struct RMesh
{
	Header header;

	Mesh drawnMesh;
	Mesh collisionMesh;

	std::uint32_t triggerBoxCount = 0;
	std::vector<Mesh> triggerBoxes;

	std::uint32_t entityCount = 0;
	std::vector<Entity> entities;

	/**
	* Reads the specified RMesh file
	* 
	* @param path The file path you wish to open
	* 
	* @returns Whether the RMesh file was read correctly.
	*/
	bool Read(const std::string& path);

	Mesh ReadDrawnMesh(BufferStream& stream);
	Mesh ReadCollisionMesh(BufferStream& stream);
};