#include "cbreader/rmesh.h"

#include "BufferStream.h"

#include <fstream>
#include <iostream>
#include <utility>

bool RMesh::Read(const std::string& path)
{
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		return false;
	}

	std::streamsize length = file.tellg();
	std::vector<char> buffer(length);

	file.seekg(0, std::ios::beg);
	file.read(&buffer[0], length);
	file.close();

	BufferStream stream(reinterpret_cast<std::byte*>(buffer.data()), buffer.size());

	stream.read(header.header, stream.read<std::uint32_t>(), false);

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
		std::uint32_t triggerBoxCount = stream.read<std::uint32_t>();
		for (std::uint32_t i = 0; i < triggerBoxCount; i++)
		{
			Mesh mesh = ReadCollisionMesh(stream);
			stream.read(mesh.name, stream.read<std::uint32_t>(), false);

			triggerBoxes.push_back(mesh);
		}
	}

	std::uint32_t entityCount = stream.read<std::uint32_t>();
	for (std::uint32_t i = 0; i < entityCount; i++)
	{
		std::string className = stream.read_string(stream.read<std::uint32_t>(), false);

		if (className == "screen")
		{
			EntityScreen ent;
			ent.Read(stream, className);
			screenEntities.push_back(ent);
		}
		else if (className == "waypoint")
		{
			EntityWaypoint ent;
			ent.Read(stream, className);
			waypointEntities.push_back(ent);
		}
		else if (className == "light")
		{
			EntityLight ent;
			ent.Read(stream, className);
			lightEntities.push_back(ent);
		}
		else if (className == "spotlight")
		{
			EntitySpotLight ent;
			ent.Read(stream, className);
			spotlightEntities.push_back(ent);
		}
		else if (className == "soundemitter")
		{
			EntitySoundEmitter ent;
			ent.Read(stream, className);
			soundEmitterEntities.push_back(ent);
		}
		else if (className == "playerstart")
		{
			EntityPlayerStart ent;
			ent.Read(stream, className);
			playerStartEntities.push_back(ent);
		}
		else if (className == "model")
		{
			EntityModel ent;
			ent.Read(stream, className);
			modelEntities.push_back(ent);
		}
	}

	return true;
}

Mesh RMesh::ReadDrawnMesh(BufferStream& stream)
{
	Mesh mesh;

	std::uint32_t surfaceCount = stream.read<std::uint32_t>();
	for (std::uint32_t i = 0; i < surfaceCount; i++)
	{
		Surface& surf = mesh.surfaces.emplace_back();

		// Read textures
		for (Texture& texture : surf.textures)
		{
			stream.read(texture.blendType);

			if (texture.blendType != std::byte(0))
			{
				stream.read(texture.textureName, stream.read<std::uint32_t>(), false);
			}
		}

		std::uint32_t vertexCount = stream.read<std::uint32_t>();
		for (std::uint32_t j = 0; j < vertexCount; j++)
		{
			Vertex& vert = surf.vertices.emplace_back();
			stream
				.read(vert.vertex.x)
				.read(vert.vertex.y)
				.read(vert.vertex.z)

				.read(vert.uv.x)
				.read(vert.uv.y)

				.read(vert.unk1)
				.read(vert.unk2)

				.read(vert.r)
				.read(vert.g)
				.read(vert.b);
		}

		std::uint32_t triangleCount = stream.read<std::uint32_t>();
		for (std::uint32_t j = 0; j < triangleCount; j++)
		{
			Triangle& triangle = surf.triangles.emplace_back();
			stream
				.read(triangle.index1)
				.read(triangle.index2)
				.read(triangle.index3);
		}
	}

	return mesh;
}

Mesh RMesh::ReadCollisionMesh(BufferStream& stream)
{
	Mesh mesh;

	std::uint32_t surfaceCount = stream.read<std::uint32_t>();
	for (std::uint32_t i = 0; i < surfaceCount; i++)
	{
		Surface& surf = mesh.surfaces.emplace_back();

		std::uint32_t vertexCount = stream.read<std::uint32_t>();
		for (std::uint32_t j = 0; j < vertexCount; j++)
		{
			Vertex& vert = surf.vertices.emplace_back();
			stream
				.read(vert.vertex.x)
				.read(vert.vertex.y)
				.read(vert.vertex.z);
		}

		std::uint32_t triangleCount = stream.read<std::uint32_t>();
		for (std::uint32_t j = 0; j < triangleCount; j++)
		{
			Triangle triangle{};
			triangle.index1 = stream.read<std::uint32_t>();
			triangle.index2 = stream.read<std::uint32_t>();
			triangle.index3 = stream.read<std::uint32_t>();
			surf.triangles.push_back(triangle);
		}
	}

	return mesh;
}

void Entity::Read(BufferStream& stream, std::string _className)
{
	className = std::move(_className);
}

void EntityScreen::Read(BufferStream& stream, std::string _className)
{
	Entity::Read(stream, std::move(_className));
	stream
		.read(position.x)
		.read(position.y)
		.read(position.z)

		.read(imgPath, stream.read<std::uint32_t>(), false);
}

void EntityWaypoint::Read(BufferStream& stream, std::string _className)
{
	Entity::Read(stream, std::move(_className));
	stream
		.read(position.x)
		.read(position.y)
		.read(position.z);
}

void EntityLight::Read(BufferStream& stream, std::string _className)
{
	Entity::Read(stream, _className);
	stream
		.read(position.x)
		.read(position.y)
		.read(position.z)

		.read(range)

		.read(color, stream.read<std::uint32_t>(), false)

		.read(intensity);
}

void EntitySpotLight::Read(BufferStream& stream, std::string _className)
{
	EntityLight::Read(stream, _className);
	stream
		.read(angles, stream.read<std::uint32_t>(), false)

		.read(innerConeAngle)
		.read(outerConeAngle);
}

void EntitySoundEmitter::Read(BufferStream& stream, std::string _className)
{
	Entity::Read(stream, _className);

	/// @todo Handle sound emitters, will need to look more into the SCP:CB code to
	/// determine how to read these correctly. for now we will just skip over them
	stream
		.read(position.x)
		.read(position.y)
		.read(position.z)

		.read(sound)
		.read(range);
}

void EntityPlayerStart::Read(BufferStream& stream, std::string _className)
{
	Entity::Read(stream, _className);
	stream
		.read(position.x)
		.read(position.y)
		.read(position.z)

		.read(angles, stream.read<std::uint32_t>(), false);
}

void EntityModel::Read(BufferStream& stream, std::string _className)
{
	Entity::Read(stream, _className);
	stream
		.read(path, stream.read<std::uint32_t>(), false)

		.read(position.x)
		.read(position.y)
		.read(position.z)

		.read(rotation.x)
		.read(rotation.y)
		.read(rotation.z)

		.read(scale.x)
		.read(scale.y)
		.read(scale.z);
}