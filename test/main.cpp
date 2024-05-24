#include <gtest/gtest.h>

#include "config.h"

#define MANUAL_TEST 1

#if MANUAL_TEST
#include "cbreader/rmesh.h"
#endif

int main(int argc, char** argv)
{
#if MANUAL_TEST
	//std::string path = RESOURCES_ROOT_PATH "/173.rmesh";
	std::string path = RESOURCES_ROOT_PATH "/173_opt.rmesh";

	RMesh mesh;
	mesh.Read(path);

	return 0;
#else
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
#endif
}