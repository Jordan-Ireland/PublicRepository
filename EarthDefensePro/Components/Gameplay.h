// define all ECS components related to gameplay
#ifndef GAMEPLAY_H
#define GAMEPLAY_H
#include "../Systems/LevelScroll.h"

// example space game (avoid name collisions)
namespace ESG
{
	struct Damage { int value; };
	struct Health { int value; };
	struct Firerate { float value; };
	struct Cooldown { float value; };
	struct LevelScrollComp { float value; ESG::LevelScroll* levelScroll; bool levelScrolling = true; };
	// gameplay tags (states)
	struct Firing {};
	struct Charging {};
	struct Select{};
	struct Invulnerable { float timeLeft = 0; };

	// powerups
	struct ChargedShot { int max_destroy; };
	struct Missiles{};
	struct Double{};
	struct Lasers{};
	struct Beams{};
	struct FireTimer { float countdown; };
	struct Speed { float speed = 1.0f; unsigned int counter = 0; };
	struct Option{};

	//UI Stuff
	struct Lives { int value;};
	struct PowerMeterTokens { int value = 3;};
	struct token{};
	struct playerScore { unsigned int score; };
	struct increaseLives{};
	struct decreaseLives{};
	struct Options{};
	struct Highscore { unsigned int score1; unsigned int score2; };

	//cheat code
	struct Input{ int key; bool pressed; };
	struct Codes { int keys[11]; int index; };

};

#endif