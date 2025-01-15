#include "Application.h"

// open some Gateware namespaces for conveinence 
// NEVER do this in a header file!
using namespace GW;
using namespace CORE;
using namespace SYSTEM;
using namespace GRAPHICS;

bool Application::Init()
{
	eventPusher.Create();
	
	
	// load all game settigns
	gameConfig = std::make_shared<GameConfig>();
	// create the ECS system
	game = std::make_shared<flecs::world>();

	levelDataPtr = std::make_shared<Level_Data>();
	levelManager.LoadLevel(*levelDataPtr);

	log = levelManager.levelLog;

	flecs::entity levelData = game->entity("LevelData");
	levelData.set<ESG::lvlData>({ *levelDataPtr });


	// init all other systems
	if (InitWindow() == false)
		return false;
	if (InitInput() == false)
		return false;
	if (InitAudio() == false)
		return false;
	if (InitGraphics() == false)
		return false;
	if (InitSystems() == false)
		return false;
	if (InitEntities() == false)
		return false;
	
	return true;
}

bool Application::Run()
{
	std::vector<GW::AUDIO::GSound> sounds;
	playmsgs.Create(eventPusher, [&]() {
		GEvent g;
		ESG::PEventGenerator::Events e;
		ESG::PEventGenerator::EVENT_DATA d;
		if(+playmsgs.Pop(g))
			if(+g.Read(e,d))
				if (e == ESG::PEventGenerator::Events::ENEMY_DESTROYED) {
					
					auto enemyPos = d.pos;
					playerSystem.SpawnToken(*game, enemyPos);
					auto en = game->lookup("GameSounds");
					auto s = en.get_mut<ESG::Sounds>();
					auto p1Score = game->lookup("Spaceship_RaeTheRedPanda");
					auto p2Score = game->lookup("Ship");
					
					if (p2Score != flecs::entity::null())
					{
					auto scoreP2 = p2Score.get_mut<ESG::playerScore>();
					scoreP2->score += 200;
					}
					auto scoreP1 = p1Score.get_mut<ESG::playerScore>();
					scoreP1->score += 200;
					p1Score.modified<ESG::playerScore>();
					s->sounds.push_back(GW::AUDIO::GSound{});
					auto sound = s->sounds.rbegin();
					sound->Create(d.sound, audioEngine, 0.05f);
					sound->Play();
					en.modified<ESG::Sounds>();
				}
				else if (e == ESG::PEventGenerator::Events::EVENT_LEVELWON) {
					levelScroll.Activate(false);
					playerSystem.Activate(false);
					renderer.gameWin = true;
					return true;
					//game->get_world().delete_with(flecs::Wildcard);
					//levelManager.NextLevel(*levelDataPtr);
				}
				else if (e == ESG::PEventGenerator::Events::EVENT_GAMEOVER) {
					levelScroll.Activate(false);
					playerSystem.Activate(false);
					enemySystem.Activate(false);
					renderer.gameover = true;
					return true;
					//game->get_world().delete_with(flecs::Wildcard);
					//levelManager.NextLevel(*levelDataPtr);
				}
				//else if (e == ESG::PEventGenerator::Events::EVENT_PAUSE) {
				//	levelScroll.Activate();
				//	playerSystem.Activate(false);
				//	enemySystem.Activate(false);
				//	return true;
				//	//game->get_world().delete_with(flecs::Wildcard);
				//	//levelManager.NextLevel(*levelDataPtr);
				//}
				//else if (e == ESG::PEventGenerator::Events::EVENT_UNPAUSE) {
				//	levelScroll.Activate();
				//	playerSystem.Activate(true);
				//	enemySystem.Activate(true);
				//	return true;
				//	//game->get_world().delete_with(flecs::Wildcard);
				//	//levelManager.NextLevel(*levelDataPtr);
				//}

		});

	// Timer for delta time
	Timer timer;
	// D3D11 CLEAR BUFFER ADDED
	float clr[] = { 0, 0, 0, 1 };// { 50 / 255.0f, 50 / 255.0f, 50 / 255.0f, 1 };
	msgs.Create([&](const GW::GEvent& e) {
		GW::SYSTEM::GWindow::Events q;
		if (+e.Read(q) && q == GWindow::Events::RESIZE)
			clr[2] += 0.01f; // move towards a cyan as they resize
		});

	// create an event handler to see if the window was closed early
	bool winClosed = false;
	GW::CORE::GEventResponder winHandler;
	winHandler.Create([&winClosed](GW::GEvent e) {
		GW::SYSTEM::GWindow::Events ev;
		if (+e.Read(ev) && ev == GW::SYSTEM::GWindow::Events::DESTROY)
			winClosed = true;
		});
	win.Register(winHandler);
	timer.Start();
	while (+win.ProcessWindowEvents())
	{
		if (winClosed == true)
			return true;

		IDXGISwapChain* swap = nullptr;
		ID3D11DeviceContext* con = nullptr;
		ID3D11RenderTargetView* view = nullptr;
		ID3D11DepthStencilView* depth = nullptr;
		if (+d3d11.GetImmediateContext((void**)&con) &&
			+d3d11.GetRenderTargetView((void**)&view) &&
			+d3d11.GetDepthStencilView((void**)&depth) &&
			+d3d11.GetSwapchain((void**)&swap))
		{
			con->ClearRenderTargetView(view, clr);
			con->ClearDepthStencilView(depth, D3D11_CLEAR_DEPTH, 1, 0);
			//if (DebugOn)
			//{
			//	// Get Delta Time
			//	deltaTime = timer.GetDeltaTime();
			//	//renderer.HandleInput(deltaTime);
			//}
			//renderer.Update();
			//renderer.Render();
			//GO.Update(game, levelDataPtr, log, immediateInput, .10);
			GameLoop();
			renderer.RenderUI(levelScroll);
			swap->Present(1, 0);
			// release incremented COM reference counts
			swap->Release();
			view->Release();
			depth->Release();
			con->Release();
			
		}
	}
	return true;
}
bool Application::Unload()
{
	if (bullets.Unload(game) == false)
		return false; 
	if (lasers.Unload(game) == false)
		return false;
	if (missiles.Unload(game) == false)
		return false;
	if (doubles.Unload(game) == false)
		return false;
	if (beams.Unload(game) == false)
		return false;
	// Load the player entities
	if (players.Unload(game) == false)
		return false;
	// Load the enemy entities
	if (enemies.Unload(game) == false)
		return false;
	/*if (boss.Unload(game) == false)
		return false;*/
	if (powerTokens.Unload(game) == false)
		return false;
	if (shield.Unload(game) == false)
		return false;
	if (options.Unload(game) == false)
		return false;
}
bool Application::Shutdown()
{
	// disconnect systems from global ECS
	/*if (playerSystem.Shutdown() == false)
		return false;*/

	// NOT SURE WE NEED A SHUTDOWN WITH THE USE OF SMARTPOINTERS AND D3D11
	/*if (vkRenderingSystem.Shutdown() == false)
		return false;*/
	/*if (levelScroll.Shutdown() == false)
		return false;*/
	/*if (soundSystem.Shutdown() == false)
		return false;*/

	if (physicsSystem.Shutdown() == false)
		return false;
	if (bulletSystem.Shutdown() == false)
		return false;
	if (enemySystem.Shutdown() == false)
		return false;
	if (powerSystem.Shutdown() == false)
		return false;
	if (levelSystem.Shutdown() == false)
		return false;

	/*game->system<ESG::Highscore>()
		.each([this](ESG::Highscore& h) {
		(*gameConfig)["Highscore"]["player1"] = h.score1;
		(*gameConfig)["Highscore"]["player2"] = h.score2;
			});*/
	return true;
}

