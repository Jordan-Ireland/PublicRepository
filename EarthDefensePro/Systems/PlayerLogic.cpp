
#include "PlayerLogic.h"
//#include "../Components/Identification.h"
//#include "../Components/Physics.h"
//#include "../Components/Visuals.h"
//#include "../Components/Gameplay.h"
//#include "../Entities/Prefabs.h"
//#include "../Events/Playevents.h"


using namespace ESG; // Example Space Game
using namespace GW::INPUT; // input libs
using namespace GW::AUDIO; // audio libs


// Connects logic to traverse any players and allow a controller to manipulate them
bool ESG::PlayerLogic::Init(std::shared_ptr<flecs::world>& _game,
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
	bufferedInput = _bufferedInput;
	controllerInput = _controllerInput;
	audioEngine = _audioEngine;
	eventPusher = _eventPusher;
	bool debugON = false;
	bool isConnected;
	bool ReadyPlayerTwo;

	// Init any helper systems required for this task
	std::shared_ptr<const GameConfig> readCfg = gameConfig.lock();
	int width = (*readCfg).at("Window").at("width").as<int>();
	float speed = (*readCfg).at("Player1").at("speed").as<float>();
	chargeTime = (*readCfg).at("Player1").at("chargeTime").as<float>();

	flecs::query q = game->query<Player, ESG::ModelTransform, ControllerID>();

	playerSystem = game->system<Player, ESG::ModelTransform, ESG::Renderable, PowerMeterTokens, ControllerID, Speed>("Player System")
		.iter([this, speed, debugON, &isConnected, &ReadyPlayerTwo, q](flecs::iter it, Player*, ESG::ModelTransform* p, ESG::Renderable* r, PowerMeterTokens* t, ControllerID* c, Speed* s) {
		auto levelScroll = game->lookup("Level Scroll Speed").get_mut<ESG::LevelScrollComp>();
		ReadyPlayerTwo = q.count() > 1; // If more than 1 player (Two Players)
		//	std::cout << q.count() << std::endl;
		for (auto i : it) {
			float xaxis = 0, yaxis = 0;

			HandlePlayerKeyBoardMovement(xaxis, yaxis, ReadyPlayerTwo, c[i]);
			HandlePlayerControllerdMovement(xaxis, yaxis, ReadyPlayerTwo, c[i], isConnected);
			/*if (ReadyPlayerTwo) {
				if (c[i].index == 0) {
					HandlePlayerOneInput(xaxis, yaxis, isConnected);
				}
				else if (c[i].index == 1) {
					HandlePlayerTwoKeyBoardMovement(xaxis, yaxis);
				}
			}
			else if (c[i].index == 0) {
				HandleSoloPlayerInput(xaxis, yaxis, isConnected);
			}*/

			// Adjust for boundaries and apply movement
			AdjustForBoundaries(p[i]);
			p[i].matrix.row4.x += xaxis * it.delta_time() * s[i].speed * 5 + (levelScroll->value * it.delta_time());
			p[i].matrix.row4.y += yaxis * it.delta_time() * s[i].speed * 5;

			if (debugON) {
				DisplayDebugInfo(p[i]);
			}

			// Handle firing state and missiles
			HandleWeapons(it, p[i], c[i].index, i);

			flecs::entity e = it.entity(i);
			if (e.has<ESG::PlayerHit>() && !e.has<Invulnerable>()) {
				auto lives = e.get_mut<Lives>();
				lives->value--;
				if (lives->value <= 0)
				{
					ESG::PEventGenerator::EVENT_DATA x{};
					GW::GEvent explode;
					explode.Write(ESG::PEventGenerator::Events::EVENT_GAMEOVER, x);
					eventPusher.Push(explode);
				}
				e.remove<ESG::PlayerHit>();
				e.modified<Lives>();
				e.set<Invulnerable>({ 0.2f });
				std::cout << "Hit" << std::endl;
			}



		}

		// process any cached button events after the loop (happens multiple times per frame)
		ProcessInputEvents(it.world(), ReadyPlayerTwo);
			});

	invulSystem = game->system<ESG::Invulnerable, Renderable>("Invulnerable System").interval(0.1)
		.each([this](flecs::entity en, Invulnerable& in, Renderable& r) {
		in.timeLeft -= en.delta_time();

		if (r.value) {
			r.value = false;
		}
		else if (!r.value) {
			r.value = true;
		}
		if (in.timeLeft <= 0) {
			r.value = true;
			en.remove<ESG::Invulnerable>();
		}
			});


	// Create an event cache for when the spacebar/'A' button is pressed
	pressEvents.Create(Max_Frame_Events); // even 32 is probably overkill for one frame Max_Frame_Events

	// register for keyboard and controller events
	bufferedInput.Register(pressEvents);
	controllerInput.Register(pressEvents);

	// create the on explode handler
	onExplode.Create([this](const GW::GEvent& e) {
		ESG::PLAY_EVENT event; ESG::PLAY_EVENT_DATA eventData;
		if (+e.Read(event, eventData)) {
			// only in here if event matches
			std::cout << "Enemy Was Destroyed!\n";
		}
		});
	_eventPusher.Register(onExplode);

	return true;
}
// Free any resources used to run this system
bool ESG::PlayerLogic::Shutdown()
{
	playerSystem.disable();
	invulSystem.destruct();
	playerSystem.destruct();
	game.reset();
	gameConfig.reset();

	return true;
}

