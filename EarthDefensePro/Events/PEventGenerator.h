#ifndef ESGEVENTS
#define ESGEVENTS

namespace ESG {
	class PEventGenerator : public virtual I::GEventGeneratorInterface
	{
	public:
		enum class Events
		{
			ENEMY_DESTROYED,
			EVENT_COUNT,
			EVENT_LEVELWON,
			EVENT_GAMEOVER,
			EVENT_WIN,
			EVENT_PAUSE,
			EVENT_UNPAUSE
		};

		struct EVENT_DATA
		{
			const char* sound; // which entity was affected?
			GW::MATH::GVECTORF pos;
		};
	};
};

#endif // !ESGEVENTS