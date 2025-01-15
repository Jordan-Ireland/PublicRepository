// define all ECS components related to identification
#ifndef IDENTIFICATION_H
#define IDENTIFICATION_H

// example space game (avoid name collisions)
namespace ESG
{
	struct Player {};
	struct Projectile { bool friendly = true; };
	struct EnemyProjectile{};
	struct Bullet {};
	struct Enemy {
		float timeBetween;
		bool attacks = true;
		bool moves = true;
		float timeSinceShoot = 0;
		int8_t dir = rand() % 2 - 1;
		float curSine = static_cast<float>(rand() % 20);
		float timeSinceSpawn = 0;
	};
	struct Boss {
		float timeBetween;
		float timeSinceShoot = 0;
		float curSine = static_cast<float>(rand() % 20);
	};
	struct ControllerID {
		unsigned index = 0;
	};
	struct PowerPickup{};
	struct Shield {
		unsigned int charges;
	};
	struct ShieldTag{};
	//struct Lazers{};
	struct SlatedForDeath{};
	struct CollideWithPlayer{};
	struct PlayerHit{};
	struct EnemyHit {};

	struct SoundFX {
		GW::AUDIO::GSound hurtSFX;
		std::string deathSFX;
	};
};

#endif