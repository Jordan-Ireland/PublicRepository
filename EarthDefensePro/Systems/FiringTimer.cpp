#include <random>
#include "FiringTimer.h"
#include "../Components/Identification.h"
#include "../Components/Physics.h"
#include "../Components/Gameplay.h"

using namespace ESG; // Example Space Game

// Connects logic to traverse any players and allow a controller to manipulate them
bool ESG::FiringTimer::Init(std::shared_ptr<flecs::world>& _game,
	std::weak_ptr<const GameConfig> _gameConfig)
{
	// save a handle to the ECS & game settings
	game = _game;
	gameConfig = _gameConfig;
	
	// destroy any bullets that have the CollidedWith relationship
	game->system<ESG::FireTimer>("Firing Timer System")
		.iter([this](flecs::iter it, FireTimer* f){
		
		for (auto i :it)
		{
			f[i].countdown -= it.entity(i).delta_time();
			
			if (f[i].countdown <= 0 && it.entity(i).is_alive())
			{
				
				it.entity(i).destruct();

			}
		}

			
			
		
	
			});

	return true;
}

// Free any resources used to run this system
bool ESG::FiringTimer::Shutdown()
{
	game->entity("Firing Timer System").destruct();
	// invalidate the shared pointers
	game.reset();
	gameConfig.reset();
	return true;
}

// Toggle if a system's Logic is actively running
bool ESG::FiringTimer::Activate(bool runSystem)
{
	if (runSystem) {
		game->entity("Firing Timer System").enable();
	}
	else {
		game->entity("Firing Timer System").disable();
	}
	return false;
}
