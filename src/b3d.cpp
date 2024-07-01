#include "cbreader/b3d.h"

#include "BufferStream.h"

#include <fstream>
#include <iostream>
#include <utility>

using namespace b3d;

std::uint32_t b3d::FourCC(std::string str)
{
	return str[3] + (str[2] << 8) + (str[1] << 16) + (str[0] << 24);
}

bool b3d::B3DModel::Read(const std::string& path)
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

	rootChunk.Read(stream);

	if (FourCC(rootChunk.name) != 'BB3D')
	{
		return false;
	}

	stream.read(b3dVersion);

	if (b3dVersion > 1)
	{
		return false;
	}

	// Read all the chunks
	while (stream.tell() < rootChunk.position + length)
	{
		std::string tag;
		stream.read(tag, 4);

		stream.seek(-4, std::ios::cur);

		switch (FourCC(tag))
		{
		case 'TEXS':
		{
			TEXSChunk texs;
			texs.Read(stream);
			texs.Process(stream);

			texsChunks.push_back(texs);
			break;
		}
		case 'BRUS':
		{
			BRUSChunk brus;
			brus.Read(stream);
			brus.Process(stream);

			brusChunks.push_back(brus);
			break;
		}
		case 'NODE':
		{
			NODEChunk node;
			node.Read(stream);
			node.Process(stream);

			rootNode = node;
			break;
		}
		}
	}

	return true;
}

void b3d::B3DChunk::Read(BufferStream& stream)
{
	stream.read(name, 4);

	position = stream.tell() - 4;

	stream.read(length);

	// Length does not include the header or itself, so add 8 to it.
	length += 8;
}

void b3d::B3DChunk::Process(BufferStream& stream)
{

}

void b3d::TEXSChunk::Process(BufferStream& stream)
{
	while (stream.tell() < position + length)
	{
		TEXSSubData sub;

		sub.name = stream.read_string();
		stream
			.read(sub.flags)
			.read(sub.blend)

			.read(sub.position.x)
			.read(sub.position.y)

			.read(sub.scale.x)
			.read(sub.scale.y)

			.read(sub.scale.y);

		textures.push_back(sub);
	}
}

void b3d::BRUSChunk::Process(BufferStream& stream)
{
	stream.read(texturesCount);

	while (stream.tell() < position + length)
	{
		BRUSSubData sub;

		// this doesn't work
		//sub.textureIds.reserve(texturesCount);

		sub.name = stream.read_string();
		stream
			.read(sub.color.r)
			.read(sub.color.g)
			.read(sub.color.b)
			.read(sub.color.a)

			.read(sub.shininess)
			.read(sub.blend)
			.read(sub.fx);

		for (std::uint32_t i = 0; i < texturesCount; i++)
		{
			sub.textureIds.push_back(-1);

			stream.read(sub.textureIds[i]);
		}

		brushes.push_back(sub);
	}
}

void b3d::NODEChunk::Process(BufferStream& stream)
{
	stream
		.read(nodeData.name)

		.read(nodeData.position.x)
		.read(nodeData.position.y)
		.read(nodeData.position.z)

		.read(nodeData.scale.x)
		.read(nodeData.scale.y)
		.read(nodeData.scale.z)

		.read(nodeData.rotation.w)
		.read(nodeData.rotation.z)
		.read(nodeData.rotation.y)
		.read(nodeData.rotation.x);

	while (stream.tell() < position + length)
	{
		std::string tag;
		stream.read(tag, 4);

		stream.seek(-4, std::ios::cur);

		switch (FourCC(tag))
		{
		case 'MESH':
		{
			MESHChunk mesh;
			mesh.Read(stream);
			mesh.Process(stream);

			hasMesh = true;
			meshes.push_back(mesh);
			break;
		}
		case 'BONE':
		{
			BONEChunk _bone;
			_bone.Read(stream);
			_bone.Process(stream);

			hasBone = true;
			bone = _bone;
			break;
		}
		case 'KEYS':
		{
			KEYSChunk keys;
			keys.Read(stream);
			keys.Process(stream);

			animKeys.push_back(keys);
			break;
		}
		case 'ANIM':
		{
			ANIMChunk anim;
			anim.Read(stream);
			anim.Process(stream);

			animations.push_back(anim);

			break;
		}
		case 'NODE':
		{
			NODEChunk node;
			node.Read(stream);
			node.Process(stream);
			childNodes.push_back(node);
			break;
		}
		default:
		{
			B3DChunk chunk;
			chunk.Read(stream);
			stream.seek(chunk.position + chunk.length, std::ios::beg);

//			__debugbreak();
			break;
		}
		}
	}
}

