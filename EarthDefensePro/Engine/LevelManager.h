#pragma once
#include "Timer.h"
#include "../Entities/GameObjects.h"
class LevelManager
{
public:
	int currentLevelIndex;
	GW::SYSTEM::GLog levelLog;
	//Level_Data* currentLevelData = new Level_Data();
	//std::shared_ptr<Level_Data> currentLevelData;
	Timer betweenLevelTime;
	bool canSwitch{ true };

	LevelManager()
	{
		currentLevelIndex = 0;
		CreateLevelLog();
	}
	~LevelManager()
	{
	};

	void CreateLevelLog()
	{
		levelLog.Create("LevelLoadLog.txt");
		levelLog.EnableConsoleLogging(true);
	}

	void LoadLevel(Level_Data& _levelData, int levelIndex = 0)
	{
		levelLog.Log("Level Loading Started");
	//	currentLevelData = std::make_shared< Level_Data>();
		
		bool levelLoaded = _levelData.LoadLevel(levelFilePaths[levelIndex], "../Models", levelLog);;
		/*GameObjects(*currentLevelData, levelLog);*/
		
	}

	void NextLevel(Level_Data& _levelData)
	{
		currentLevelIndex++;
		if (currentLevelIndex >= levelFilePaths.size())
			currentLevelIndex = 0;

		_levelData.UnloadLevel();
		levelLog.Log("Level Switch - Loading Level Now");
		bool levelLoaded = _levelData.LoadLevel(levelFilePaths[currentLevelIndex], "../Models", levelLog);;
	}
};
