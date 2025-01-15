#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "../GameConfig.h"
#include "../Components/Identification.h"
#include "../Components/Blender.h"
#include "../Components/Gameplay.h"
#include "../Components/Physics.h"


namespace ESG
{
	class Collisions
	{
		std::shared_ptr<flecs::world> game;
		std::weak_ptr<const GameConfig> gameConfig;
		flecs::query<ESG::Enemy, ESG::ModelBoundry, ESG::Collidable> enemyQ;
		flecs::query<ESG::Missiles, ESG::ModelBoundry> missileQ;
		flecs::query<ESG::Double, ESG::ModelBoundry> doubleQ;
		flecs::query<ESG::Bullet, ESG::ModelBoundry> bulletQ;
		flecs::query<ESG::Lasers, ESG::ModelBoundry> laserQ;
		flecs::query<ESG::Beams, ESG::ModelBoundry> beamQ;
		flecs::query<CollideWithPlayer, ESG::ModelBoundry> withPlayerQ;
		flecs::query<ESG::Collidable> collidableQ;
		flecs::query<ESG::Projectile, ESG::ModelBoundry> projectileQ;
	public:
		bool Init(std::shared_ptr<flecs::world> _game,
			std::weak_ptr<const GameConfig> _gameConfig);
		bool Activation(bool runSystem);
		bool Shutdown();
	};
};
#endif
