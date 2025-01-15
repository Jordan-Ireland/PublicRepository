#include "PowerTokenData.h"
#include "../Components/Identification.h"
#include "../Components/Visuals.h"
#include "../Components/Physics.h"
#include "Prefabs.h"
#include "../Components/Gameplay.h"
#include "../Components/Blender.h"

bool ESG::PowerTokenData::Load(	std::shared_ptr<flecs::world> _game,
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
	float speed = (*readCfg).at("Tokens").at("speed").as<float>();
	float xscale = (*readCfg).at("Tokens").at("xscale").as<float>();
	float yscale = (*readCfg).at("Tokens").at("yscale").as<float>();
	float zscale = (*readCfg).at("Tokens").at("zscale").as<float>();
	int dmg = (*readCfg).at("Tokens").at("damage").as<int>();
	float forwardDirection = (*readCfg).at("Tokens").at("direction").as<int>();
	std::string sound = (*readCfg).at("Tokens").at("sound").as<std::string>();

	// default projectile scale
	GW::MATH::GMATRIXF world;
	auto e = _game->lookup("Thunder");
	//e.add<ESG::InstanceTag>();
	
		auto inst = e.get_mut<ESG::Instance>();
		auto mesh = e.get<ESG::Mesh>();
		auto modelTransform = e.get_mut<ESG::ModelTransform>();
		auto model = e.get_mut<ESG::Model>();
		auto modelBoundary = e.get_mut<ESG::ModelBoundry>();


		modelBoundary->obb.extent.x *= xscale;
		modelBoundary->obb.extent.y *= yscale;
		modelBoundary->obb.extent.z *= zscale;

		GW::MATH::GVECTORF s;
		GW::MATH::GMatrix::GetScaleF(modelTransform->matrix, s);

		s.x = xscale;
		s.z = zscale;
		s.y = yscale;

		GW::MATH::GMatrix::ScaleGlobalF(modelTransform->matrix, s, modelTransform->matrix);


		modelBoundary->obb.extent.x *= xscale;
		modelBoundary->obb.extent.y *= yscale;
		modelBoundary->obb.extent.z *= zscale;
	// Load sound effect used by this bullet prefab
	GW::AUDIO::GSound shoot;
	shoot.Create(sound.c_str(), _audioEngine, 0.05f); // we need a global music & sfx volumes
	// add prefab to ECS
	auto tokenPrefab = _game->prefab()

		// .set<> in a prefab means components are shared (instanced)


		
		.set<ESG::Mesh>(*mesh)
		.set<GW::AUDIO::GSound>(shoot.Relinquish())
		.set<ESG::Model>({ *model })
		.set_override<ESG::ModelTransform>(*modelTransform)
		.set_override<ESG::Instance>(*inst)
		.override<token>()
		.override<Collidable>() //uncomment this if you want to shoot the tokens
		.set<prefabName>({ "Token" })
		.set<ESG::ModelBoundry>({*modelBoundary })
		
		//.override<ESG::Renderable>()// Tag this prefab as a bullet (for queries/systems)
		.override<Collidable>(); // can be collided with

	// register this prefab by name so other systems can use it
	RegisterPrefab("Power Token", tokenPrefab);
	e.destruct();
	return true;
}

bool ESG::PowerTokenData::Unload(std::shared_ptr<flecs::world> _game)
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