bool Application::InitWindow()
{
	// grab settings
	int width = gameConfig->at("Window").at("width").as<int>();
	int height = gameConfig->at("Window").at("height").as<int>();
	int xstart = gameConfig->at("Window").at("xstart").as<int>();
	int ystart = gameConfig->at("Window").at("ystart").as<int>();
	std::string title = gameConfig->at("Window").at("title").as<std::string>();
	// open window
	if (+win.Create(xstart, ystart, width, height, GWindowStyle::WINDOWEDLOCKED) && +win.SetWindowName(title.c_str()))
	{
		return true;
	}
	return false;
}

bool Application::InitInput()
{
	if (-gamePads.Create())
		return false;
	if (-immediateInput.Create(win))
		return false;
	if (-bufferedInput.Create(win))
		return false;
	return true;
}

bool Application::InitAudio()
{
	if (-audioEngine.Create())
		return false;
	return true;
}

bool Application::InitGraphics()
{
	if (+d3d11.Create(win, GW::GRAPHICS::DEPTH_BUFFER_SUPPORT))
		return true;
}

bool Application::InitEntities()
{
	GameObjects GO(game, levelDataPtr, log);

	// Load bullet prefabs
	if (bullets.Load(game, gameConfig, audioEngine) == false)
		return false;
	if (lasers.Load(game, gameConfig, audioEngine) == false)
		return false;
	if (missiles.Load(game, gameConfig, audioEngine) == false)
		return false;
	if (doubles.Load(game, gameConfig, audioEngine) == false)
		return false;
	if (beams.Load(game, gameConfig, audioEngine) == false)
		return false;
	// Load the player entities
	if (players.Load(game, gameConfig) == false)
		return false;
	// Load the enemy entities
	if (enemies.Load(game, gameConfig, audioEngine) == false)
		return false;
	/*if (boss.Load(game, gameConfig, audioEngine) == false)
		return false;*/
	if (powerTokens.Load(game, gameConfig, audioEngine) == false)
		return false;
	if (shield.Load(game, gameConfig, audioEngine) == false)
		return false;
	if (options.Load(game, gameConfig, audioEngine) == false)
		return false;

	return true;
}

