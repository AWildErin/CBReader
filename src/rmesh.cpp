#include "cbreader/rmesh.h"

#include "BufferStream.h"

#include <fstream>
#include <iostream>

bool RMesh::Read(const std::string& path)
{
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		return false;
	}

	int length = file.tellg();
	std::vector<char> buffer(length);

	file.seekg(0, std::ios::beg);
	file.read(&buffer[0], length);
	file.close();

	BufferStream stream(reinterpret_cast<std::byte*>(buffer.data()), buffer.size());

	header.header_length = stream.read<std::uint32_t>();
	header.header = stream.read_string(header.header_length, false);

	if (header.header.find("RoomMesh") == std::string::npos)
	{
		/// @todo Error messages
		return false;
	}

	header.hasTriggerBox = header.header.find(".HasTriggerBox") != std::string::npos;
	header.hasNoColl = header.header.find(".HasNoColl") != std::string::npos;

	// Drawn Mesh
	drawnMesh = ReadDrawnMesh(stream);

	// Collision Mesh
	collisionMesh = ReadCollisionMesh(stream);

	if (header.hasTriggerBox)
	{
		triggerBoxCount = stream.read<std::uint32_t>();
		for (int i = 0; i < triggerBoxCount; i++)
		{
			Mesh mesh = ReadCollisionMesh(stream);
			mesh.nameLength = stream.read<std::uint32_t>();
			mesh.name = stream.read_string(mesh.nameLength, false);

			triggerBoxes.push_back(mesh);
		}
	}

	/**
	@todo This is broken, for some reason it doesn't read the next entity correctly on 173_opt.rmesh

	entityCount = stream.read<std::uint32_t>();
	for (int i = 0; i < entityCount; i++)
	{
		std::uint32_t classNameLength = stream.read<std::uint32_t>();
		std::string className = stream.read_string(classNameLength, false);

		if (className == "screen")
		{
			EntityScreen ent;
			ent.Read(stream, classNameLength, className);
			entities.push_back(ent);
		}
		else if (className == "waypoint")
		{
			EntityWaypoint ent;
			ent.Read(stream, classNameLength, className);
			entities.push_back(ent);
		}
		else if (className == "light")
		{
			EntityLight ent;
			ent.Read(stream, classNameLength, className);
			entities.push_back(ent);
		}
		else if (className == "spotlight")
		{
			EntitySpotLight ent;
			ent.Read(stream, classNameLength, className);
			entities.push_back(ent);
		}
		else if (className == "soundemitter")
		{
			EntitySoundEmitter ent;
			ent.Read(stream, classNameLength, className);
			entities.push_back(ent);
		}
		else if (className == "playerstart")
		{
			EntityPlayerStart ent;
			ent.Read(stream, classNameLength, className);
			entities.push_back(ent);
		}
		else if (className == "model")
		{
			EntityModel ent;
			ent.Read(stream, classNameLength, className);
			entities.push_back(ent);
		}
	}
	*/

	return true;
}

Mesh RMesh::ReadDrawnMesh(BufferStream& stream)
{
	Mesh mesh;

	mesh.surfaceCount = stream.read<std::uint32_t>();

	for (int i = 0; i < mesh.surfaceCount; i++)
	{
		Surface surf;

		// Read textures
		for (int j = 0; j < 2; j++)
		{
			Texture texture;
			texture.blendType = stream.read<std::byte>();

			if (texture.blendType != std::byte(0))
			{
				texture.textureNameLength = stream.read<std::uint32_t>();
				texture.textureName = stream.read_string(texture.textureNameLength, false);
			}

			surf.textures[j] = texture;
		}

		surf.vertexCount = stream.read<std::uint32_t>();
		for (int j = 0; j < surf.vertexCount; j++)
		{
			Vertex vert{};

			vert.vertex.x = stream.read<float>();
			vert.vertex.y = stream.read<float>();
			vert.vertex.z = stream.read<float>();

			vert.uv.x = stream.read<float>();
			vert.uv.y = stream.read<float>();

			vert.unk1 = stream.read<float>();
			vert.unk2 = stream.read<float>();

			vert.r = stream.read<std::byte>();
			vert.g = stream.read<std::byte>();
			vert.b = stream.read<std::byte>();

			surf.vertices.push_back(vert);
		}

		surf.trianglesCount = stream.read<std::uint32_t>();
		for (int j = 0; j < surf.trianglesCount; j++)
		{
			Triangle triangle{};
			triangle.index1 = stream.read<std::uint32_t>();
			triangle.index2 = stream.read<std::uint32_t>();
			triangle.index3 = stream.read<std::uint32_t>();
			surf.triangles.push_back(triangle);
		}

		mesh.surfaces.push_back(surf);
	}

	return mesh;
}

