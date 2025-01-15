#ifndef POWERUP_H
#define POWERUP_H

#include "../GameConfig.h"

namespace ESG
{
	class PowerUp
	{
		std::shared_ptr<flecs::world> game;
		std::weak_ptr<const GameConfig> gameConfig;

		GW::INPUT::GInput* immediateInput;
		GW::INPUT::GController* controllerInput;

		GW::CORE::GThreadShared pointLock;

		unsigned int powerPoints;
		unsigned int powerMult;


	public:
		bool Init(std::shared_ptr<flecs::world>& _game,
			std::weak_ptr<const GameConfig> _gameConfig,
			GW::INPUT::GInput* _immediateInput,
			GW::INPUT::GController* _controllerInput);
		bool Activation(bool runSystem);
		void UpPoints(unsigned int valueUp);
		int GetPoints();
		bool Shutdown();
	};

};

#endif
