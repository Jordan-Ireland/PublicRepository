#ifndef LEVELSCROLL
#define LEVELSCROLL

namespace ESG {
	class LevelScroll
	{
		// shared connection to the main ECS engine
		std::shared_ptr<flecs::world> game;
		std::vector<float> levelStopping;
		int currentLevel = 0;
		float currentLevelLocation = 0;
		float speed = 1;
		flecs::entity system;
		flecs::entity lsp;

	public:
		// attach the required logic to the ECS 
		bool Init(std::shared_ptr<flecs::world>& _game, float* _levelLength);
		// control if the system is actively running
		bool Activate(bool run );
		bool IsAlive();
		// release any resources allocated by the system
		bool Shutdown();
		bool Update(double elapsed);
	};
}
#endif

