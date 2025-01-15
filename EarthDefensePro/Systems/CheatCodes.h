// The Firing Timer system is responsible for tracking time for projectile based attacks

#ifndef CHEATCODES_H
#define CHEATCODES_H

// Contains our global game settings
#include "../GameConfig.h"


// example space game (avoid name collisions)
namespace ESG
{
	class CheatCodes
	{
		// shared connection to the main ECS engine
		std::shared_ptr<flecs::world> game;
		// non-ownership handle to configuration settings
		std::weak_ptr<const GameConfig> gameConfig;
		GW::INPUT::GInput immediateInput;
	public:
		// attach the required logic to the ECS 
		bool Init(std::shared_ptr<flecs::world>& _game,
			std::weak_ptr<const GameConfig> _gameConfig,
			GW::INPUT::GInput _immediateInput,
			GW::INPUT::GBufferedInput _bufferedInput,
			GW::INPUT::GController _controllerInput,
			GW::AUDIO::GAudio _audioEngine,
			GW::CORE::GEventGenerator _eventPusher);
		// control if the system is actively running
		bool Activate(bool runSystem);
		// release any resources allocated by the system
		bool Shutdown();
	};

};

#endif