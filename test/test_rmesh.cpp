#include "cbreader/rmesh.h"
#include "config.h"

#include <gtest/gtest.h>

/** Read 173_opt.rmesh. This mesh has trigger boxes and is a good candidate for all around rmesh testing. */
TEST(RMesh, read_triggerboxes)
{
	std::string path = RESOURCES_ROOT_PATH "/173_opt.rmesh";

	RMesh rmesh;

	// Check if the RMesh file loaded
	EXPECT_TRUE(rmesh.Read(path));

	// The header for this file should be "RoomMesh.HasTriggerBox"
	EXPECT_STREQ(rmesh.header.header.data(), "RoomMesh.HasTriggerBox");

	// Length of the header string.
	EXPECT_EQ(rmesh.header.header_length, 22);

	// This RMesh file should have trigger boxes but not "hasNoColl"
	EXPECT_TRUE(rmesh.header.hasTriggerBox);
	EXPECT_FALSE(rmesh.header.hasNoColl);

	EXPECT_EQ(rmesh.drawnMesh.surfaceCount, 22);

	// Test both the first and second index, if these pass the rest should be fine

	// The first texture index usually contains the texture. Each "surface" is a section of the mesh that all uses the same texture
	EXPECT_EQ(rmesh.drawnMesh.surfaces[0].textures[1].textureNameLength, 21);
	EXPECT_STREQ(rmesh.drawnMesh.surfaces[0].textures[1].textureName.data(), "containment_doors.jpg");

	EXPECT_EQ(rmesh.drawnMesh.surfaces[0].vertexCount, 318);
	EXPECT_EQ(rmesh.drawnMesh.surfaces[0].trianglesCount, 352);

	EXPECT_EQ(rmesh.drawnMesh.surfaces[1].textures[1].textureNameLength, 9);
	EXPECT_STREQ(rmesh.drawnMesh.surfaces[1].textures[1].textureName.data(), "metal.jpg");

	EXPECT_EQ(rmesh.drawnMesh.surfaces[1].vertexCount, 9994);
	EXPECT_EQ(rmesh.drawnMesh.surfaces[1].trianglesCount, 5104);

	// Test the collision model, if we got this passes then it's safe to assume the drawn mesh was read fine.
	EXPECT_EQ(rmesh.collisionMesh.surfaceCount, 1);
	EXPECT_EQ(rmesh.collisionMesh.surfaces[0].vertexCount, 18);
	EXPECT_EQ(rmesh.collisionMesh.surfaces[0].trianglesCount, 8);
}

/** Reads an RMesh file from the SCP: NTF Mod. This file incorporates hasNoColl in the header. */
TEST(RMesh, read_ntf) 
{
	// TODO: Implement reading hasNoColl first
}