// Toggle if a system's Logic is actively running
bool ESG::PlayerLogic::Activate(bool runSystem)
{
	if (playerSystem.is_alive()) {
		(runSystem) ?
			playerSystem.enable()
			: playerSystem.disable();
		return true;
	}
	return false;
}


bool ESG::PlayerLogic::ProcessInputEvents(flecs::world& stage, bool ReadyPlayerTwo) {
	GW::GEvent event;
	bool debugON = false;

	while (+pressEvents.Pop(event)) {

		bool fireWeaponsP1 = false;
		bool fireWeaponsP2 = false;

		bool selectPowerP1 = false;
		bool selectPowerP2 = false;

		GController::Events controller;
		GController::EVENT_DATA c_data;
		GBufferedInput::Events keyboard;
		GBufferedInput::EVENT_DATA k_data;


		if (+event.Read(keyboard, k_data)) {
			if (keyboard == GBufferedInput::Events::KEYPRESSED) {
				switch (k_data.data) {
				case G_KEY_SPACE:
					fireWeaponsP1 = ProcessPlayerAction(debugON, stage.time());
					break;
				case G_KEY_LEFTCONTROL:
					fireWeaponsP2 = ReadyPlayerTwo && ProcessPlayerAction(debugON, stage.time());
					break;
				case G_KEY_ENTER:
					selectPowerP1 = ProcessPlayerAction(debugON, stage.time());
					break;
				case G_KEY_RIGHTSHIFT:
					selectPowerP2 = ReadyPlayerTwo && ProcessPlayerAction(debugON, stage.time());
					break;

				}
			}
			else if (keyboard == GBufferedInput::Events::KEYRELEASED) {
				if (k_data.data == G_KEY_SPACE) fireWeaponsP1 = false;
				else if (k_data.data == G_KEY_RIGHTSHIFT) selectPowerP1 = false;
				else if (k_data.data == G_KEY_LEFTCONTROL) fireWeaponsP2 = false && ReadyPlayerTwo;//redundant && but lets me see what is player two
				else if (k_data.data == G_KEY_RIGHTSHIFT) selectPowerP2 = false && ReadyPlayerTwo;//redundant && but lets me see what is player two


			}
		}

		if (+event.Read(controller, c_data) && c_data.controllerIndex == 0)
		{


			if (+event.Read(controller, c_data) && controller == GController::Events::CONTROLLERBUTTONVALUECHANGED) {
				if (c_data.inputValue > 0 && c_data.inputCode == G_SOUTH_BTN) {

					fireWeaponsP2 = true;
				}
				else if (c_data.inputValue > 0 && c_data.inputCode == G_EAST_BTN) {
					selectPowerP2 = true;
				}
				else if (c_data.inputValue > 0 && c_data.inputCode == G_START_BTN)
				{
					stage.entity("Ship").add<Player>();
					stage.entity("Ship").set<Renderable>({ true });
					/*if (!isPaused)
					{
						isPaused = true;
						ESG::PEventGenerator::EVENT_DATA x{};
						GW::GEvent explode;
						explode.Write(ESG::PEventGenerator::Events::EVENT_PAUSE, x);
						eventPusher.Push(explode);
					}
					else
					{
						isPaused = false;
						ESG::PEventGenerator::EVENT_DATA x{};
						GW::GEvent explode;
						explode.Write(ESG::PEventGenerator::Events::EVENT_UNPAUSE, x);
						eventPusher.Push(explode);
					}*/
					

				}

			}
		}
		else if (+event.Read(controller, c_data) && c_data.controllerIndex == 1)
		{

			if (+event.Read(controller, c_data) && controller == GController::Events::CONTROLLERBUTTONVALUECHANGED) {
				if (c_data.inputValue > 0 && c_data.inputCode == G_SOUTH_BTN) {

					fireWeaponsP1 = true;
				}
				else if (c_data.inputValue > 0 && c_data.inputCode == G_EAST_BTN) {
					selectPowerP1 = true;
				}
				else if (c_data.inputValue > 0 && c_data.inputCode == G_START_BTN) {
					stage.entity("Ship").add<Player>();
					stage.entity("Ship").set<Renderable>({ true });
				}

			}
		}

		SetPlayerState(stage, fireWeaponsP1, fireWeaponsP2, selectPowerP1, selectPowerP2);
	}

	return true;
}



