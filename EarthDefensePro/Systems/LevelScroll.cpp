#include "LevelScroll.h"
#include "../Components/Blender.h"
#include "../Components/Identification.h"
#include "../Components/Gameplay.h"

using namespace ESG;

bool ESG::LevelScroll::Init(std::shared_ptr<flecs::world>& _game,
							float* _levelLength) {
	game = _game;
	for (size_t i = 0; i < sizeof(_levelLength) / sizeof(_levelLength[0]); i++)
	{
		levelStopping.push_back(_levelLength[i]);
	}

	lsp = game->entity("Level Scroll Speed").set<ESG::LevelScrollComp>({speed, this});
	lsp.get_mut<ESG::LevelScrollComp>()->value = speed;
	lsp.modified<ESG::LevelScrollComp>();
	system = game->system<ESG::ModelTransform, ESG::Renderable&>("Level Scroll System")
		.each([this](flecs::entity en, ESG::ModelTransform& t, ESG::Renderable&) {

		auto trans = en.get_mut<ESG::ModelTransform>();
		trans->matrix.row4.x -= speed * en.delta_time();
		en.modified<ESG::ModelTransform>();
		//std::cout << "Scrolling: " << lsp.get_mut<LevelScrollComp>()->levelScrolling << std::endl;
			});
	return true;
}

bool ESG::LevelScroll::Activate(bool run)
{
	if (!run) {
		system.disable();
		auto c = lsp.get_mut<ESG::LevelScrollComp>();
		c->value = 0;
		c->levelScrolling = false;

		lsp.modified<ESG::LevelScrollComp>();
		return false;
	}
	else {
		system.enable();
		auto c = lsp.get_mut<ESG::LevelScrollComp>();
		c->value = speed;
		c->levelScrolling = true;
		lsp.modified<ESG::LevelScrollComp>();
		return true;
	}
}

bool ESG::LevelScroll::IsAlive() {
	return system.enabled();
}

bool ESG::LevelScroll::Update(double elapsed) {
	if (system.enabled()){
		currentLevelLocation += speed * elapsed;
	}
	return false;
}

bool ESG::LevelScroll::Shutdown()
{
	system.destruct();
	game.reset();
	return true;
}