Mesh RMesh::ReadCollisionMesh(BufferStream& stream)
{
	Mesh mesh;

	mesh.surfaceCount = stream.read<std::uint32_t>();

	for (int i = 0; i < mesh.surfaceCount; i++)
	{
		Surface surf;

		surf.vertexCount = stream.read<std::uint32_t>();
		for (int j = 0; j < surf.vertexCount; j++)
		{
			Vertex vert{};
			vert.vertex.x = stream.read<float>();
			vert.vertex.y = stream.read<float>();
			vert.vertex.z = stream.read<float>();
			surf.vertices.push_back(vert);
		}

		surf.trianglesCount = stream.read<std::uint32_t>();
		for (int j = 0; j < surf.trianglesCount; j++)
		{
			Triangle triangle{};
			triangle.index1 = stream.read<std::uint32_t>();
			triangle.index2 = stream.read<std::uint32_t>();
			triangle.index3 = stream.read<std::uint32_t>();
			surf.triangles.push_back(triangle);
		}

		mesh.surfaces.push_back(surf);
	}

	return mesh;
}

void Entity::Read(BufferStream stream, std::uint32_t _classNameLength, std::string _className)
{
	classNameLength = _classNameLength;
	className = _className;
}

void EntityScreen::Read(BufferStream stream, std::uint32_t _classNameLength, std::string _className)
{
	Entity::Read(stream, _classNameLength, _className);

	position.x = stream.read<float>();
	position.y = stream.read<float>();
	position.z = stream.read<float>();

	imgPathLength = stream.read<std::uint32_t>();
	imgPath = stream.read_string(imgPathLength, false);
}

void EntityWaypoint::Read(BufferStream stream, std::uint32_t _classNameLength, std::string _className)
{
	Entity::Read(stream, _classNameLength, _className);

	position.x = stream.read<float>();
	position.y = stream.read<float>();
	position.z = stream.read<float>();
}

void EntityLight::Read(BufferStream stream, std::uint32_t _classNameLength, std::string _className)
{
	Entity::Read(stream, _classNameLength, _className);

	position.x = stream.read<float>();
	position.y = stream.read<float>();
	position.z = stream.read<float>();

	range = stream.read<float>();

	colorLength = stream.read<std::uint32_t>();
	color = stream.read_string(colorLength, false);

	intensity = stream.read<float>();
}

void EntitySpotLight::Read(BufferStream stream, std::uint32_t _classNameLength, std::string _className)
{
	EntityLight::Read(stream, _classNameLength, _className);

	anglesLength = stream.read<std::uint32_t>();
	angles = stream.read_string(anglesLength, false);

	innerConeAngle = stream.read<std::uint32_t>();
	outerConeAngle = stream.read<std::uint32_t>();
}

void EntitySoundEmitter::Read(BufferStream stream, std::uint32_t _classNameLength, std::string _className)
{
	Entity::Read(stream, _classNameLength, _className);

	/// @todo Handle sound emitters, will need to look more into the SCP:CB code to
	/// determine how to read these correctly. for now we will just skip over them
	stream.read<float>();
	stream.read<float>();
	stream.read<float>();
	stream.read<std::uint32_t>();
	stream.read<float>();
}

void EntityPlayerStart::Read(BufferStream stream, std::uint32_t _classNameLength, std::string _className)
{
	Entity::Read(stream, _classNameLength, _className);

	position.x = stream.read<float>();
	position.y = stream.read<float>();
	position.z = stream.read<float>();

	anglesLength = stream.read<std::uint32_t>();
	angles = stream.read_string(anglesLength, false);
}

void EntityModel::Read(BufferStream stream, std::uint32_t _classNameLength, std::string _className)
{
	Entity::Read(stream, _classNameLength, _className);

	pathLength = stream.read<std::uint32_t>();
	path = stream.read_string(pathLength, false);

	position.x = stream.read<float>();
	position.y = stream.read<float>();
	position.z = stream.read<float>();

	rotation.x = stream.read<float>();
	rotation.y = stream.read<float>();
	rotation.z = stream.read<float>();

	scale.x = stream.read<float>();
	scale.y = stream.read<float>();
	scale.z = stream.read<float>();
}