#include <random>
#include "LevelLogic.h"
#include "../Components/Identification.h"
#include "../Components/Physics.h"
#include "../Entities/Prefabs.h"
#include "../Utils/Macros.h"
#include "../Events/PEventGenerator.h"

using namespace ESG; // Example Space Game

// Connects logic to traverse any players and allow a controller to manipulate them
bool ESG::LevelLogic::Init(std::shared_ptr<flecs::world> _game,
	std::weak_ptr<const GameConfig> _gameConfig,
	GW::AUDIO::GAudio _audioEngine,
	GW::CORE::GEventGenerator _eventPusher)
{
	// sets current level to  1 always will need to change in future
	currentLevel = 1;

	// save a handle to the ECS & game settings
	game = _game;
	gameConfig = _gameConfig;
	audioEngine = _audioEngine;
	eventPusher = _eventPusher;
	// create an asynchronus version of the world
	gameAsync = game->async_stage(); // just used for adding stuff, don't try to read data
	gameLock.Create();
	// Pull enemy Y start location from config file
	std::shared_ptr<const GameConfig> readCfg = _gameConfig.lock();
	auto q = game->query<Enemy>(); //(*readCfg).at(GetLevelGroup()).at("numofenemies").as<int>();
	float enemystartY[5];
	float enemyaccmax[5];
	float enemyaccmin[5];
	float enemyVelocity[5];
	float spawnDelay[5];
	for (int i = 0; i < numberEnemyTyp; i++)
	{
		std::string enemyNum = "Enemy" + std::to_string(i + 1);
		enemystartY[i] = (*readCfg).at(enemyNum).at("ystart").as<float>();
		enemyaccmax[i] = (*readCfg).at(enemyNum).at("accmax").as<float>();
		enemyaccmin[i] = (*readCfg).at(enemyNum).at("accmin").as<float>();
		enemyVelocity[i] = (*readCfg).at(enemyNum).at("velocity").as<float>();
		spawnDelay[i] = (*readCfg).at(enemyNum).at("spawndelay").as<float>();
	}
	// level one info

	// spins up a job in a thread pool to invoke a function at a regular interval
	//for (int i = 0; i < numberEnemyTyp; i++)
	//{
	//	timedEvents.Create(spawnDelay[i] * 1000, [this, enemystartY, enemyaccmax, enemyaccmin, enemyVelocity, i]() {
	//		// compute random spawn location
	//		std::random_device rd;  // Will be used to obtain a seed for the random number engine
	//		std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	//		std::uniform_real_distribution<float> x_range(-0.9f, +0.9f);
	//		std::uniform_real_distribution<float> a_range(enemyaccmin[i], enemyaccmax[i]);
	//		float Xstart = x_range(gen); // normal rand() doesn't work great multi-threaded
	//		float accel = a_range(gen);
	//		// grab enemy type 1 prefab
	//		flecs::entity et1;
	//		if (RetreivePrefab("Enemy Type1", et1)) {
	//			// you must ensure the async_stage is thread safe as it has no built-in synchronization
	//			gameLock.LockSyncWrite();
	//			// this method of using prefabs is pretty conveinent
	//			gameAsync.entity().is_a(et1)
	//				.set<Velocity>({ -enemyVelocity[i],0 })
	//				.set<Acceleration>({ -accel / 2,0 })
	//				.set<Position>({ enemystartY[i],Xstart });

	//			// be sure to unlock when done so the main thread can safely merge the changes
	//			gameLock.UnlockSyncWrite();
	//		}
	//		}, 5000); // wait 5 seconds to start enemy wave
	//}
	// create a system the runs at the end of the frame only once to merge async changes
	struct LevelSystem {}; // local definition so we control iteration counts
	game->entity("Level System").add<LevelSystem>();
	// only happens once per frame at the very start of the frame
	game->system<LevelSystem>().kind(flecs::OnLoad) // first defined phase
		.each([this, q](flecs::entity e, LevelSystem& s) {
		auto levelScroll = game->entity("Level Scroll Speed").get_mut<ESG::LevelScrollComp>();
		levelScroll->levelScroll->Update(e.delta_time());
		if (q.count() == 0) {
			ESG::PEventGenerator::EVENT_DATA x{ nullptr };
			GW::GEvent explode;
			explode.Write(ESG::PEventGenerator::Events::EVENT_LEVELWON, x);
			eventPusher.Push(explode);
		}
		// merge any waiting changes from the last frame that happened on other threads
		gameLock.LockSyncWrite();
		gameAsync.merge();
		gameLock.UnlockSyncWrite();
			});

	// Load and play current level's music
	UpdateLevelMusic();
	menuMusic.Create("../Music/horror-game-menu.wav", audioEngine, 0.15f);

	//test stuff
	/*MenuAudioStartStop(true);*/
	/*MenuAudioStartStop(false);*/
	currentLevel = 2;
	UpdateLevelMusic();

	return true;
}

// Free any resources used to run this system
bool ESG::LevelLogic::Shutdown()
{
	timedEvents = nullptr; // stop adding enemies
	gameAsync.merge(); // get rid of any remaining commands
	auto e = game->entity("Level System");
	e.destruct();
	// invalidate the shared pointers
	game.reset();
	gameConfig.reset();
	return true;
}

// Toggle if a system's Logic is actively running
bool ESG::LevelLogic::Activate(bool runSystem)
{
	if (runSystem) {
		game->entity("Level System").enable();
	}
	else {
		game->entity("Level System").disable();
	}
	return false;
}

std::string LevelLogic::GetLevelGroup()
{
	std::string levelGroup = "Level" + std::to_string(currentLevel);
	return levelGroup;
}

//stops current music and plays new level music based on current level variable
void LevelLogic::UpdateLevelMusic()
{
	currentTrack.Stop();
	std::shared_ptr<const GameConfig> readCfg = gameConfig.lock();
	std::string music = (*readCfg).at(GetLevelGroup()).at("music").as<std::string>();
	currentTrack.Create(music.c_str(), audioEngine, 0.01f);
	currentTrack.Play(true);
}


//not much in this yet but its here for when we need it later - Jacob
void LevelLogic::ChangeCurrentLevel(int newLevel)
{
	currentLevel = newLevel;
	UpdateLevelMusic();
}

void LevelLogic::MenuAudioStartStop(bool opening)
{
	bool isplaying;
	currentTrack.isPlaying(isplaying);
	bool menuPlaying;
	menuMusic.isPlaying(menuPlaying);
	if (opening)
	{
		if (isplaying)
		{
			currentTrack.Pause();
		}
		if (!menuPlaying)
		{
			menuMusic.Play();
		}

	}
	else
	{
		if (!isplaying)
		{
			currentTrack.Play();
		}
		if (menuPlaying)
		{
			menuMusic.Stop();
		}
	}
}

// **** SAMPLE OF MULTI_THREADED USE ****
//flecs::world world; // main world
//flecs::world async_stage = world.async_stage();
//
//// From thread
//lock(async_stage_lock);
//flecs::entity e = async_stage.entity().child_of(parent)...
//unlock(async_stage_lock);
//
//// From main thread, periodic
//lock(async_stage_lock);
//async_stage.merge(); // merge all commands to main world
//unlock(async_stage_lock);