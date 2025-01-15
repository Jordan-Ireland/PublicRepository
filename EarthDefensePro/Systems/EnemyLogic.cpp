#include <random>
#include "EnemyLogic.h"

using namespace ESG; // Example Space Game

// Connects logic to traverse any players and allow a controller to manipulate them
bool ESG::EnemyLogic::Init(std::shared_ptr<flecs::world>& _game,
	std::weak_ptr<const GameConfig> _gameConfig,
	GW::CORE::GEventGenerator _eventPusher)
{
	// save a handle to the ECS & game settings
	game = _game;
	gameConfig = _gameConfig;
	eventPusher = _eventPusher;

	// destroy any bullets that have the CollidedWith relationship
	game->system<Enemy, Health, ESG::ModelTransform, ESG::Renderable>("Enemy System")
		.each([this](flecs::entity e, Enemy&, Health& h, ESG::ModelTransform& t, ESG::Renderable&) {
		auto player1 = game->lookup("Spaceship_RaeTheRedPanda");
		auto player1Pos = player1.get<ESG::ModelTransform>();
		auto name = e.get<ESG::BlenderName>();
		auto enemy = e.get_mut<ESG::Enemy>();
		if (player1Pos != nullptr) {
			auto t = e.get_mut<ESG::ModelTransform>();
			float dis = distance({ player1Pos->matrix.row4.x, 0 }, { t->matrix.row4.x, 0 });
			const ESG::Velocity* vel = e.get<ESG::Velocity>();
			const ESG::Acceleration* acc = e.get<ESG::Acceleration>();
			if (enemy->moves) {
				if (enemy->timeSinceSpawn <= 12) {
					if (dis <= 10) {
						if (enemy->attacks && enemy->timeSinceShoot >= enemy->timeBetween) {
							FireBullets(game->get_world(), *t);
							enemy->timeSinceShoot = 0;
						}
						enemy->timeSinceSpawn += e.delta_time();
						t->matrix.row4.x += sinewave(enemy->curSine, 2) * e.delta_time();
						enemy->curSine += e.delta_time() * 2;
					}


					/*if (dis <= 5) {
						auto levelScroll = game->lookup("Level Scroll Speed").get_mut<ESG::LevelScrollComp>();
						float wantedPosX = t->matrix.row4.x + e.delta_time() * vel->value.x + (levelScroll->value * e.delta_time());
						t->matrix.row4.x = lerp(t->matrix.row4.x, wantedPosX, e.delta_time() * acc->value.x);
					}*/


					t->matrix.row4.y += e.delta_time() * vel->value.y * enemy->dir;
					if (t->matrix.row4.y >= 5) enemy->dir = -1;
					else if (t->matrix.row4.y <= -5) enemy->dir = 1;
				}
				else {
					t->matrix.row4.y = lerp(t->matrix.row4.y, t->matrix.row4.y + e.delta_time() * vel->value.x, e.delta_time() * acc->value.x);

					t->matrix.row4.x = lerp(t->matrix.row4.x, t->matrix.row4.x - e.delta_time() * vel->value.x, e.delta_time() * acc->value.x);

					enemy->timeSinceSpawn += e.delta_time();
					if (enemy->timeSinceSpawn >= 12) e.destruct();
				}
			}
			else {
				enemy->timeSinceSpawn += e.delta_time();
				t->matrix.row4.x += sinewave(enemy->curSine, 2) * e.delta_time();
				t->matrix.row4.y += sinewave(enemy->curSine - 360, 2) * e.delta_time();
				enemy->curSine += e.delta_time() * 2;
			}


			//AdjustForBoundaries(*t);
		}

		if (e.has<ESG::EnemyHit>()) {
			auto health = e.get_mut<Health>();
			auto sounds = e.get<ESG::SoundFX>();
			health->value--;
			GW::AUDIO::GSound hurt = sounds->hurtSFX;
			hurt.Play();
			e.remove<ESG::EnemyHit>();
			e.modified<Health>();
		}

		// if you have no health left be destroyed
		if (h.value <= 0) {
			// play explode sound
			auto sounds = e.get<ESG::SoundFX>();
			ESG::PEventGenerator::EVENT_DATA x{ sounds->deathSFX.c_str(),t.matrix.row4 };
			GW::GEvent explode;
			explode.Write(ESG::PEventGenerator::Events::ENEMY_DESTROYED, x);
 			eventPusher.Push(explode);
			e.destruct();
		} 		

		enemy->timeSinceShoot += e.delta_time();
		e.modified<ESG::Enemy>();
	});

	return true;
}

bool ESG::EnemyLogic::FireBullets(flecs::world& stage, ESG::ModelTransform origin)
{
	// Grab the prefab for a laser round
	flecs::entity bulletClone;
	auto b = RetreivePrefab("Enemy Basic Bullet", bulletClone);


	auto& mt = *bulletClone.get_mut<ESG::ModelTransform>();
	auto inst = bulletClone.get_mut<ESG::Instance>();

	auto lvl = game->lookup("LevelData");



	//std::cout << "Bullet" << stage.count<ESG::Bullet>();


	bool debugON = false;
	origin.matrix.row4.x -= 0.2f;
	origin.matrix.row4.z -= 0;
	origin.matrix.row4.y -= 0.1f;
	mt.matrix.row4 = origin.matrix.row4;
	//	inst->transformCount = stage.count<ESG::Bullet>()+ 1;
		//inst->transformCount +=1;

		//origin.value.y += 0.0f;
	auto bBullet = stage.entity().is_a(bulletClone)
		.set_override<ESG::ModelTransform>(mt);

	//.set<ESG::Instance>(*inst);
	//.add<ESG::Renderable>()
	//.add<ESG::Bullet>();
	if (debugON)
	{
		std::cout << "Bullet X: " << origin.matrix.row4.x << std::endl;
		std::cout << "Bullet Y: " << origin.matrix.row4.y << std::endl;
		std::cout << "Bullet Z: " << origin.matrix.row4.z << std::endl;
	}




	// play the sound of the Lazer prefab
	GW::AUDIO::GSound shoot = *bulletClone.get<GW::AUDIO::GSound>();
	shoot.Play();

	return true;
}

// Free any resources used to run this system
bool ESG::EnemyLogic::Shutdown()
{
	game->entity("Enemy System").destruct();
	// invalidate the shared pointers
	game.reset();
	gameConfig.reset();
	return true;
}

// Toggle if a system's Logic is actively running
bool ESG::EnemyLogic::Activate(bool runSystem)
{
	if (runSystem) {
		game->entity("Enemy System").enable();
	}
	else {
		game->entity("Enemy System").disable();
	}
	return false;
}

void ESG::EnemyLogic::AdjustForBoundaries(ESG::ModelTransform& transform) {
	transform.matrix.row4.x = G_LARGER(transform.matrix.row4.x, -9.8f);
	transform.matrix.row4.x = G_SMALLER(transform.matrix.row4.x, +9.8f);
	transform.matrix.row4.y = G_LARGER(transform.matrix.row4.y, -150.0f);
	transform.matrix.row4.y = G_SMALLER(transform.matrix.row4.y, +150.0f);
}