bool ESG::PlayerLogic::ProcessPlayerAction(bool debug, float currentTime) {
	if (debug) std::cout << "FIRE" << std::endl;
	//chargeStart = currentTime;
	return true;
}

void ESG::PlayerLogic::SetPlayerState(flecs::world& stage, bool fireWeaponsP1, bool fireWeaponsP2,
	bool selectPowerP1, bool selectPowerP2) {


	if (fireWeaponsP1) stage.entity("Spaceship_RaeTheRedPanda").add<Firing>();
	if (fireWeaponsP2) stage.entity("Ship").add<Firing>();
	//if (selectPowerP1) GetPowerUpComponents(stage.entity("Spaceship_RaeTheRedPanda"));
	//if (selectPowerP2) GetPowerUpComponents(stage.entity("Ship"));
	if (selectPowerP1)stage.entity("Spaceship_RaeTheRedPanda").add<Select>();
	if (selectPowerP2)stage.entity("Ship").add<Select>();
}

void ESG::PlayerLogic::HandlePlayerOneInput(float& xaxis, float& yaxis, bool& isConnected) {
	//float input = 0;
	//immediateInput.GetState(G_KEY_A, input); xaxis -= input;
	//immediateInput.GetState(G_KEY_D, input); xaxis += input;
	//immediateInput.GetState(G_KEY_W, input); yaxis += input;
	//immediateInput.GetState(G_KEY_S, input); yaxis -= input;

	//controllerInput.IsConnected(1, isConnected);
	//if (isConnected) {
	//	controllerInput.GetState(0, G_LX_AXIS, input); xaxis += input;
	//	controllerInput.GetState(0, G_LY_AXIS, input); yaxis += input;
	//	controllerInput.GetState(0, G_DPAD_LEFT_BTN, input); xaxis -= input;
	//	controllerInput.GetState(0, G_DPAD_RIGHT_BTN, input); xaxis += input;
	//	controllerInput.GetState(0, G_DPAD_UP_BTN, input); yaxis += input;
	//	controllerInput.GetState(0, G_DPAD_DOWN_BTN, input); yaxis -= input;
	//}

	//xaxis = G_LARGER(xaxis, -1);  // cap right motion
	//xaxis = G_SMALLER(xaxis, 1);  // cap left motion
	//yaxis = G_LARGER(yaxis, -1);  // cap down motion
	//yaxis = G_SMALLER(yaxis, 1);  // cap up motion
}
void ESG::PlayerLogic::HandlePlayerKeyBoardMovement(float& xaxis, float& yaxis, bool& _ReadyPlayerTwo, ControllerID& player) {
	float input = 0;

	switch (player.index)
	{
	case 0:
		immediateInput.GetState(G_KEY_A, input); xaxis -= input;
		immediateInput.GetState(G_KEY_D, input); xaxis += input;
		immediateInput.GetState(G_KEY_W, input); yaxis += input;
		immediateInput.GetState(G_KEY_S, input); yaxis -= input;
		/*if (!_ReadyPlayerTwo)
		{
			immediateInput.GetState(G_KEY_LEFT, input); xaxis -= input;
			immediateInput.GetState(G_KEY_RIGHT, input); xaxis += input;
			immediateInput.GetState(G_KEY_UP, input); yaxis += input;
			immediateInput.GetState(G_KEY_DOWN, input); yaxis -= input;
		}*/
		break;
	case 1:
		immediateInput.GetState(G_KEY_LEFT, input); xaxis -= input;
		immediateInput.GetState(G_KEY_RIGHT, input); xaxis += input;
		immediateInput.GetState(G_KEY_UP, input); yaxis += input;
		immediateInput.GetState(G_KEY_DOWN, input); yaxis -= input;
		break;
	}

	xaxis = G_LARGER(xaxis, -1);  // cap right motion
	xaxis = G_SMALLER(xaxis, 1);  // cap left motion
	yaxis = G_LARGER(yaxis, -1);  // cap down motion
	yaxis = G_SMALLER(yaxis, 1);  // cap up motion
}
void ESG::PlayerLogic::HandlePlayerControllerdMovement(float& xaxis, float& yaxis, bool& _ReadyPlayerTwo, ControllerID& player, bool& isConnected) {
	float input = 0;
	bool controllerP1;
	bool controllerP2;

	controllerInput.IsConnected(0, controllerP1);
	controllerInput.IsConnected(1, controllerP2);

	switch (player.index)
	{
	case 0:
		if (controllerP1&& controllerP2)
		{
			controllerInput.GetState(0, G_LX_AXIS, input); xaxis += input;
			controllerInput.GetState(0, G_LY_AXIS, input); yaxis += input;
			controllerInput.GetState(0, G_DPAD_LEFT_BTN, input); xaxis -= input;
			controllerInput.GetState(0, G_DPAD_RIGHT_BTN, input); xaxis += input;
			controllerInput.GetState(0, G_DPAD_UP_BTN, input); yaxis += input;
			controllerInput.GetState(0, G_DPAD_DOWN_BTN, input); yaxis -= input;
		}
		break;
	case 1:
		if (controllerP1)
		{
			controllerInput.GetState(0, G_LX_AXIS, input); xaxis += input;
			controllerInput.GetState(0, G_LY_AXIS, input); yaxis += input;
			controllerInput.GetState(0, G_DPAD_LEFT_BTN, input); xaxis -= input;
			controllerInput.GetState(0, G_DPAD_RIGHT_BTN, input); xaxis += input;
			controllerInput.GetState(0, G_DPAD_UP_BTN, input); yaxis += input;
			controllerInput.GetState(0, G_DPAD_DOWN_BTN, input); yaxis -= input;
		}
		if (controllerP2)
		{
			controllerInput.GetState(1, G_LX_AXIS, input); xaxis += input;
			controllerInput.GetState(1, G_LY_AXIS, input); yaxis += input;
			controllerInput.GetState(1, G_DPAD_LEFT_BTN, input); xaxis -= input;
			controllerInput.GetState(1, G_DPAD_RIGHT_BTN, input); xaxis += input;
			controllerInput.GetState(1, G_DPAD_UP_BTN, input); yaxis += input;
			controllerInput.GetState(1, G_DPAD_DOWN_BTN, input); yaxis -= input;
		}
		break;
	}

	xaxis = G_LARGER(xaxis, -1);  // cap right motion
	xaxis = G_SMALLER(xaxis, 1);  // cap left motion
	yaxis = G_LARGER(yaxis, -1);  // cap down motion
	yaxis = G_SMALLER(yaxis, 1);  // cap up motion
}
void ESG::PlayerLogic::HandlePlayerTwoKeyBoardMovement(float& xaxis, float& yaxis) {
	//float input = 0;
	//immediateInput.GetState(G_KEY_LEFT, input); xaxis -= input;
	//immediateInput.GetState(G_KEY_RIGHT, input); xaxis += input;
	//immediateInput.GetState(G_KEY_UP, input); yaxis += input;
	//immediateInput.GetState(G_KEY_DOWN, input); yaxis -= input;

	//// TODO: set up logic for second controller check

	//xaxis = G_LARGER(xaxis, -1);  // cap right motion
	//xaxis = G_SMALLER(xaxis, 1);  // cap left motion
	//yaxis = G_LARGER(yaxis, -1);  // cap down motion
	//yaxis = G_SMALLER(yaxis, 1);  // cap up motion
}

