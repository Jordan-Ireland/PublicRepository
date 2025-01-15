#include "ShieldData.h"
#include "../Components/Identification.h"
#include "../Components/Visuals.h"
#include "../Components/Physics.h"
#include "Prefabs.h"
#include "../Components/Gameplay.h"
#include "../Components/Blender.h"


bool ESG::ShieldData::Load(std::shared_ptr<flecs::world> _game,
	std::weak_ptr<const GameConfig> _gameConfig,
	GW::AUDIO::GAudio _audioEngine)
{
	// Grab init settings for players
	std::shared_ptr<const GameConfig> readCfg = _gameConfig.lock();
	_game;

	// other attributes
	float speed = (*readCfg).at("Shield").at("speed").as<float>();
	float xscale = (*readCfg).at("Shield").at("xscale").as<float>();
	float yscale = (*readCfg).at("Shield").at("yscale").as<float>();
	float zscale = (*readCfg).at("Shield").at("zscale").as<float>();
	//int dmg = (*readCfg).at("Shield").at("damage").as<int>();
	float forwardDirection = (*readCfg).at("Shield").at("direction").as<int>();
	std::string fireFX = (*readCfg).at("Shield").at("fireFX").as<std::string>();

	// default projectile scale
	GW::MATH::GMATRIXF world;
	auto e = _game->lookup("Xullet");



	auto inst = e.get_mut<ESG::Instance>();
	auto mesh = e.get<ESG::Mesh>();
	auto modelTransform = e.get_mut<ESG::ModelTransform>();
	auto modelBoundary = e.get_mut<ESG::ModelBoundry>();
	auto model = e.get_mut<ESG::Model>();

	modelBoundary->obb.extent.x *= xscale;
	modelBoundary->obb.extent.y *= yscale;
	modelBoundary->obb.extent.z *= zscale;

	GW::MATH::GVECTORF s;
	GW::MATH::GMatrix::GetScaleF(modelTransform->matrix, s);

	s.x = xscale;
	s.z = zscale;
	s.y = yscale;

	GW::MATH::GMatrix::ScaleGlobalF(modelTransform->matrix, s, modelTransform->matrix);

	
	// Load sound effect used by this bullet prefab
	

	//GW::AUDIO::GSound shoot;
	//shoot.Create(fireFX.c_str(), _audioEngine, 0.05f); // we need a global music & sfx volumes
	// add prefab to ECS

	auto shieldPrefab = _game->prefab()

		// .set<> in a prefab means components are shared (instanced)


		.set<Acceleration>({ 0, 0,0 })
		.set<Velocity>({ speed,0,0 })
		.set<ESG::Mesh>(*mesh)
		//.set<GW::AUDIO::GSound>(shoot.Relinquish())
		.set<ESG::ForwardDirection>({ forwardDirection })
		.set<ESG::Model>({ *model })
		//.set<ESG::Instance>(*inst)
		// .override<> ensures a component is unique to each entity created from a prefab
		//.set_override<ChargedShot>({ 2 })
		.set_override<ESG::ModelTransform>(*modelTransform)
		.set_override<ESG::Instance>(*inst)
		.add<ShieldTag>()
		//.override<Bullet>()
		//.override<Projectile>()
		//.override<Collidable>()
		.set_override<ModelBoundry>(*modelBoundary)
		.set<prefabName>({ "Shield" });
		//.set_override<FireTimer>({ 2.0f });
	//.remove<ESG::SingleInstance>();

	//.override<ESG::Renderable>()// Tag this prefab as a bullet (for queries/systems)
	//.override<Collidable>(); // can be collided with

// register this prefab by name so other systems can use it
	RegisterPrefab("Shield", shieldPrefab);

	return true;
}

bool ESG::ShieldData::Unload(std::shared_ptr<flecs::world> _game)
{
	// remove all bullets and their prefabs
	_game->defer_begin(); // required when removing while iterating!
	_game->each([](flecs::entity e, ShieldTag&) {
		e.destruct(); // destroy this entitiy (happens at frame end)
		});
	_game->defer_end(); // required when removing while iterating!

	// unregister this prefab by name
	UnregisterPrefab("Shield");

	return true;
}