#include "EnemyData.h"
#include "../Components/Identification.h"
#include "../Components/Visuals.h"
#include "../Components/Physics.h"
#include "Prefabs.h"
#include "../Components/Gameplay.h"
#include "../Components/Blender.h"

bool ESG::EnemyData::Load(	std::shared_ptr<flecs::world> _game,
							std::weak_ptr<const GameConfig> _gameConfig,
							GW::AUDIO::GAudio _audioEngine)
{
	// Grab init settings for players
	readCfg = _gameConfig.lock();
	audioEngine = _audioEngine;

	// Create prefab for lazer weapon
	// color
	float red = (*readCfg).at("Enemy1").at("red").as<float>();
	float green = (*readCfg).at("Enemy1").at("green").as<float>();
	float blue = (*readCfg).at("Enemy1").at("blue").as<float>();
	// other attributes
	float xscale = (*readCfg).at("Enemy1").at("xscale").as<float>();
	float yscale = (*readCfg).at("Enemy1").at("yscale").as<float>();
	float angle = (*readCfg).at("Enemy1").at("angle").as<float>();
	int health = (*readCfg).at("Enemy1").at("health").as<int>();
	float timeBetweenShots = (*readCfg).at("Enemy1").at("timeBetweenShots").as<float>();
	
	// default projectile orientation & scale
	GW::MATH::GMATRIXF world;
	GW::MATH::GMatrix::RotateYGlobalF(GW::MATH::GIdentityMatrixF, 
		G_DEGREE_TO_RADIAN_F(angle), world);
	GW::MATH::GMatrix::ScaleGlobalF(world,
		GW::MATH::GVECTORF{ xscale, yscale*2,xscale}, world);

	SetData(_game, health, xscale, yscale, timeBetweenShots);

	//auto e = _game->lookup("EnemyShip.001");

	//e.add<ESG::InstanceTag>();

	//auto b = e.get<ESG::BlenderName>()->name;
	//auto inst = e.get_mut<ESG::Instance>();
	//auto mesh = e.get<ESG::Mesh>();
	//auto modelTransform = e.get_mut<ESG::ModelTransform>();
	//auto model = e.get_mut<ESG::Model>();
	//auto modelBoundary = e.get_mut<ESG::ModelBoundry>();

	//
	//// add prefab to ECS
	//auto enemyPrefab = _game->prefab("EnemyShip")
	//	// .set<> in a prefab means components are shared (instanced)
	//	.set<Material>({ red, green, blue })
	//	.set<Orientation>({ world })
	//	// .override<> ensures a component is unique to each entity created from a prefab
	//	.set_override<Health>({ health })
	//	.override<Acceleration>()
	//	.override<Velocity>()
	//	.override<Position>()
	//	.override<Enemy>() // Tag this prefab as an enemy (for queries/systems)
	//	.override<Collidable>(); // can be collided with

	//// register this prefab by name so other systems can use it
	//RegisterPrefab("Enemy Type1", enemyPrefab);

	return true;
}

bool ESG::EnemyData::SetData(std::shared_ptr<flecs::world> _game, int _health, float _xscale, float _yscale, float _timeBetween) {
	auto lvl = _game->lookup("LevelData").get<ESG::lvlData>();
	for (size_t i = 0; i < lvl->levelData.blenderObjects.size(); i++)
	{
		auto name = std::string(lvl->levelData.blenderObjects[i].blendername);
		if (name.find("EnemyShip") != name.npos || name.find("Drone") != name.npos) {
			std::string hurtFX = (*readCfg).at("Enemy1").at("hurtsfx").as<std::string>();
			std::string deathFX = (*readCfg).at("Enemy1").at("deathsfx").as<std::string>();
			GW::AUDIO::GSound hurt;
			hurt.Create(hurtFX.c_str(), audioEngine, 0.05f); // we need a global music & sfx volumes

			//GW::AUDIO::GSound death;
			//death.Create(deathFX.c_str(), audioEngine, 0.05f); // we need a global music & sfx volumes

			auto n = _game->lookup(name.c_str());
			n.set<ESG::Health>({ _health })
				.set<Acceleration>({ 5 })
				.set<Velocity>({ rand() % 5 - 2.5f, 2 })
				.set<Position>({ n.get_mut<ESG::ModelTransform>()->matrix.row4 })
				.set<Enemy>({ _timeBetween }) // Tag this prefab as an enemy (for queries/systems)
				.add<Collidable>() // can be collided with
				.add<ESG::Renderable>()
				.set<ESG::SoundFX>({ hurt.Relinquish(), deathFX });

			auto e = n.get_mut<ESG::Enemy>();
			e->dir = e->dir == 0 ? 1 : e->dir;
			e->attacks =  rand() % 100 < 30 ? true : false;

			n.modified<ESG::Enemy>();
		}
		else if (name.find("Mine") != name.npos) {
			std::string hurtFX = (*readCfg).at("Enemy1").at("hurtsfx").as<std::string>();
			std::string deathFX = (*readCfg).at("Enemy1").at("deathsfx").as<std::string>();
			GW::AUDIO::GSound hurt;
			hurt.Create(hurtFX.c_str(), audioEngine, 0.05f); // we need a global music & sfx volumes

			//GW::AUDIO::GSound death;
			//death.Create(deathFX.c_str(), audioEngine, 0.05f); // we need a global music & sfx volumes

			auto n = _game->lookup(name.c_str());
			n.set<ESG::Health>({ 100 })
				.set<Acceleration>({ 5 })
				.set<Velocity>({ rand() % 5 - 2.5f, 2 })
				.set<Position>({ n.get_mut<ESG::ModelTransform>()->matrix.row4 })
				.set<Enemy>({ _timeBetween, false, false }) // Tag this prefab as an enemy (for queries/systems)
				.add<Collidable>() // can be collided with
				.add<ESG::Renderable>()
				.set<ESG::SoundFX>({ hurt.Relinquish(), deathFX });
		}
	}

	return true;
}

bool ESG::EnemyData::Unload(std::shared_ptr<flecs::world> _game)
{
	// remove all bullets and their prefabs
	_game->defer_begin(); // required when removing while iterating!
	_game->each([](flecs::entity e, Enemy&) {
		e.destruct(); // destroy this entitiy (happens at frame end)
	});
	_game->defer_end(); // required when removing while iterating!

	return true;
}
