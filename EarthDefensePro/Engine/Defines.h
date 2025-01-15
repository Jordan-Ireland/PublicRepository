#pragma once
#ifndef DEFINES_H
#define DEFINES_H
#include <DirectXMath.h>
#define _USE_MATH_DEFINES
#include <math.h>

// This reads .h2b files which are optimized binary .obj+.mtl files
#include "h2bParser.h"
#define MAX_INSTANCES 240

//sine wave
#define OFFSET        1


using namespace DirectX;

// open some namespaces to compact the code a bit
using namespace GW;
using namespace CORE;
using namespace SYSTEM;
using namespace GRAPHICS;

namespace
{
	// File locations for Level Management
	std::string levelPath = "../../Levels/GameLevel.txt";
	std::vector<const char*> levelFilePaths = { "../Levels/GameLevel.txt", "../Levels/MineField.txt", "../Levels/DroneAttack.txt" };
	const char* musicFilePath = "../../Audio/levelMusic.wav";
	const char* soundFilePath = "../../Audio/levelChangeSound.wav";


	UINT windowWidth{ 800 };
	UINT windowHeight{ 600 };

	double deltaTime{ 0 };

	// Directional Light settings
	XMFLOAT4 sunLightColor{ 0.5f, 0.5f, 0.5f, 0.5f };
	XMFLOAT3 sunLightDirection{ -1.0f, -1.0f, 2.0f };

	// Ambient Light setting
	static XMFLOAT4 ambient{ 0.5f, 0.5f, 0.5f, 0.5f };

	// Max number of lights per rubric
	const UINT NUM_POINT_LIGHTS{ 16 };
	const UINT NUM_SPOT_LIGHTS{ 16 };

	float sinewave(float angle, float amplitude = 0.5) {
		return float(amplitude * sin(angle) + 0);
	}

	float lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}

	float distance(GW::MATH::GVECTORF a, GW::MATH::GVECTORF b) {
		return sqrt(pow((b.x - a.x), 2) + pow((b.y - a.y), 2));
	}
	
	wchar_t* const widen(const std::string s)
	{
		wchar_t* wide_string = new wchar_t[s.length() + 1];
		std::copy(s.begin(), s.end(), wide_string);
		wide_string[s.length()] = 0;
		return wide_string;
	}
	struct POINT_LIGHT
	{
		GW::MATH::GMATRIXF transform;
		XMFLOAT4 color;
		float watts;
		float cutoff_distance;
		float pad1, pad2;
	};

	struct SPOT_LIGHT
	{
		GW::MATH::GMATRIXF transform;
		XMFLOAT4 color;
		float watts;
		float cutoff_distance;
		float padding1, padding2;
	};

	struct CB_Scene
	{
		H2B::ATTRIBUTES attributes[150];
		XMFLOAT4 ambient;
		float pointlights;
		float spotlights;
		float pad1, pad2;
	};

	__declspec(align(16))
	struct CB_Object
	{
		XMFLOAT4X4 cbViewMatrix;
		XMFLOAT4X4 cbProjectionMatrix;
		XMFLOAT4 materialIndex;
		uint32_t useTexture;
	};

	struct CB_Frame
	{
		XMFLOAT4 sunLightColor;
		XMFLOAT3 sunLightDir;
		float time;
		POINT_LIGHT plights[NUM_POINT_LIGHTS];
		SPOT_LIGHT slights[NUM_SPOT_LIGHTS];
	};

	struct BulletData
	{
		GW::MATH::GMATRIXF Bullets[MAX_INSTANCES];
		unsigned int vertexCount[MAX_INSTANCES];
		unsigned int indexCount[MAX_INSTANCES];
		unsigned int indexStart[MAX_INSTANCES];
		unsigned int indexOffset[MAX_INSTANCES];
		unsigned int vertexStart[MAX_INSTANCES];
		unsigned int materialStart[MAX_INSTANCES];
		unsigned int materialIndex[MAX_INSTANCES];
		unsigned int modelIndex[MAX_INSTANCES];
		unsigned int transformCount[MAX_INSTANCES];
		unsigned int meshStart[MAX_INSTANCES];
		unsigned int meshCount[MAX_INSTANCES];
		uint32_t useTextures[MAX_INSTANCES];

	}instanceDataObject;

	struct MissileData
	{
		GW::MATH::GMATRIXF Missiles[MAX_INSTANCES];

	}instanceMissileObject;

	float ApplyRenderDistance(GW::MATH::GMATRIXF a, GW::MATH::GMATRIXF b) {
		float dx = a.row4.x - b.row4.x;
		float dy = a.row4.y - b.row4.y;

		return std::sqrt(dx * dx + dy * dy);
	}
	
	const std::vector<int> CHEAT_CODE = {
	G_KEY_UP,G_KEY_UP,G_KEY_DOWN,G_KEY_DOWN,G_KEY_LEFT,G_KEY_RIGHT,G_KEY_LEFT,G_KEY_RIGHT, G_KEY_B,G_KEY_A,G_KEY_ENTER
	};

}
#endif