// This class populates all player entities 
#ifndef PLAYERDATA_H
#define PLAYERDATA_H

// Contains our global game settings
#include "../GameConfig.h"
#include "../Components/Identification.h"
#include "../Components/Visuals.h"
#include "../Components/Physics.h"
#include "Prefabs.h"
#include "../Components/Blender.h"
#include "../Components/Gameplay.h"
// example space game (avoid name collisions)
namespace ESG
{
	class PlayerData
	{
	public:
		// Load required entities and/or prefabs into the ECS 
		bool Load(	std::shared_ptr<flecs::world>& _game,
					std::weak_ptr<const GameConfig> _gameConfig);
		// Unload the entities/prefabs from the ECS
		bool Unload(std::shared_ptr<flecs::world>& _game);
	private:
		bool ReadyPlayerOne(std::shared_ptr<flecs::world>& _game,
			std::weak_ptr<const GameConfig> _gameConfig);
		bool ReadyPlayerTwo(std::shared_ptr<flecs::world>& _game,
			std::weak_ptr<const GameConfig> _gameConfig);
	};

};

#endif