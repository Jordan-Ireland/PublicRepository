#include <random>
#include "CheatCodes.h"
#include "../Components/Identification.h"
#include "../Components/Physics.h"
#include "../Components/Gameplay.h"

using namespace ESG; // Example Space Game

// Connects logic to traverse any players and allow a controller to manipulate them
bool ESG::CheatCodes::Init(std::shared_ptr<flecs::world>& _game,
	std::weak_ptr<const GameConfig> _gameConfig,
	GW::INPUT::GInput _immediateInput,
	GW::INPUT::GBufferedInput _bufferedInput,
	GW::INPUT::GController _controllerInput,
	GW::AUDIO::GAudio _audioEngine,
	GW::CORE::GEventGenerator _eventPusher)
{
	// save a handle to the ECS & game settings
	game = _game;
	gameConfig = _gameConfig;
	immediateInput = _immediateInput;
	// destroy any bullets that have the CollidedWith relationship
	game->system<ESG::Input, ESG::CheatCodes>("CheatCodes Timer System")
		.iter([this](flecs::iter it, ESG::Input* input, ESG::CheatCodes* cc){
		
		for (auto i :it)
		{
		

		//	immediateInput.GetState(it.entity(i).get<ESG::Input>(),)
		}

			
	
			});

	return true;
}

// Free any resources used to run this system
bool ESG::CheatCodes::Shutdown()
{
	game->entity("CheatCodes  System").destruct();
	// invalidate the shared pointers
	game.reset();
	gameConfig.reset();
	return true;
}

// Toggle if a system's Logic is actively running
bool ESG::CheatCodes::Activate(bool runSystem)
{
	if (runSystem) {
		game->entity("CheatCodes  System").enable();
	}
	else {
		game->entity("Firing  System").disable();
	}
	return false;
}
