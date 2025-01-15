
#ifndef SOUNDSYSTEM
#define SOUNDSYSTEM

namespace ESG {
	struct Sounds { std::vector<GW::AUDIO::GSound> sounds; };
	class SoundSystem {

		// shared connection to the main ECS engine
		std::shared_ptr<flecs::world> game;
		flecs::system system;
	public:
		bool Init(std::shared_ptr<flecs::world>& _game);
		bool Shutdown();
	};
};
#endif // !SOUNDSYSTEM