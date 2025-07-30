#include "unittest/catch.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "scene/scene.h"
#include "scene/map.h"

TEST_CASE("Test Map Random WalkablePos", "[CheckWalkerablePosList]") {
	
	Scene* scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(230, COMMON_SCENE_KEY); // ±ù·âÖ®Â·
	CHECK(NULL != scene);

	for (int i = 0; i < 1000000; ++i)
	{
		Posi posi = scene->GetMap()->GetARandWalkablePoint();

		CHECK(posi != Posi(0, 0));
		CHECK(posi != Posi(177, 292));
		CHECK('1' == scene->GetMap()->GetMapGrid(posi.x, posi.y));
	}
}