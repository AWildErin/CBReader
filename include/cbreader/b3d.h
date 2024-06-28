#include "cbreader/types.h"

#include <string>
#include <vector>
#include <array>

class BufferStream;

/**
* @note B3D Vectors are left hand y up.
* 
* @todo Maybe rename the SubData blocks to be easier to use
*/
namespace b3d
{
	// Forward declare some structs
	struct B3DModel;
	struct MESHChunk;
	struct BONEChunk;
	struct KEYSChunk;
	struct ANIMChunk;
	struct VRTSChunk;
	struct TRISChunk;

	enum B3DChunkType
	{
		BB3D,
		TEXS,
		BRUS,
		NODE,
		MESH,
		BONE,
		KEYS,
		ANIM,
	};

	/**
	* Takes a string and returns a FourCC based on the first 4 characters.
	* 
	* @todo Maybe just take a char[4] array instead?
	* 
	* https://en.wikipedia.org/wiki/FourCC
	*/
	static std::uint32_t FourCC(std::string str);

	struct B3DChunk
	{
		/** Name of the chunk, usually BB3D, TEXS, BRUS, NODE, MESH, VRTS, or TRIS */
		std::string name;

		/** Start position of this chunk in the file */
		std::uint32_t position = 0;

		/** Length of the chunk */
		std::uint32_t length = 0;

		/** Model this chunk belongs to */
		b3d::B3DModel& model;

		/**
		* Reads basic information about the chunk but does not process it
		*/
		virtual void Read(BufferStream& stream);

		virtual void Process(BufferStream& stream);
	};

	struct TEXSChunk : public B3DChunk
	{
		struct TEXSSubData
		{
			std::string name;
			/** @todo What are the flags*/
			std::uint32_t flags;
			std::uint32_t blend;
			cbtypes::Vector2 position;
			cbtypes::Vector2 scale;
			float rotation;
		};

		std::vector<TEXSSubData> textures;

		virtual void Process(BufferStream& stream) override;
	};

	struct BRUSChunk : public B3DChunk
	{	
		struct BRUSSubData
		{
			std::string name;
			cbtypes::Color color;
			float shininess;
			std::uint32_t blend;
			std::uint32_t fx;
			std::vector<std::uint32_t> textureIds;

		};

		std::uint32_t texturesCount = 0;

		std::vector<BRUSSubData> brushes;

		virtual void Process(BufferStream& stream) override;
	};

	struct MESHChunk : public B3DChunk
	{
		std::uint32_t brushId = 0;

		std::vector<VRTSChunk> vrtsChunks;
		std::vector<TRISChunk> trisChunks;

		virtual void Process(BufferStream& stream) override;
	};

	struct VRTSChunk : public B3DChunk
	{
		enum VertexFlags : std::uint32_t
		{
			None = 0,
			HasNormals = 1,
			HasColor = 2
		};

		VRTSChunk::VertexFlags flags = VertexFlags::None;
		std::uint32_t texCoordSets = 0;
		std::uint32_t texCoordSetsSize = 0;

		/**
		* A vertex for a B3d Mesh
		* @todo Maybe extract this and call it B3DVert
		*/
		struct VRTSSubData
		{
			cbtypes::Vector3 position;
			cbtypes::Vector3 normal;
			cbtypes::Color color;
			std::vector<std::vector<float>> textureCoordinates;
		};

		std::vector<VRTSSubData> vertices;

		virtual void Process(BufferStream& stream) override;
	};

	struct TRISChunk : public B3DChunk
	{
		std::uint32_t brushId = 0;
		std::vector<cbtypes::Triangle> triangles;

		virtual void Process(BufferStream& stream) override;
	};

	struct BONEChunk : public B3DChunk
	{
		struct BONESubData
		{
			std::uint32_t vertex = 0;
			float weight = 0.f;
		};

		/** @todo maybe make this a map? */
		std::vector<BONESubData> weights;

		virtual void Process(BufferStream& stream) override;
	};

	struct KEYSChunk : public B3DChunk
	{
		/** @todo according to the b3d code there is no 3rd flag, is this true? */
		enum KeyFlags : std::uint32_t
		{
			None = 0,
			HasPosition = 1,
			HasScale = 2,
			HasRotation = 4
		};

		struct KEYSSubData
		{
			std::uint32_t frame = 0;
			cbtypes::Vector3 position;
			cbtypes::Vector3 scale;
			cbtypes::Quaternion rotation;
		};

		KEYSChunk::KeyFlags flags;
		std::vector<KEYSSubData> frames;

		virtual void Process(BufferStream& stream) override;
	};

	struct ANIMChunk : public B3DChunk
	{
		/** This isn't used anymore but it's kept for parity */
		std::uint32_t flags = 0;

		std::uint32_t frames = 0;
		float fps =  0.f;

		virtual void Process(BufferStream& stream) override;
	};

	struct NODEChunk : public B3DChunk
	{
		struct NODESubData
		{
			std::string name;
			cbtypes::Vector3 position;
			cbtypes::Vector3 scale;
			cbtypes::Quaternion rotation;
		};

		NODESubData nodeData;

		/** @todo can there be multiple bones for a node? */
		BONEChunk bone;

		std::vector<MESHChunk> meshes;
		std::vector<KEYSChunk> animKeys;
		std::vector<ANIMChunk> animations;
		std::vector<NODEChunk> childNodes;

		virtual void Process(BufferStream& stream) override;
	};

	struct B3DModel
	{
		std::uint32_t b3dVersion;

		B3DChunk rootChunk;

		std::vector<TEXSChunk> texsChunks;
		std::vector<BRUSChunk> brusChunks;
		std::vector<NODEChunk> nodeChunks;

		std::vector<B3DChunk> unknownChunks;

		/**
		* Reads the specified RMesh file
		*
		* @param path The file path you wish to open
		*
		* @returns Whether the RMesh file was read correctly.
		*/
		bool Read(const std::string& path);
	};
};