bool Application::InitSystems()
{
	// connect systems to global ECS
	if (levelSystem.Init(game, gameConfig, audioEngine, eventPusher) == false)
		return false;
	
	if (renderer.Init(game, gameConfig, d3d11, win,levelDataPtr) == false)
		return false;
	float t[1]{ 245 };
	if (levelScroll.Init(game, t) == false)
		return false;
	if (soundSystem.Init(game) == false)
		return false;
	if (playerSystem.Init(game, gameConfig, immediateInput, bufferedInput,
		gamePads, audioEngine, eventPusher) == false)
		return false;
	if (physicsSystem.Init(game, gameConfig) == false)
		return false;
	if (bulletSystem.Init(game, gameConfig) == false)
		return false;
	if (firingTimerSystem.Init(game, gameConfig) == false)
		return false;
	if (enemySystem.Init(game, gameConfig, eventPusher) == false)
		return false;
	/*if (bossSystem.Init(game, gameConfig, eventPusher) == false)
		return false;*/
	if (powerSystem.Init(game, gameConfig, &immediateInput, &gamePads) == false)
		return false;
	if (collisionSystem.Init(game, gameConfig) == false)
		return false;
	if (cheatCodesSystem.Init(game, gameConfig, immediateInput, bufferedInput,
		gamePads, audioEngine, eventPusher) == false)

	levelScroll.Activate(false);
	return true;
}

bool Application::GameLoop()
{
	// compute delta time and pass to the ECS system
	static auto start = std::chrono::steady_clock::now();
	double elapsed = std::chrono::duration<double>(
		std::chrono::steady_clock::now() - start).count();
	start = std::chrono::steady_clock::now();
	// let the ECS system run
	return game->progress(static_cast<float>(elapsed));
}

