#pragma once
//#ifndef APPLICATION_H
//#define APPLICATION_H

//#include <d3d11.h>
//#include "Engine/load_data_oriented.h"
#include "Engine/FileIntoString.h"
#include "Components/Blender.h"
// include events
//#include "Events/Playevents.h"
// Contains our global game settings
#include "GameConfig.h"
// Load all entities+prefabs used by the game 
#include "Entities/BulletData.h"
#include "Entities/LaserData.h"
#include "Entities/MissileData.h"
#include "Entities/DoubleData.h"
#include "Entities/PlayerData.h"
#include "Entities/EnemyData.h"
#include "Entities/BeamData.h"
#include "Entities/PowerTokenData.h"
#include "Entities/ShieldData.h"
#include "Entities/Options.h"
//#include "Entities/BossData.h"
// Include all systems used by the game and their associated components
#include "Systems/PlayerLogic.h"

// REPLACE VULKAN
//#include "Systems/VulkanRendererLogic.h"

#include "Systems/LevelLogic.h"
#include "Systems/PhysicsLogic.h"
#include "Systems/BulletLogic.h"
#include "Systems/EnemyLogic.h"
//#include "Systems/BossLogic.h"
#include "Systems/PowerUp.h"
#include "Systems/SoundSystem.h"

#include "Systems/LevelScroll.h"

#include "Systems/Collisions.h"

#include "Systems/FiringTimer.h"
#include "Systems/CheatCodes.h"


#include "Engine/LevelManager.h"
#include "Engine/Defines.h"
#include "Engine/renderer.h"

// Allocates and runs all sub-systems essential to operating the game
class Application
{
	// gateware libs used to access operating system
	GW::SYSTEM::GWindow win; // gateware multi-platform window
	GW::SYSTEM::GLog log;
	// REPLACE VULKAN
	//GW::GRAPHICS::GVulkanSurface vulkan; // gateware vulkan API wrapper
	GW::GRAPHICS::GDirectX11Surface d3d11;

	GW::INPUT::GController gamePads; // controller support
	GW::INPUT::GInput immediateInput; // twitch keybaord/mouse
	GW::INPUT::GBufferedInput bufferedInput; // event keyboard/mouse
	GW::AUDIO::GAudio audioEngine; // can create music & sound effects
	// third-party gameplay & utility libraries
	std::shared_ptr<flecs::world> game; // ECS database for gameplay
	std::shared_ptr<GameConfig> gameConfig; // .ini file game settings
	// ECS Entities and Prefabs that need to be loaded
	ESG::BulletData bullets;
	ESG::LaserData lasers;
	ESG::MissileData missiles;
	ESG::DoubleData doubles;
	ESG::BeamData beams;
	ESG::PlayerData players;
    ESG::EnemyData enemies;
	ESG::SoundSystem soundSystem;
	ESG::ShieldData shield;
	ESG::OptionsData options;
	//ESG::BossData boss;

	ESG::PowerTokenData powerTokens;
	// specific ECS systems used to run the game
	ESG::PlayerLogic playerSystem;

	ESG::PowerUp powerSystem;

	// REPLACE VULKAN
	//ESG::VulkanRendererLogic vkRenderingSystem;
	Renderer renderer;
	
	ESG::LevelLogic levelSystem;
	ESG::PhysicsLogic physicsSystem;
	ESG::BulletLogic bulletSystem;
	ESG::EnemyLogic enemySystem;
	//ESG::BossLogic bossSystem;
	ESG::FiringTimer firingTimerSystem;
	ESG::LevelScroll levelScroll;
	ESG::CheatCodes cheatCodesSystem;

	// EventGenerator for Game Events
	GW::CORE::GEventGenerator eventPusher;
	GW::CORE::GEventResponder msgs;
	GW::CORE::GEventReceiver playmsgs;

	std::shared_ptr<Level_Data> levelDataPtr;
	LevelManager levelManager;
	
	//Collisions
	ESG::Collisions collisionSystem;

	bool DebugOn{ false };


	

public:
	bool Init();
	bool Run();
	bool Shutdown();
	

private:
	bool InitWindow();
	bool InitInput();
	bool InitAudio();
	bool InitGraphics();
	bool InitEntities();
	bool InitSystems();
	bool GameLoop();
	bool Unload();
};
//#endif 