void ESG::PlayerLogic::HandleSoloPlayerInput(float& xaxis, float& yaxis, bool& isConnected) {
	float input = 0;
	// Repeated logic here for solo player; you can adjust based on requirements
	HandlePlayerOneInput(xaxis, yaxis, isConnected);

	immediateInput.GetState(G_KEY_LEFT, input); xaxis -= input;
	immediateInput.GetState(G_KEY_RIGHT, input); xaxis += input;
	immediateInput.GetState(G_KEY_UP, input); yaxis += input;
	immediateInput.GetState(G_KEY_DOWN, input); yaxis -= input;

}
void ESG::PlayerLogic::AdjustForBoundaries(ESG::ModelTransform& transform) {
	transform.matrix.row4.x = G_LARGER(transform.matrix.row4.x, -9.8f);
	transform.matrix.row4.x = G_SMALLER(transform.matrix.row4.x, +9.8f);
	transform.matrix.row4.y = G_LARGER(transform.matrix.row4.y, -4.8f);
	transform.matrix.row4.y = G_SMALLER(transform.matrix.row4.y, +4.8f);
}
void ESG::PlayerLogic::DisplayDebugInfo(const ESG::ModelTransform& transform) {
	std::cout << "ship X: " << transform.matrix.row4.x << std::endl;
	std::cout << "ship Y: " << transform.matrix.row4.y << std::endl;
	std::cout << "ship Z: " << transform.matrix.row4.z << std::endl;
}
void ESG::PlayerLogic::HandleWeapons(flecs::iter& it, ESG::ModelTransform& transform, int controllerIndex, int i) {


	if (it.entity(i).has<Select>())
	{
		GetPowerUpComponents(it, i);
		it.entity(i).remove<Select>();
	}
	if (it.entity(i).has<Firing>())
	{

		if (it.entity(i).has<Bullet>()) {
			ESG::ModelTransform offset = transform;
			offset.matrix.row4.x += 1.5f;
			controllerInput.StartVibration(controllerIndex, 0, 2, 1);
			FireBullets(it.world(), offset);
			//it.entity(i).remove<Bullet>();
		}
		if (it.entity(i).has<Lasers>()) {
			ESG::ModelTransform offset = transform;
			offset.matrix.row4.x += 3.5f;
			controllerInput.StartVibration(controllerIndex, 0, 2, 1);
			FireLasers(it.world(), offset);
			//it.entity(i).remove<Lasers>();
		}
		if (it.entity(i).has<Missiles>()) {
			ESG::ModelTransform offset = transform;
			offset.matrix.row4.x += 1.5f;
			FireMissile(it.world(), offset);
			//it.entity(i).remove<Missiles>();

		}
		if (it.entity(i).has<Double>()) {
			ESG::ModelTransform offset = transform;
			offset.matrix.row4.x += 1.5f;
			FireDouble(it.world(), offset);
			//	it.entity(i).remove<Double>();

		}
		if (it.entity(i).has<Beams>()) {
			ESG::ModelTransform offset = transform;
			offset.matrix.row4.x += 1.5f;
			controllerInput.StartVibration(controllerIndex, 0, 2, 1);
			FireBeam(it.world(), offset);
			//it.entity(i).remove<Beams>();
		}
		it.entity(i).remove<Firing>();
	}
}
bool ESG::PlayerLogic::FireBullets(flecs::world& stage, ESG::ModelTransform origin)
{
	// Grab the prefab for a laser round
	flecs::entity bulletClone;
	auto b = RetreivePrefab("Basic Bullet", bulletClone);


	auto& mt = *bulletClone.get_mut<ESG::ModelTransform>();
	auto inst = bulletClone.get_mut<ESG::Instance>();

	auto lvl = game->lookup("LevelData");



	std::cout << "Bullet" << stage.count<ESG::Bullet>();


	bool debugON = false;
	origin.matrix.row4.x += 0.2f;
	origin.matrix.row4.z += 0;
	origin.matrix.row4.y += 0.1f;
	mt.matrix.row4 = origin.matrix.row4;
	//	inst->transformCount = stage.count<ESG::Bullet>()+ 1;
		//inst->transformCount +=1;

		//origin.value.y += 0.0f;
	auto bBullet = stage.entity().is_a(bulletClone)
		.set_override<ESG::ModelTransform>(mt)
		.set_override<Projectile>({ true });

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
bool ESG::PlayerLogic::FireLasers(flecs::world& stage, ESG::ModelTransform origin)
{
	// Grab the prefab for a laser round
	flecs::entity laserClone;
	auto b = RetreivePrefab("Lazer Bullet", laserClone);


	auto& mt = *laserClone.get_mut<ESG::ModelTransform>();
	auto inst = laserClone.get_mut<ESG::Instance>();

	auto lvl = game->lookup("LevelData");



	std::cout << "Laser" << stage.count<ESG::Lasers>();


	bool debugON = false;
	origin.matrix.row4.x += 0.0f;
	origin.matrix.row4.z = 0;
	mt.matrix.row4 = origin.matrix.row4;
	//	inst->transformCount = stage.count<ESG::Bullet>()+ 1;
		//inst->transformCount +=1;

		//origin.value.y += 0.0f;
	auto laserLeft = stage.entity().is_a(laserClone)
		.set_override<ESG::ModelTransform>(mt)
		.set_override<Projectile>({ true });

	//.set<ESG::Instance>(*inst);
	//.add<ESG::Renderable>()
	//.add<ESG::Bullet>();
	if (debugON)
	{
		std::cout << "Laser X: " << origin.matrix.row4.x << std::endl;
		std::cout << "Laser Y: " << origin.matrix.row4.y << std::endl;
		std::cout << "Laser Z: " << origin.matrix.row4.z << std::endl;
	}




	// play the sound of the Lazer prefab
	GW::AUDIO::GSound shoot = *laserClone.get<GW::AUDIO::GSound>();
	shoot.Play();

	return true;
}
bool ESG::PlayerLogic::FireMissile(flecs::world& stage, ESG::ModelTransform origin)
{
	// Grab the prefab for a laser round
	flecs::entity missileClone;
	auto b = RetreivePrefab("missile", missileClone);


	auto& mt = *missileClone.get_mut<ESG::ModelTransform>();
	auto inst = missileClone.get_mut<ESG::Instance>();

	auto lvl = game->lookup("LevelData");



	std::cout << "Missiles" << stage.count<ESG::Missiles>();


	bool debugON = false;
	origin.matrix.row4.x -= 1.5f;
	origin.matrix.row4.y -= 0.5f;
	mt.matrix.row4 = origin.matrix.row4;
	//inst->transformCount = stage.count<ESG::Bullet>() + 1;
	//inst->transformCount += 1;
	//origin.value.y += 0.0f;
	auto missileDown = stage.entity().is_a(missileClone)
		.set_override<ESG::ModelTransform>(mt)
		.set_override<Projectile>({ true });

	//.set<ESG::Instance>(*inst);
	//.add<ESG::Renderable>()
	//.add<ESG::Bullet>();
	if (debugON)
	{
		std::cout << "Missile X: " << origin.matrix.row4.x << std::endl;
		std::cout << "Missile Y: " << origin.matrix.row4.y << std::endl;
		std::cout << "Missile Z: " << origin.matrix.row4.z << std::endl;
	}




	// play the sound of the Lazer prefab
	GW::AUDIO::GSound shoot = *missileClone.get<GW::AUDIO::GSound>();
	shoot.Play();

	return true;
}
bool ESG::PlayerLogic::FireDouble(flecs::world& stage, ESG::ModelTransform origin)
{
	// Grab the prefab for a laser round
	flecs::entity doubleClone;
	auto b = RetreivePrefab("double", doubleClone);


	auto& mt = *doubleClone.get_mut<ESG::ModelTransform>();
	auto inst = doubleClone.get_mut<ESG::Instance>();

	auto lvl = game->lookup("LevelData");



	std::cout << "Double" << stage.count<ESG::Double>();


	bool debugON = false;
	origin.matrix.row4.x += 0.0f;
	origin.matrix.row4.z = 0;
	mt.matrix.row4 = origin.matrix.row4;
	//inst->transformCount = stage.count<ESG::Bullet>() + 1;
	//inst->transformCount += 1;
	//origin.value.y += 0.0f;
	auto missileDown = stage.entity().is_a(doubleClone)
		.set_override<ESG::ModelTransform>(mt)
		.set_override<Projectile>({ true });

	//.set<ESG::Instance>(*inst);
	//.add<ESG::Renderable>()
	//.add<ESG::Bullet>();
	if (debugON)
	{
		std::cout << "Double X: " << origin.matrix.row4.x << std::endl;
		std::cout << "Double Y: " << origin.matrix.row4.y << std::endl;
		std::cout << "Double Z: " << origin.matrix.row4.z << std::endl;
	}




	// play the sound of the Lazer prefab
	GW::AUDIO::GSound shoot = *doubleClone.get<GW::AUDIO::GSound>();
	shoot.Play();

	return true;
}
bool ESG::PlayerLogic::FireBeam(flecs::world& stage, ESG::ModelTransform origin)
{
	// Grab the prefab for a laser round
	flecs::entity beamClone;
	if (RetreivePrefab("Laser Beam", beamClone))
	{


		auto& mt = *beamClone.get_mut<ESG::ModelTransform>();
		auto inst = beamClone.get_mut<ESG::Instance>();

		auto lvl = game->lookup("LevelData");



		std::cout << "Beams" << stage.count<ESG::Beams>();


		bool debugON = false;


		origin.matrix.row4.y = 5;
		origin.matrix.row4.x = -10.0f;
		for (int i = 0; i < 20; i++)
		{

			if (rand() % 2 == 0)
			{
				origin.matrix.row4.x += 2.0f;

			}
			else
			{
				origin.matrix.row4.x -= 2.0f;
			}
			origin.matrix.row4.y -= 0.5f;

			mt.matrix.row4 = origin.matrix.row4;
			//	inst->transformCount = stage.count<ESG::Bullet>()+ 1;
				//inst->transformCount +=1;

				//origin.value.y += 0.0f;
			auto beam = stage.entity().is_a(beamClone)
				.set_override<ESG::ModelTransform>(mt)
				.set_override<Projectile>({ true });
		}

		//.set<ESG::Instance>(*inst);
		//.add<ESG::Renderable>()
		//.add<ESG::Bullet>();
		if (debugON)
		{
			std::cout << "Laser X: " << origin.matrix.row4.x << std::endl;
			std::cout << "Laser Y: " << origin.matrix.row4.y << std::endl;
			std::cout << "Laser Z: " << origin.matrix.row4.z << std::endl;




			// play the sound of the Lazer prefab
		}
		GW::AUDIO::GSound shoot = *beamClone.get<GW::AUDIO::GSound>();
		shoot.Play();

	}

	return true;
}
bool ESG::PlayerLogic::SpawnToken(flecs::world& stage, GW::MATH::GVECTORF pos)
{
	// Grab the prefab for a laser round
	auto chance = (rand() % 100 < 20) ? true : false;
	if (chance)
	{

		flecs::entity tokenClone;
		if (RetreivePrefab("Power Token", tokenClone))
		{


			auto& mt = *tokenClone.get_mut<ESG::ModelTransform>();
			//	auto inst = tokenClone.get_mut<ESG::Instance>();

			auto lvl = game->lookup("LevelData");



			std::cout << "Token" << stage.count<ESG::token>();


			bool debugON = false;

			mt.matrix.row4 = pos;



			auto token = stage.entity().is_a(tokenClone)
				.set_override<ESG::ModelTransform>(mt);


			if (debugON)
			{
				/*std::cout << "Token X: " << origin.matrix.row4.x << std::endl;
				std::cout << "Token Y: " << origin.matrix.row4.y << std::endl;
				std::cout << "Token Z: " << origin.matrix.row4.z << std::endl;*/




				// play the sound of the Lazer prefab
			}
			GW::AUDIO::GSound shoot = *tokenClone.get<GW::AUDIO::GSound>();
			shoot.Play();

		}
	}

	return true;
}
void ESG::PlayerLogic::GetPowerUpComponents(flecs::iter& it, int i)
{

	auto tokensBank = it.entity(i).get_mut<PowerMeterTokens>();

	switch (tokensBank->value)
	{
	case 1:
		std::cout << "speed" << std::to_string(tokensBank->value) << std::endl;
		if (it.entity(i).has<ESG::Speed>())
		{
			auto speedPower = it.entity(i).get_mut<ESG::Speed>();

			if (speedPower[i].counter < 3)
			{
				speedPower[i].speed *= 1.2f;
				speedPower[i].counter += 1;

			}
			it.entity(i).modified<Speed>();
		}
		else
		{
			it.entity(i).add<Speed>();
		}

		tokensBank->value = 0;
		it.entity(i).modified<PowerMeterTokens>();
		break;
	case 2:
		if (!it.entity(i).has<ESG::Missiles>())
		{

			it.entity(i).add<Missiles>();
			tokensBank->value = 0;
			it.entity(i).modified<PowerMeterTokens>();
		}
		break;
	case 3:
		if (!it.entity(i).has<ESG::Double>())
		{


			it.entity(i).add<Double>();
			tokensBank->value = 0;
			it.entity(i).modified<PowerMeterTokens>();
		}
		break;
	case 4:
		if (!it.entity(i).has<ESG::Lasers>())
		{
			it.entity(i).add<Lasers>();
			it.entity(i).remove<Bullet>();
			tokensBank->value = 0;
			it.entity(i).modified<PowerMeterTokens>();
		}
		break;
	case 5:
		//std::cout << "Option" << std::to_string(tokensBank->value) << std::endl;
		/*it.entity(i).add<Option>();
		tokensBank->value = 0;
		it.entity(i).modified<PowerMeterTokens>();*/
		break;
	case 6:
		//std::cout << "Shield" << std::to_string(tokensBank->value) << std::endl;
	/*	it.entity(i).add<Shield>();
		tokensBank->value = 0;
		it.entity(i).modified<PowerMeterTokens>();*/
		break;
	case 7:
		if (!it.entity(i).has<ESG::Beams>())
		{
			std::cout << "Beams" << std::to_string(tokensBank->value) << std::endl;
			it.entity(i).add<Beams>();
			tokensBank->value = 0;
			it.entity(i).modified<PowerMeterTokens>();
		}
		break;
	default:
		//std::cout << "Nothing" << std::to_string(tokensBank->value) << std::endl;
		break;

	}




}