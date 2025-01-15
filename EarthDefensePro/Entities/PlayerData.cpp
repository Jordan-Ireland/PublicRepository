#include "PlayerData.h"
//#include "../Components/Identification.h"
//#include "../Components/Visuals.h"
//#include "../Components/Physics.h"
//#include "Prefabs.h"
//#include "../Components/Blender.h"
bool ESG::PlayerData::Load(std::shared_ptr<flecs::world>& _game,
	std::weak_ptr<const GameConfig> _gameConfig)
{

	if (ReadyPlayerOne(_game, _gameConfig))
	{
		if (ReadyPlayerTwo(_game, _gameConfig))
		{
			//add new controller support tag
		}

		return true;
	}

	return false;

}

bool ESG::PlayerData::Unload(std::shared_ptr<flecs::world>& _game)
{
	// remove all players
	_game->defer_begin(); // required when removing while iterating!
	_game->each([](flecs::entity e, Player&) {
		e.destruct(); // destroy this entitiy (happens at frame end)
		});
	_game->defer_end(); // required when removing while iterating!

	return true;
}


bool ESG::PlayerData::ReadyPlayerOne(std::shared_ptr<flecs::world>& _game,
	std::weak_ptr<const GameConfig> _gameConfig)
{
	// Grab init settings for players
	std::shared_ptr<const GameConfig> readCfg = _gameConfig.lock();
	// color
	float red = (*readCfg).at("Player1").at("red").as<float>();
	float green = (*readCfg).at("Player1").at("green").as<float>();
	float blue = (*readCfg).at("Player1").at("blue").as<float>();
	// start position
	float xstart = (*readCfg).at("Player1").at("xstart").as<float>();
	float ystart = (*readCfg).at("Player1").at("ystart").as<float>();
	float scale = (*readCfg).at("Player1").at("scale").as<float>();
	// other attributes
	float angle = (*readCfg).at("Player1").at("angle").as<float>();
	//get this from a source eventually
	int lives = 3;
	

	// Create Player One
	_game->entity("Spaceship_RaeTheRedPanda")
		.set([&](ControllerID& c, ESG::ModelTransform& p, ESG::Orientation& o, ESG::PowerMeterTokens& t, ESG::ModelBoundry& b) {
		c = { 0 };
		t = { 0 };
		o = { GW::MATH::GIdentityMatrixF };

		GW::MATH::GMatrix::RotateYGlobalF(p.matrix,
			G_DEGREE_TO_RADIAN_F(angle), p.matrix);
		GW::MATH::GMatrix::ScaleGlobalF(p.matrix, GW::MATH::GVECTORF{ scale, scale, scale }, p.matrix);

		b.obb.extent.x *= scale * 10;
		b.obb.extent.y *= scale * 10;
		b.obb.extent.z *= scale * 10;

		GW::MATH::GMatrix::GetRotationF(p.matrix, b.obb.rotation);

		GW::MATH::GVECTORF s;
		GW::MATH::GMatrix::GetScaleF(p.matrix, s);

		s.x = scale;
		s.z = scale;
		s.y = scale;

		GW::MATH::GMatrix::ScaleGlobalF(p.matrix, s, p.matrix);
			})
		.add<Player>()// Tag this entity as a Player
				//.add<Renderable>()
				.set_override<ESG::Lives>({ lives })
				.set_override<ESG::playerScore>({0})
				.add<ESG::Collidable>()
				.add<Bullet>()
				.set<prefabName>({ "Spaceship_RaeTheRedPanda" })
				.set<ESG::Speed>({1.0f,0});


			return true;

};

bool ESG::PlayerData::ReadyPlayerTwo(std::shared_ptr<flecs::world>& _game,
	std::weak_ptr<const GameConfig> _gameConfig)
{
	// Grab init settings for players
	std::shared_ptr<const GameConfig> readCfg = _gameConfig.lock();
	// color
	float red = (*readCfg).at("Player2").at("red").as<float>();
	float green = (*readCfg).at("Player2").at("green").as<float>();
	float blue = (*readCfg).at("Player2").at("blue").as<float>();
	// start position
	float xstart = (*readCfg).at("Player2").at("xstart").as<float>();
	float ystart = (*readCfg).at("Player2").at("ystart").as<float>();
	float scale = (*readCfg).at("Player2").at("scale").as<float>();
	// other attributes
	float angle = (*readCfg).at("Player2").at("angle").as<float>();
	//get this from a source eventually
	int lives = 3;
	
	// Create Player One
	_game->entity("Ship")
		.set([&](ControllerID& c, ESG::ModelTransform& p, ESG::Orientation& o, ESG::PowerMeterTokens& t, ESG::ModelBoundry& b) {
		c = { 1 };
		t = { 0 };
		o = { GW::MATH::GIdentityMatrixF };

		GW::MATH::GMatrix::RotateYGlobalF(p.matrix,
			G_DEGREE_TO_RADIAN_F(angle), p.matrix);
		GW::MATH::GMatrix::ScaleGlobalF(p.matrix, GW::MATH::GVECTORF{ scale, scale, scale }, p.matrix);

		b.obb.extent.x *= scale * 10;
		b.obb.extent.y *= scale * 10;
		b.obb.extent.z *= scale * 10;

		GW::MATH::GMatrix::GetRotationF(p.matrix, b.obb.rotation);

		GW::MATH::GVECTORF s;
		GW::MATH::GMatrix::GetScaleF(p.matrix, s);

		s.x = scale;
		s.z = scale;
		s.y = scale;

		GW::MATH::GMatrix::ScaleGlobalF(p.matrix, s, p.matrix);

			})
		.add<Player>() // Tag this entity as a Player
				.set<Renderable>({false})
				.set_override<ESG::Lives>({ lives })
				.set_override<ESG::playerScore>({ 0 })
				.add<ESG::Collidable>()
				.add<Bullet>()
				.set<prefabName>({ "Ship" })
				.set<ESG::Speed>({1.0f, 0 });






			return true;
};