#include "cbreader/b3d.h"
#include "config.h"

#include <gtest/gtest.h>

TEST(B3D, read_staticmesh)
{
	std::string path = RESOURCES_ROOT_PATH "/173_2.b3d";

	b3d::B3DModel model;
	EXPECT_TRUE(model.Read(path));

	EXPECT_EQ(model.b3dVersion, 1);

	// Check for the only texture in this TEXS block (yes, this is the actual path for some reason)
	EXPECT_STREQ(model.texsChunks[0].textures[0].name.data(), "D:\\SCP - CB - Vanilla\\GFX\\npcs\\173texture.jpg");

	EXPECT_STREQ(model.nodeChunks[0].nodeData.name.data(), "Scene_Root");

	__debugbreak();
}