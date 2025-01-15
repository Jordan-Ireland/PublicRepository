#include "BulletData.h"
#include "../Components/Identification.h"
#include "../Components/Visuals.h"
#include "../Components/Physics.h"
#include "Prefabs.h"
#include "../Components/Gameplay.h"
#include "../Components/Blender.h"

bool ESG::BulletData::Load(std::shared_ptr<flecs::world> _game,
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
	float speed = (*readCfg).at("Bullets").at("speed").as<float>();
	float xscale = (*readCfg).at("Bullets").at("xscale").as<float>();
	float yscale = (*readCfg).at("Bullets").at("yscale").as<float>();
	float zscale = (*readCfg).at("Bullets").at("zscale").as<float>();
	int dmg = (*readCfg).at("Bullets").at("damage").as<int>();
	float forwardDirection = (*readCfg).at("Bullets").at("direction").as<int>();
	std::string fireFX = (*readCfg).at("Bullets").at("fireFX").as<std::string>();
	std::string eFireFX = (*readCfg).at("Enemy1Bullet").at("fireFX").as<std::string>();

	// default projectile scale
	GW::MATH::GMATRIXF world;
	auto e = _game->lookup("Xullet");
	//e.add<ESG::InstanceTag>();


	
		auto inst = e.get_mut<ESG::Instance>();
		auto mesh = e.get<ESG::Mesh>();
		auto modelTransform = e.get_mut<ESG::ModelTransform>();
		auto modelBoundary = e.get_mut<ESG::ModelBoundry>();
		auto model = e.get_mut<ESG::Model>();

		modelBoundary->obb.extent.x *= xscale * 2;
		modelBoundary->obb.extent.y *= yscale * 2;
		modelBoundary->obb.extent.z *= zscale * 2;

		GW::MATH::GVECTORF s;
		GW::MATH::GMatrix::GetScaleF(modelTransform->matrix, s);

		s.x = xscale;
		s.z = zscale;
		s.y = yscale;

		GW::MATH::GMatrix::ScaleGlobalF(modelTransform->matrix, s, modelTransform->matrix);

		/*std::cout << modelTransform->matrix.row1.x<< " "<< modelTransform->matrix.row1.y << " " << modelTransform->matrix.row1.z << " " << modelTransform->matrix.row1.w << std::endl;
		std::cout << modelTransform->matrix.row2.x << " " << modelTransform->matrix.row2.y << " " << modelTransform->matrix.row2.z << " " << modelTransform->matrix.row2.w << std::endl;
		std::cout << modelTransform->matrix.row3.x << " " << modelTransform->matrix.row3.y << " " << modelTransform->matrix.row3.z << " " << modelTransform->matrix.row3.w << std::endl;
		std::cout << modelTransform->matrix.row4.x << " " << modelTransform->matrix.row4.y << " " << modelTransform->matrix.row4.z << " " << modelTransform->matrix.row4.w << std::endl;
		modelTransform->matrix.row1.x = xscale;
		modelTransform->matrix.row2.z = zscale;
		modelTransform->matrix.row3.y = yscale;
		std::cout << modelTransform->matrix.row1.x << " " << modelTransform->matrix.row1.y << " " << modelTransform->matrix.row1.z << " " << modelTransform->matrix.row1.w << std::endl;
		std::cout << modelTransform->matrix.row2.x << " " << modelTransform->matrix.row2.y << " " << modelTransform->matrix.row2.z << " " << modelTransform->matrix.row2.w << std::endl;
		std::cout << modelTransform->matrix.row3.x << " " << modelTransform->matrix.row3.y << " " << modelTransform->matrix.row3.z << " " << modelTransform->matrix.row3.w << std::endl;
		std::cout << modelTransform->matrix.row4.x << " " << modelTransform->matrix.row4.y << " " << modelTransform->matrix.row4.z << " " << modelTransform->matrix.row4.w << std::endl;*/

	// Load sound effect used by this bullet prefab
	GW::AUDIO::GSound shoot;
	shoot.Create(fireFX.c_str(), _audioEngine, 0.05f); // we need a global music & sfx volumes
	// add prefab to ECS
	auto bulletPrefab = _game->prefab()
		.set<Acceleration>({ 0, 0,0 })
		.set<Velocity>({ speed,0,0 })
		.set<ESG::Mesh>(*mesh)
		.set<GW::AUDIO::GSound>(shoot.Relinquish())
		.set<ESG::ForwardDirection>({ forwardDirection })
		.set<ESG::Model>({ *model })
		//.set<ESG::Instance>(*inst)
		// .override<> ensures a component is unique to each entity created from a prefab 
		.set_override<Damage>({ dmg })
		//.set_override<ChargedShot>({ 2 })
		.set_override<ESG::ModelTransform>(*modelTransform)
		.set_override<ESG::Instance>(*inst)
		.override<Bullet>()
		.override<Projectile>()
		.override<Collidable>()
		.set_override<ModelBoundry>(*modelBoundary)
		.set<prefabName>({ "Bullet" })
		.set_override<FireTimer>({ 2.0f });
	//.remove<ESG::SingleInstance>();

	//.override<ESG::Renderable>()// Tag this prefab as a bullet (for queries/systems)
	//.override<Collidable>(); // can be collided with

// register this prefab by name so other systems can use it
	RegisterPrefab("Basic Bullet", bulletPrefab);

	GW::AUDIO::GSound enemyShoot;
	enemyShoot.Create(eFireFX.c_str(), _audioEngine, 0.05f);
	auto enemyBulletPrefab = _game->prefab()
		.set<Acceleration>({ 0, 0,0 })
		.set<Velocity>({ -speed,0,0 })
		.set<ESG::Mesh>(*mesh)
		.set<GW::AUDIO::GSound>(enemyShoot.Relinquish())
		.set<ESG::ForwardDirection>({ -forwardDirection })
		.set<ESG::Model>({ *model })
		.set_override<Damage>({ dmg })
		.set_override<ESG::ModelTransform>(*modelTransform)
		.set_override<ESG::Instance>(*inst)
		.override<Bullet>()
		.set_override<Projectile>({ false })
		.add<ESG::CollideWithPlayer>()
		.override<Collidable>()
		.set_override<ModelBoundry>(*modelBoundary)
		.set<prefabName>({ "EnemyBullet" })
		.set_override<FireTimer>({ 5.0f });


	RegisterPrefab("Enemy Basic Bullet", enemyBulletPrefab);

	return true;
}

bool ESG::BulletData::Unload(std::shared_ptr<flecs::world> _game)
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
