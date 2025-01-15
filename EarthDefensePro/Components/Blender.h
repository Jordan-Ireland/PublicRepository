#pragma once
// define all ECS components related to gameplay
#ifndef BLENDER_H
#define BLENDER_H

// example space game (avoid name collisions)
namespace ESG
{
	struct BlenderName {std::string name;};
	struct prefabName { std::string name; };
	struct ModelBoundry { GW::MATH::GOBBF obb;};
	struct ModelTransform {GW::MATH::GMATRIXF matrix;};
	struct lvlData { Level_Data levelData;};
	struct Renderable { bool value = true; };
	struct Instance { unsigned int modelIndex; unsigned int transformStart; unsigned int transformCount;unsigned int transformIndex; };
	struct Model { unsigned int indexStart; unsigned int vertexStart; 
					unsigned int materialStart; unsigned int meshStart;
					unsigned int meshCount; unsigned int vertexCount;
					uint32_t useTextures;
				};
	struct Mesh { unsigned int indexOffset; unsigned int indexCount;
	unsigned int materialIndex;};
	struct InstanceTag {};
	struct SingleInstance{};

	
	


};

#endif

//ship
//transform index 4
//mTransform good
//modelindex 2, transformStart 4 transform count 1
// ESGInstance good
// 
//
