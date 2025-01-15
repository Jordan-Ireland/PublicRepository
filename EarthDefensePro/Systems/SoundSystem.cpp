#include "SoundSystem.h"

using namespace ESG;

bool ESG::SoundSystem::Init(std::shared_ptr<flecs::world>& _game)
{
	// save a handle to the ECS & game settings
	game = _game;
	game->entity("GameSounds").add<Sounds>();
	// destroy any bullets that have the CollidedWith relationship
	system = game->system<Sounds>("Sound System")
		.each([this](flecs::entity e, Sounds&) {
		auto s = e.get_mut<Sounds>();
		for (int i = 0; i < s->sounds.size(); i ++) {
			bool isPlaying;
			s->sounds[i].isPlaying(isPlaying);
			if (!isPlaying) {
				//delete &s->sounds[i];
				s->sounds.erase(s->sounds.begin() + i);
				break;
			}
		}
		e.modified<Sounds>();
			});

	return true;

}

bool ESG::SoundSystem::Shutdown()
{
	game->entity("GameSounds").destruct();
	system.destruct();
	game.reset();

	return true;
}