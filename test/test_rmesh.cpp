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
	EXPECT_EQ(rmesh.header.header.size(), 22);

	// This RMesh file should have trigger boxes but not "hasNoColl"
	EXPECT_TRUE(rmesh.header.hasTriggerBox);
	EXPECT_FALSE(rmesh.header.hasNoColl);

	EXPECT_EQ(rmesh.drawnMesh.surfaces.size(), 22);

	// Test both the first and second index, if these pass the rest should be fine

	// The first texture index usually contains the texture. Each "surface" is a section of the mesh that all uses the same texture
	EXPECT_EQ(rmesh.drawnMesh.surfaces[0].textures[1].textureName.size(), 21);
	EXPECT_STREQ(rmesh.drawnMesh.surfaces[0].textures[1].textureName.data(), "containment_doors.jpg");

	EXPECT_EQ(rmesh.drawnMesh.surfaces[0].vertices.size(), 318);
	EXPECT_EQ(rmesh.drawnMesh.surfaces[0].triangles.size(), 352);

	EXPECT_EQ(rmesh.drawnMesh.surfaces[1].textures[1].textureName.size(), 9);
	EXPECT_STREQ(rmesh.drawnMesh.surfaces[1].textures[1].textureName.data(), "metal.jpg");

	EXPECT_EQ(rmesh.drawnMesh.surfaces[1].vertices.size(), 9994);
	EXPECT_EQ(rmesh.drawnMesh.surfaces[1].triangles.size(), 5104);

	// Test the collision model, if we got this passes then it's safe to assume the drawn mesh was read fine.
	EXPECT_EQ(rmesh.collisionMesh.surfaces.size(), 1);
	EXPECT_EQ(rmesh.collisionMesh.surfaces[0].vertices.size(), 18);
	EXPECT_EQ(rmesh.collisionMesh.surfaces[0].triangles.size(), 8);

	// Test trigger boxes
	EXPECT_EQ(rmesh.triggerBoxes.size(), 3);
	EXPECT_STREQ(rmesh.triggerBoxes[0].name.data(), "173scene_timer");
	EXPECT_STREQ(rmesh.triggerBoxes[1].name.data(), "173scene_activated");
	EXPECT_STREQ(rmesh.triggerBoxes[2].name.data(), "173scene_end");


	// Test the first 2 model entities
	EXPECT_STREQ(rmesh.modelEntities[0].className.data(), "model");
	EXPECT_STREQ(rmesh.modelEntities[0].path.data(), "lamp2.x");
	EXPECT_STREQ(rmesh.modelEntities[1].className.data(), "model");
	EXPECT_STREQ(rmesh.modelEntities[1].path.data(), "lamp2.x");
}

/** Reads an RMesh file from the SCP: NTF Mod. This file incorporates hasNoColl in the header. */
TEST(RMesh, read_ntf) 
{
	// TODO: Implement reading hasNoColl first
}