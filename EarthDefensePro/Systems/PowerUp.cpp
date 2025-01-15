
#include "PowerUp.h"
#include "LevelLogic.h"

bool ESG::PowerUp::Init(std::shared_ptr<flecs::world>& _game,
	std::weak_ptr<const GameConfig> _gameConfig,
	GW::INPUT::GInput* _immediateInput,
	GW::INPUT::GController* _controllerInput)
{
	game = _game;
	gameConfig = _gameConfig;
	powerPoints = 0;

	immediateInput = _immediateInput;
	controllerInput = _controllerInput;

	pointLock.Create();

	std::shared_ptr<const GameConfig> readCfg = gameConfig.lock();
	auto levelSys = game->lookup("Level System");
	auto levelSysData = levelSys.get_mut<LevelLogic>();
	//TODO Add to ini
	//powerMult = (*readCfg).at(levelSysData->GetLevelGroup()).at("powermult").as<int>();

	struct PowerCollision {};
	struct PowerSystem {};
	auto players = game->query<ESG::Player, ESG::ModelBoundry>();

	game->entity("Power Collision").add<PowerCollision>();

	game->system<token, const ModelBoundry>()
		.each([&,this](flecs::entity e, token&, const ModelBoundry& b) {
		
		auto player1 = game->lookup("Spaceship_RaeTheRedPanda");
		auto player1Hit = player1.get<ESG::ModelBoundry>();

		auto player2 = game->lookup("Ship");
		auto player2Hit = player2.get<ESG::ModelBoundry>();

		GW::MATH::GCollision::GCollisionCheck checkPlayer1;
		GW::MATH::GCollision::GCollisionCheck checkPlayer2;

		GW::MATH::GCollision::TestOBBToOBBF(b.obb, player1Hit->obb, checkPlayer1);
		GW::MATH::GCollision::TestOBBToOBBF(b.obb, player2Hit->obb, checkPlayer2);
		if (checkPlayer1 == GW::MATH::GCollision::GCollisionCheck::COLLISION && !e.has<ESG::Player>())
		{
			pointLock.LockSyncWrite();
			powerPoints++;
			auto tokenCount =player1.get_mut<PowerMeterTokens>();
			auto score = player1.get_mut<playerScore>();
			if (tokenCount->value <7)
			{
			tokenCount->value += 1;
			score->score += 500;
			}
			else
			{
				score->score += 5000;
			}
			player1.modified<PowerMeterTokens>();
			pointLock.UnlockSyncWrite();
			e.destruct();
		}
		else if (checkPlayer2 == GW::MATH::GCollision::GCollisionCheck::COLLISION && !e.has<ESG::Player>())
		{
			pointLock.LockSyncWrite();
			powerPoints++;
			auto tokenCount = player2.get_mut<PowerMeterTokens>();
			auto score = player2.get_mut<playerScore>();
			if (tokenCount->value < 7)
			{
				tokenCount->value += 1;
				score->score += 500;
			}
			else
			{
				score->score += 5000;
				//increase score 
			}
			player2.modified<PowerMeterTokens>();
			pointLock.UnlockSyncWrite();
			
			e.destruct();
		}

	//	float zero = 0;
	//	(*immediateInput).GetState(G_KEY_0, zero);
	//	if (zero > 0.0f)
	//	{
	//		powerPoints += 1;
	//	}
			});

	//game->entity("Power System").add<PowerSystem>();
	//game->system<Player>()
	//	.each([this](flecs::entity e, Player) {

	//	//need to remake into getting powerups based on nuber of points not choosing
	//	float one = 0;
	//	(*immediateInput).GetState(G_KEY_1, one);

	//	if (one > 0.0f)
	//	{
	//		pointLock.LockSyncWrite();
	//		if (powerPoints >= 15)
	//		{
	//			if (e.has<Shield>())
	//			{
	//				auto shield = e.get_mut<Shield>();
	//				shield->charges += 1;
	//			}
	//			else
	//			{
	//				e.set_override<Shield>({ 1 });
	//			}
	//			powerPoints = 0;

	//		}
	//		else if (powerPoints >= 10)
	//		{
	//			if (!e.has<Lasers>())
	//			{
	//				e.add<Lasers>();
	//				powerPoints = 0;
	//			}
	//		}
	//		else if (powerPoints >= 5)
	//		{
	//			if (!e.has<Missiles>())
	//			{
	//				e.add<Missiles>();
	//				powerPoints = 0;
	//			}

	//		}
	//		pointLock.UnlockSyncWrite();
	//	}
	//		});
	return true;
}

void ESG::PowerUp::UpPoints(unsigned int valueUp)
{
	if (10 - powerPoints <= valueUp * powerMult)
	{
		powerPoints = 10;
	}
	else
	{
		powerPoints = valueUp * powerMult;
	}
}

bool ESG::PowerUp::Activation(bool runSystem)
{
	if (runSystem)
	{
		game->entity("Power System").enable();
		game->entity("Power Collision").enable();
	}
	else
	{
		game->entity("Power System").disable();
		game->entity("Power Collision").disable();
	}

	return true;
}

int ESG::PowerUp::GetPoints()
{
	return powerPoints;
}

bool ESG::PowerUp::Shutdown()
{
	game->entity("Power System").destruct();
	game->entity("Power Collision").destruct();
	game.reset();
	gameConfig.reset();
	return true;
}