void b3d::MESHChunk::Process(BufferStream& stream)
{
	stream.read(brushId);

	while (stream.tell() < position + length)
	{
		std::string tag;
		stream.read(tag, 4);

		stream.seek(-4, std::ios::cur);

		switch (FourCC(tag))
		{
		case 'VRTS':
		{
			VRTSChunk vrts;
			vrts.Read(stream);
			vrts.Process(stream);

			vrtsChunks.push_back(vrts);
			break;
		}
		case 'TRIS':
		{
			TRISChunk tris;
			tris.Read(stream);
			tris.Process(stream);

			trisChunks.push_back(tris);
			break;
		}
		default:
		{
//			__debugbreak();
		}
		}
	}
}

void b3d::VRTSChunk::Process(BufferStream& stream)
{
	stream
		.read(flags)
		.read(texCoordSets)
		.read(texCoordSetsSize);


	while (stream.tell() < position + length)
	{
		VRTSSubData sub;
		
		stream
			.read(sub.position.x)
			.read(sub.position.y)
			.read(sub.position.z);

		if (flags & VertexFlags::HasNormals)
		{
			stream
				.read(sub.normal.x)
				.read(sub.normal.y)
				.read(sub.normal.z);
		}
	
		if (flags & VertexFlags::HasColor)
		{
			stream
				.read(sub.color.r)
				.read(sub.color.g)
				.read(sub.color.b)
				.read(sub.color.a);
		}

		for (std::uint32_t i = 0; i < texCoordSets; i++)
		{
			std::vector<float> texcoord(texCoordSetsSize);
			for (std::uint32_t j = 0; j < texCoordSetsSize; j++)
			{
				stream.read(texcoord[j]);
			}

			sub.textureCoordinates.push_back(texcoord);
		}

		vertices.push_back(sub);
	}
}

void b3d::TRISChunk::Process(BufferStream& stream)
{
	stream.read(brushId);

	while (stream.tell() < position + length)
	{
		cbtypes::Triangle tri;
		stream
			.read(tri.index1)
			.read(tri.index2)
			.read(tri.index3);

		triangles.push_back(tri);
	}
}

void b3d::BONEChunk::Process(BufferStream& stream)
{
	while (stream.tell() < position + length)
	{
		BONESubData sub;

		stream
			.read(sub.vertex)
			.read(sub.weight);

		weights.push_back(sub);
	}
}

void b3d::KEYSChunk::Process(BufferStream& stream)
{
	stream.read(flags);

	while (stream.tell() < position + length)
	{
		KEYSSubData sub;

		stream.read(sub.frame);

		if (flags & KeyFlags::HasPosition)
		{
			stream
				.read(sub.position.x)
				.read(sub.position.y)
				.read(sub.position.z);
		}

		if (flags & KeyFlags::HasScale)
		{
			stream
				.read(sub.scale.x)
				.read(sub.scale.y)
				.read(sub.scale.z);
		}

		if (flags & KeyFlags::HasRotation)
		{
			// @todo This may be inverted
			stream
				.read(sub.rotation.w)
				.read(sub.rotation.x)
				.read(sub.rotation.y)
				.read(sub.rotation.z);
		}

		frames.push_back(sub);
	}
}

void b3d::ANIMChunk::Process(BufferStream& stream)
{
	stream
		.read(flags)
		.read(frames)
		.read(fps);
}
