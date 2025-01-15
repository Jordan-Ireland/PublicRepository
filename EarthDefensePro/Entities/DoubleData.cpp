#include "DoubleData.h"
#include "../Components/Identification.h"
#include "../Components/Visuals.h"
#include "../Components/Physics.h"
#include "Prefabs.h"
#include "../Components/Gameplay.h"
#include "../Components/Blender.h"

bool ESG::DoubleData::Load(	std::shared_ptr<flecs::world> _game,
							std::weak_ptr<const GameConfig> _gameConfig,
							GW::AUDIO::GAudio _audioEngine)
{
	// Grab init settings for players
	std::shared_ptr<const GameConfig> readCfg = _gameConfig.lock();
	_game;
	// Create prefab for lazer weapon
	// color
	

	//auto mesh = bulletData.get_mut<ESG::Mesh>();
	
	// other attributes
	float speed = (*readCfg).at("Double").at("speed").as<float>();
	float xscale = (*readCfg).at("Double").at("xscale").as<float>();
	float yscale = (*readCfg).at("Double").at("yscale").as<float>();
	float zscale = (*readCfg).at("Double").at("zscale").as<float>();
	int dmg = (*readCfg).at("Double").at("damage").as<int>();
	float forwardDirection = (*readCfg).at("Double").at("direction").as<int>();
	std::string fireFX = (*readCfg).at("Double").at("fireFX").as<std::string>();

	// default projectile scale
	//ESG::ModelTransform world;
	auto e = _game->lookup("Xullet");
	
	//e.add<ESG::InstanceTag>();
	
		auto inst = e.get_mut<ESG::Instance>();
		auto mesh = e.get<ESG::Mesh>();
		auto modelTransform = e.get_mut<ESG::ModelTransform>();
		auto model= e.get_mut<ESG::Model>();
		modelTransform->matrix.row1.x = xscale;
		modelTransform->matrix.row2.z = zscale;
		modelTransform->matrix.row3.y = -yscale;
		GW::MATH::GMATRIXF rotationMatrix = GW::MATH::GIdentityMatrixF;
		GW::MATH::GMatrix::RotateZLocalF(rotationMatrix, G_DEGREE_TO_RADIAN_F(-45), rotationMatrix);

		GW::MATH::GMatrix::MultiplyMatrixF(  modelTransform->matrix, rotationMatrix, modelTransform->matrix);
		auto modelBoundary = e.get_mut<ESG::ModelBoundry>();

	// Load sound effect used by this bullet prefab
	GW::AUDIO::GSound shoot;
	shoot.Create(fireFX.c_str(), _audioEngine, 0.05f); // we need a global music & sfx volumes
	// add prefab to ECS
	auto doublePrefab = _game->prefab()

		// .set<> in a prefab means components are shared (instanced)


		.set<Acceleration>({ 0, 0,0 })
		.set<Velocity>({ speed,0,0 })
		.set<ESG::Mesh>(*mesh)
		.set<GW::AUDIO::GSound>(shoot.Relinquish())
		.set<ESG::ForwardDirection>({ forwardDirection })
		.set<ESG::Model>({ *model })
		.set_override<Damage>({ dmg })
		.set_override<ESG::ModelTransform>(*modelTransform)
		.set_override<ESG::Instance>(*inst)
		.override<Missiles>()
		.override<Projectile>()
		.override<Collidable>()
		.set_override<ModelBoundry>(*modelBoundary)

	.set<prefabName>({ "Double" });;
		

	// register this prefab by name so other systems can use it
	RegisterPrefab("double", doublePrefab);
	
	return true;
}

bool ESG::DoubleData::Unload(std::shared_ptr<flecs::world> _game)
{
	// remove all bullets and their prefabs
	_game->defer_begin(); // required when removing while iterating!
	_game->each([](flecs::entity e, Bullet&) {
		e.destruct(); // destroy this entitiy (happens at frame end)
	});
	_game->defer_end(); // required when removing while iterating!

	// unregister this prefab by name
	//UnregisterPrefab("Lazer Bullet");

	return true;
}
