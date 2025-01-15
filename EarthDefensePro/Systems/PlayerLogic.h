// The player system is responsible for allowing control over the main ship(s)

#ifndef PLAYERLOGIC_H
#define PLAYERLOGIC_H
#pragma once
// Contains our global game settings
#include "../GameConfig.h"
#include "../Components/Physics.h"
#include "../Components/Blender.h"
#include "../Components/Identification.h"
#include "../Components/Visuals.h"
#include "../Components/Gameplay.h"
#include "../Entities/Prefabs.h"
#include "../Events/Playevents.h"
#include "../Events/PEventGenerator.h"
// example space game (avoid name collisions)
namespace ESG 
{
	class PlayerLogic 
	{
		// shared connection to the main ECS engine
		std::shared_ptr<flecs::world> game;
		// non-ownership handle to configuration settings
		std::weak_ptr<const GameConfig> gameConfig;
		// handle to our running ECS system
		flecs::system playerSystem;
		flecs::system powerSystem;
		flecs::system invulSystem;
		// permananent handles input systems
		GW::INPUT::GInput immediateInput;
		GW::INPUT::GBufferedInput bufferedInput;
		GW::INPUT::GController controllerInput;
		// permananent handle to audio system
		GW::AUDIO::GAudio audioEngine;
		// key press event cache (saves input events)
		// we choose cache over responder here for better ECS compatibility
		GW::CORE::GEventCache pressEvents;
		// varibables used for charged shot timing
		float chargeStart = 0, chargeEnd = 0, chargeTime;
		// event responder
		GW::CORE::GEventResponder onExplode;
		GW::CORE::GEventResponder onShoot;
		GW::CORE::GEventGenerator eventPusher;
		unsigned int firingTick1;
		unsigned int firingTick2;
		bool isPaused;
	public:
		// attach the required logic to the ECS 
		bool Init(std::shared_ptr<flecs::world>& _game,
			std::weak_ptr<const GameConfig> _gameConfig,
			GW::INPUT::GInput _immediateInput,
			GW::INPUT::GBufferedInput _bufferedInput,
			GW::INPUT::GController _controllerInput,
			GW::AUDIO::GAudio _audioEngine,
			GW::CORE::GEventGenerator _eventPusher);
		// control if the system is actively running
		bool Activate(bool runSystem);
		// release any resources allocated by the system
		bool Shutdown();
		bool SpawnToken(flecs::world& stage, GW::MATH::GVECTORF pos);

	private:
		// how big the input cache can be each frame
		static constexpr unsigned int Max_Frame_Events = 32;
		// helper routines
		bool ProcessInputEvents(flecs::world& stage, bool ReadyPlayerTwo);
		bool ProcessKeyEvents(flecs::world& stage, bool ReadyPlayerTwo);
		bool ProcessPlayerAction(bool debug, float currentTime);
		void SetPlayerState(flecs::world& stage, bool fireWeaponsP1, bool fireWeaponsP2,
			bool selectPowerP1, bool selectPowerP2);
		void HandlePlayerOneInput(float& xaxis, float& yaxis, bool& isConnected);
		void HandlePlayerKeyBoardMovement(float& xaxis, float& yaxis, bool& _ReadyPlayerTwo, ControllerID& player);
		void HandlePlayerControllerdMovement(float& xaxis, float& yaxis, bool& _ReadyPlayerTwo, ControllerID& player, bool& isConnected);
		void HandlePlayerTwoKeyBoardMovement(float& xaxis, float& yaxis);
		void HandleSoloPlayerInput(float& xaxis, float& yaxis, bool& isConnected);
		void AdjustForBoundaries(ESG::ModelTransform& transform);
		void DisplayDebugInfo(const ESG::ModelTransform& transform);
		void HandleWeapons(flecs::iter& it, ESG::ModelTransform& transform, int controllerIndex, int i);
		bool FireBullets(flecs::world& stage, ESG::ModelTransform origin);
		bool FireLasers(flecs::world& stage, ESG::ModelTransform origin);
		bool FireMissile(flecs::world& stage, ESG::ModelTransform origin);
		bool FireDouble(flecs::world& stage, ESG::ModelTransform origin);
		bool FireBeam(flecs::world& stage, ESG::ModelTransform origin);
		void GetPowerUpComponents(flecs::iter& it, int i);

	};
	

};

#endif