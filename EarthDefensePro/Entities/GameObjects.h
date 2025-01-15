#include "../Components/Blender.h"



// defines logic used to manipulate the models in the level


class GameObjects
{
	std::shared_ptr<flecs::world> world;
	GW::MATH::GMatrix matrixProxy;

public:
	// constructor assumes game objects have been loaded by level loader
	GameObjects(std::shared_ptr<flecs::world>& _world, std::shared_ptr<Level_Data>& import, GW::SYSTEM::GLog log) {
		matrixProxy.Create();
		// create flecs world
		world = _world;
		//flecs::entity levelData = world->entity("LevelData");
		//levelData.set<ESG::lvlData>({ *import });
		flecs::entity ent;
		// add a named entity for each object in blender
	
		for (auto& i : import->blenderObjects) {
			auto modelInfo = import->levelModels[i.modelIndex];
			auto instanceInfo = import->levelInstances[i.modelIndex];

			// create entity with same name as blender object
			ent = world->entity(i.blendername);
			ent.set<ESG::BlenderName>({ i.blendername });

			GW::MATH::GOBBF bound = import->levelColliders[import->levelModels[i.modelIndex].colliderIndex];
			GW::MATH::GMATRIXF trans = import->levelTransforms[i.transformIndex];

			bound.center.x = trans.row4.x;
			bound.center.y = trans.row4.y;
			bound.center.z = 0;

			GW::MATH::GMatrix::GetRotationF(trans, bound.rotation);

			ent.set<ESG::ModelBoundry>({ bound });
		
			ent.set<ESG::ModelTransform>({ trans });
			//ent.set<ESG::ModelBoundry>({ import->levelTransforms[i.transformIndex].row4.x,import->levelTransforms[i.transformIndex].row4.y,import->levelTransforms[i.transformIndex].row4.z, .10f});
			
			//get instance data for rendering
			ent.set<ESG::Instance>({i.modelIndex,import->levelInstances[i.modelIndex].transformStart,import->levelInstances[i.modelIndex].transformCount,i.transformIndex }); //last two might be wrong
			//get model data for rendering
			ent.set<ESG::Model>({ import->levelModels[i.modelIndex].indexStart, import->levelModels[i.modelIndex].vertexStart,
				import->levelModels[i.modelIndex].materialStart, import->levelModels[i.modelIndex].meshStart, import->levelModels[i.modelIndex].meshCount,
				import->levelModels[i.modelIndex].vertexCount, import->levelModels[i.modelIndex].useTextures });
			//get mesh data for rendering
			ent.set<ESG::Mesh>({ import->levelMeshes[i.modelIndex].drawInfo.indexOffset, import->levelMeshes[i.modelIndex].drawInfo.indexCount, import->levelMeshes[i.modelIndex].materialIndex });
			//ent.set<ESG::Renderable>({ true });
		
			//ent.add<ESG::CollideWithPlayer>();
		}
		

		

		// for now just print objects added to FLECS
		auto f = world->filter<ESG::BlenderName, ESG::ModelTransform>();
		f.each([&log](ESG::BlenderName& n, ESG::ModelTransform& t) {
			std::string obj = "FLECS Entity ";
			obj += n.name + " located at X " + std::to_string(t.matrix.row4.x) +
				" Y " + std::to_string(t.matrix.row4.y) + " Z " + std::to_string(t.matrix.row4.z);
			
			log.LogCategorized("GAMEPLAY", obj.c_str());
			});

	};
	// TODO: Write more gameplay logic/systems to manpulate specific objects in the level 

	void Update(std::shared_ptr<flecs::world>& _world, std::shared_ptr<Level_Data>& level, GW::SYSTEM::GLog log, GW::INPUT::GInput input, float dt) {
		//world = _world;
		//// array of floats to store the current key states
		//float kbm_states[256] = { 0 };
		//// loop over the possible keyboard keys
		//for (size_t i = 0; i < ARRAYSIZE(kbm_states); ++i) {
		//	// get the current keyboard key state
		//	input.GetState(G_KEY_UNKNOWN + i, kbm_states[i]);


		//}
		//// create a change in forward motion
		//float z_change = kbm_states[G_KEY_W] - kbm_states[G_KEY_S];
		//float x_change = kbm_states[G_KEY_D] - kbm_states[G_KEY_A];

		//if (kbm_states[G_KEY_W])
		//{
		//	std::cout << "Up" << std::endl;
		//}
		//if (kbm_states[G_KEY_S])
		//{
		//	std::cout << "Down" << std::endl;
		//}
		//if (kbm_states[G_KEY_A])
		//{
		//	std::cout << "Left" << std::endl;
		//}
		//if (kbm_states[G_KEY_D])
		//{
		//	std::cout << "Right" << std::endl;
		//}
		//// vector to store the change in forward motion
		//GW::MATH::GVECTORF v = { x_change * dt, z_change * dt, 0 };
		////std::cout << "UPDATE" << std::endl;
		//// get the model's transform
		//auto e = world->lookup("Ship");
		//auto w = world->lookup("LevelData");

		//auto LD = w.get_mut<ESG::lvlData>();

		//if (e.is_valid()) {
		//	ESG::ModelTransform* edit = world->entity(e).get_mut<ESG::ModelTransform>();
		//	if (edit) {
		//		// translate the current matrix by the vector containing forward motion
		//		matrixProxy.TranslateGlobalF(edit->matrix, v, edit->matrix);

		//		// store the updated matrix in the level's transform std::vector
		//		//level->levelTransforms[edit->transformIndex] = edit->matrix;
		//		LD->levelData.levelTransforms[edit->transformIndex] = edit->matrix;
		//		
		//	}
		//}




	}



};

//	// This class creates all types of bullet prefabs
//#ifndef GAMEOBJECTS_H
//#define GAMEOBJECTS_H
//
//// Contains our global game settings
//#include "../GameConfig.h"
//
//// example space game (avoid name collisions)
//	namespace ESG
//	{
//		class GameObjects
//		{
//		public:
//			// Load required entities and/or prefabs into the ECS 
//			bool Load(std::shared_ptr<flecs::world> _game);
//			// Unload the entities/prefabs from the ECS
//			bool Unload(std::shared_ptr<flecs::world> _game);
//		};
//
//	};
//
//#endif