#pragma once

#include <d3dcompiler.h>
#include <d3d11.h>

// DirectXTK
#include <DDSTextureLoader.h>
#include <SpriteBatch.h>
#include <CommonStates.h>
#include <SimpleMath.h>
#include <WICTextureLoader.h>
#include <SpriteFont.h>

#include "../Application.h"

#include "FileIntoString.h"
#include "TextureArray.h"

#include "../Engine/VertexBuffer.h"
#include "../Engine/InstanceBuffer.h"
#include "../Engine/IndexBuffer.h"
#include "../Engine/ConstantBuffer.h"
#include "../Engine/Camera.h"


inline void PrintLabeledDebugString(const char* label, const char* toPrint)
{
	std::cout << label << toPrint << std::endl;
#if defined WIN32 //OutputDebugStringA is a windows-only function 
	OutputDebugStringA(label);
	OutputDebugStringA(toPrint);
#endif
}

// Creation, Rendering & Cleanup
class Renderer
{
	// Handles
	GW::SYSTEM::GWindow win;
	GW::GRAPHICS::GDirectX11Surface d3d;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> vertexFormat;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> matrixBufferSRV;

	// Buffers
	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;

	// Constant Buffers
	ConstantBuffer<CB_Scene> sceneBuffer;
	ConstantBuffer<CB_Frame> frameBuffer;
	ConstantBuffer<CB_Object> objectBuffer;


	Microsoft::WRL::ComPtr<ID3D11Buffer> matrixBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> stagingBuffer;

	// Model Texture and Resource  - all models use same texture - not ideal just quick implementation
	std::vector<ESG::TextureArray> loadedTextures;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	// Rasterizer States
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rsDefault;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rsWireframe;

	// Constant Buffer Data Holders
	CB_Object cb_Object;
	CB_Frame cb_Frame;
	CB_Scene cb_Scene;

	// Flecs
	std::shared_ptr<flecs::world> game;
	flecs::system updateDraw;
	flecs::system completeDraw;
	flecs::system completeDrawInstance;
	flecs::system startDraw;
	flecs::system updateBullet;
	flecs::system updateLaser;
	flecs::system updateMissile;
	flecs::system updateDouble;
	flecs::system updateBeam;
	flecs::system updateTokens;
	flecs::system PlayerUI;
	flecs::system PowerMeterUI;
	flecs::system updateScore;
	flecs::system updateLives;

	//powerup UI system
	flecs::system speedUI;
	flecs::system missileUI;
	flecs::system laserUI;
	flecs::system doubleUI;
	flecs::system optionUI;
	flecs::system shieldUI;
	flecs::system beamUI;

	flecs::system renderDistance;

	//	flecs::query<ESG::Bullet,ESG::ModelTransform> bulletQuery;
	int draw_counter = 0;
	int bulletInstances = 0;

	Level_Data* lvl;

	Camera camera = Camera((float)800 / 400);

	// UI TRIAL
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_powerupBar;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_puSpeed;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_puMissle;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_puDouble;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_puLaser;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_puOption;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_puShield;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_puBeam;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_mainMenu;

	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	DirectX::SimpleMath::Vector2 m_screenPosP1;
	DirectX::SimpleMath::Vector2 m_screenPosP2;
	DirectX::SimpleMath::Vector2 m_puPosP1;
	DirectX::SimpleMath::Vector2 m_puPosP2;

	CD3D11_TEXTURE2D_DESC lifeDesc;

	DirectX::SimpleMath::Vector2 m_origin;
	std::unique_ptr<DirectX::CommonStates> m_states;
	/*RECT m_fullscreenRect;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_background;*/

	// ?????
	ID3D11DepthStencilState* depthStencilState = nullptr;

	Timer timer;
	bool scroll{ false };

	// FONT TRIAL
	std::unique_ptr<DirectX::SpriteFont> m_font;
	DirectX::SimpleMath::Vector2 m_fontPos;
	bool player1{ false };
	bool player2{ false };
	int p1TokenCount{ 1 };
	int p2TokenCount{ 1 };


	// Screen Overlay
	RECT m_fullscreenRect;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_overlay;

	int p1Score{ 0 };
	int p2Score{ 0 };
	const wchar_t* player1ScoreString;
	const wchar_t* player2ScoreString;
	const wchar_t* gameOverString;
	int p1Lives{ 0 };
	int p2Lives{ 0 };

	bool p1Speed{ true };
	bool p2Speed{ true };

	bool p1Missile{ true };
	bool p2Missile{ true };

	bool p1Double{ true };
	bool p2Double{ true };

	bool p1Laser{ true };
	bool p2Laser{ true };

	bool p1Option{ false };
	bool p2Option{ false };

	bool p1Shield{ false };
	bool p2Shield{ false };

	bool p1Beam{ true };
	bool p2Beam{ true };


	//int p2TokenCount{ 20 };
	std::shared_ptr<GameConfig> gameConfig;

public:
	bool splitscreen{ false };
	bool mainMenu{ true };
	bool credits{ false };
	bool optionMenu{ false };
	int musicLevel{ 10 };
	int soundLevel{ 10 };

	bool paused{ false };
	bool gameover{ false };
	bool gameWin{ false };
	unsigned int highscorePlayer1;
	unsigned int highscorePlayer2;

	bool Init(std::shared_ptr<flecs::world>& _game,
		std::weak_ptr<GameConfig> _gameConfig,
		GW::GRAPHICS::GDirectX11Surface _d3d,
		GW::SYSTEM::GWindow _win,
		std::shared_ptr<Level_Data>& _lvl)
	{
		win = _win;
		d3d = _d3d;
		game = _game;
		gameConfig = _gameConfig.lock();

		timer.Start();

		std::shared_ptr<const GameConfig> readCfg = _gameConfig.lock();
		highscorePlayer1 = (*readCfg).at("Highscore").at("player1").as<unsigned int>();
		highscorePlayer2 = (*readCfg).at("Highscore").at("player2").as<unsigned int>();

		flecs::entity l = game->lookup("LevelData");

		if (l.is_valid())
		{
			auto lv = l.get_mut<ESG::lvlData>();
			lvl = &lv->levelData;
		}
		else
		{
			return false;
		}

		InitializeGraphics();

		return true;
	}

private:
	void InitializeGraphics()
	{
		ID3D11Device* creator = nullptr;
		d3d.GetDevice((void**)&creator);
		PipelineHandles curHandles = GetCurrentPipelineHandles();
		InitializePipeline(creator);
		CreateRasterizerStates(creator);

		objectBuffer.Initialize(creator);
		frameBuffer.Initialize(creator);
		sceneBuffer.Initialize(creator);
		InitializeStructuredBuffer(creator, curHandles.context);

		// Initialize Scene Buffers
		vertexBuffer.Initialize(creator, lvl->levelVertices.data(), sizeof(H2B::VERTEX) * lvl->levelVertices.size());
		indexBuffer.Initialize(creator, lvl->levelIndices.data(), sizeof(UINT) * lvl->levelIndices.size());


		InitializeScene(*lvl, curHandles);

		// Create Texture from file - models
		HRESULT hr = E_NOTIMPL;
		for (size_t i = 0; i < lvl->Materials.size(); i++)
		{
			std::string folder = "../Textures/";
			//std::wstring folder = L"../Textures/";
			/*const wchar_t* map_Kd = (folder + std::wstring(lvl.Materials[i].map_Kd.begin(), lvl.Materials[i].map_Kd.end())).c_str();
			const wchar_t* map_Ks = (folder + std::wstring(lvl.Materials[i].map_Ks.begin(), lvl.Materials[i].map_Ks.end())).c_str();
			const wchar_t* map_Ka = (folder + std::wstring(lvl.Materials[i].map_Ka.begin(), lvl.Materials[i].map_Ka.end())).c_str();
			const wchar_t* map_Ns = (folder + std::wstring(lvl.Materials[i].map_Ns.begin(), lvl.Materials[i].map_Ns.end())).c_str();*/

			std::string filePaths[4] = { folder + lvl->Materials[i].map_Kd, folder + lvl->Materials[i].map_Ks, folder + lvl->Materials[i].map_Ka, folder + lvl->Materials[i].map_Ns };
			loadedTextures.push_back(ESG::TextureArray(creator, filePaths));
		}
		// Create sampler for model textures
		CD3D11_SAMPLER_DESC samp_desc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
		creator->CreateSamplerState(&samp_desc, samplerState.GetAddressOf());

		// Create LifeBar
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		hr = CreateDDSTextureFromFile(creator, L"../Textures/shipWhite.dds", resource.GetAddressOf(), m_texture.ReleaseAndGetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> life;
		hr = resource.As(&life);
		life->GetDesc(&lifeDesc);

		// Create PowerUpBar
		Microsoft::WRL::ComPtr<ID3D11Resource> resourcePUBar;
		hr = CreateWICTextureFromFile(creator, L"../Textures/puBG.png", resourcePUBar.GetAddressOf(), m_powerupBar.ReleaseAndGetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pu1;
		hr = resourcePUBar.As(&pu1);
		CD3D11_TEXTURE2D_DESC pu1Desc;
		pu1->GetDesc(&pu1Desc);

		// Create Speed Label
		Microsoft::WRL::ComPtr<ID3D11Resource> resourceSpeed;
		hr = CreateWICTextureFromFile(creator, L"../Textures/puSpeed.png", resourceSpeed.GetAddressOf(), m_puSpeed.ReleaseAndGetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> puSpeed;
		hr = resourceSpeed.As(&puSpeed);
		CD3D11_TEXTURE2D_DESC speedDesc;
		puSpeed->GetDesc(&speedDesc);

		// Create Missle Label
		Microsoft::WRL::ComPtr<ID3D11Resource> resourceMissle;
		hr = CreateWICTextureFromFile(creator, L"../Textures/puMissle.png", resourceMissle.GetAddressOf(), m_puMissle.ReleaseAndGetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> puMissle;
		hr = resourceSpeed.As(&puMissle);
		CD3D11_TEXTURE2D_DESC missleDesc;
		puMissle->GetDesc(&missleDesc);

		// Create Double Label
		Microsoft::WRL::ComPtr<ID3D11Resource> resourceDouble;
		hr = CreateWICTextureFromFile(creator, L"../Textures/puDouble.png", resourceDouble.GetAddressOf(), m_puDouble.ReleaseAndGetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> puDouble;
		hr = resourceDouble.As(&puDouble);
		CD3D11_TEXTURE2D_DESC doubleDesc;
		puDouble->GetDesc(&doubleDesc);

		// Create Laser Label
		Microsoft::WRL::ComPtr<ID3D11Resource> resourceLaser;
		hr = CreateWICTextureFromFile(creator, L"../Textures/puLaser.png", resourceLaser.GetAddressOf(), m_puLaser.ReleaseAndGetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> puLaser;
		hr = resourceLaser.As(&puLaser);
		CD3D11_TEXTURE2D_DESC laserDesc;
		puLaser->GetDesc(&laserDesc);

		// Create Option Label
		Microsoft::WRL::ComPtr<ID3D11Resource> resourceOption;
		hr = CreateWICTextureFromFile(creator, L"../Textures/puOption.png", resourceOption.GetAddressOf(), m_puOption.ReleaseAndGetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> puOption;
		hr = resourceOption.As(&puOption);
		CD3D11_TEXTURE2D_DESC optionDesc;
		puOption->GetDesc(&optionDesc);

		// Create Shield Label
		Microsoft::WRL::ComPtr<ID3D11Resource> resourceShield;
		hr = CreateWICTextureFromFile(creator, L"../Textures/puShield.png", resourceShield.GetAddressOf(), m_puShield.ReleaseAndGetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> puShield;
		hr = resourceShield.As(&puShield);
		CD3D11_TEXTURE2D_DESC shieldDesc;
		puShield->GetDesc(&shieldDesc);

		// Create Beam Label
		Microsoft::WRL::ComPtr<ID3D11Resource> resourceBeam;
		hr = CreateWICTextureFromFile(creator, L"../Textures/puBeam.png", resourceBeam.GetAddressOf(), m_puBeam.ReleaseAndGetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> puBeam;
		hr = resourceBeam.As(&puBeam);
		CD3D11_TEXTURE2D_DESC beamDesc;
		puBeam->GetDesc(&beamDesc);

		// Create Main Menu Image
		Microsoft::WRL::ComPtr<ID3D11Resource> resourceMM;
		hr = CreateWICTextureFromFile(creator, L"../Textures/MainMenu.png", resourceMM.GetAddressOf(), m_mainMenu.ReleaseAndGetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mm;
		hr = resourceMM.As(&mm);
		CD3D11_TEXTURE2D_DESC mmDesc;
		mm->GetDesc(&mmDesc);

		if (setupDrawCall() == false)
			return;
	}

	void InitializePipeline(ID3D11Device* creator)
	{
		UINT compilerFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
		compilerFlags |= D3DCOMPILE_DEBUG;
#endif
		Microsoft::WRL::ComPtr<ID3DBlob> vsBlob = CompileVertexShader(creator, compilerFlags);
		Microsoft::WRL::ComPtr<ID3DBlob> psBlob = CompilePixelShader(creator, compilerFlags);
		CreateVertexInputLayout(creator, vsBlob);
	}

	Microsoft::WRL::ComPtr<ID3DBlob> CompileVertexShader(ID3D11Device* creator, UINT compilerFlags)
	{
		std::string vertexShaderSource = ReadFileIntoString("../Shaders/VertexShader.hlsl");

		Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, errors;

		HRESULT compilationResult =
			D3DCompile(vertexShaderSource.c_str(), vertexShaderSource.length(),
				nullptr, nullptr, nullptr, "main", "vs_4_0", compilerFlags, 0,
				vsBlob.GetAddressOf(), errors.GetAddressOf());

		if (SUCCEEDED(compilationResult))
		{
			creator->CreateVertexShader(vsBlob->GetBufferPointer(),
				vsBlob->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
		}
		else
		{
			PrintLabeledDebugString("Vertex Shader Errors:\n", (char*)errors->GetBufferPointer());
			abort();
			return nullptr;
		}

		return vsBlob;
	}

	Microsoft::WRL::ComPtr<ID3DBlob> CompilePixelShader(ID3D11Device* creator, UINT compilerFlags)
	{
		std::string pixelShaderSource = ReadFileIntoString("../Shaders/PixelShader.hlsl");

		Microsoft::WRL::ComPtr<ID3DBlob> psBlob, errors;

		HRESULT compilationResult =
			D3DCompile(pixelShaderSource.c_str(), pixelShaderSource.length(),
				nullptr, nullptr, nullptr, "main", "ps_4_0", compilerFlags, 0,
				psBlob.GetAddressOf(), errors.GetAddressOf());

		if (SUCCEEDED(compilationResult))
		{
			creator->CreatePixelShader(psBlob->GetBufferPointer(),
				psBlob->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
		}
		else
		{
			PrintLabeledDebugString("Pixel Shader Errors:\n", (char*)errors->GetBufferPointer());
			abort();
			return nullptr;
		}

		return psBlob;
	}

	void CreateVertexInputLayout(ID3D11Device* creator, Microsoft::WRL::ComPtr<ID3DBlob>& vsBlob)
	{
		D3D11_INPUT_ELEMENT_DESC format[] = {
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"UVW", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"NORM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			}
		};
		creator->CreateInputLayout(format, ARRAYSIZE(format),
			vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
			vertexFormat.GetAddressOf());
	}

	void CreateRasterizerStates(ID3D11Device* creator)
	{
		CD3D11_RASTERIZER_DESC normalDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
		normalDesc.FillMode = D3D11_FILL_SOLID;
		HRESULT res = creator->CreateRasterizerState(&normalDesc, rsDefault.GetAddressOf());

		CD3D11_RASTERIZER_DESC wireframeDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
		wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
		res = creator->CreateRasterizerState(&wireframeDesc, rsWireframe.GetAddressOf());
	}

	void InitializeStructuredBuffer(Microsoft::WRL::ComPtr<ID3D11Device> creator, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		//unsigned int matSize = sizeof(XMFLOAT4X4) * (lvl->levelTransforms.size() + 100);
		unsigned int lvlSize = sizeof(XMFLOAT4X4) * lvl->levelTransforms.size();
		unsigned int instanceSize = sizeof(XMFLOAT4X4) * MAX_INSTANCES;
		unsigned int missileSize = sizeof(XMFLOAT4X4) * MAX_INSTANCES;
		unsigned int matSize = instanceSize;// lvlSize + instanceSize + missileSize;

		// Define the DEFAULT buffer description
		D3D11_BUFFER_DESC defaultBufferDesc = {};
		defaultBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		defaultBufferDesc.ByteWidth = matSize;
		defaultBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		defaultBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		defaultBufferDesc.StructureByteStride = sizeof(GW::MATH::GMATRIXF);

		// Create DEFAULT buffer
		HRESULT hr = creator->CreateBuffer(&defaultBufferDesc, nullptr, matrixBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			// Handle error
			return;
		}

		// Define the STAGING buffer description
		D3D11_BUFFER_DESC stagingBufferDesc = {};
		stagingBufferDesc.Usage = D3D11_USAGE_STAGING;
		stagingBufferDesc.ByteWidth = matSize;
		stagingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		// Create STAGING buffer
		hr = creator->CreateBuffer(&stagingBufferDesc, nullptr, stagingBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			// Handle error
			return;
		}

		// Map the staging buffer, write new data, then unmap
		D3D11_MAPPED_SUBRESOURCE mappedData;
		hr = context->Map(stagingBuffer.Get(), 0, D3D11_MAP_WRITE, 0, &mappedData);
		if (SUCCEEDED(hr))
		{
			// Copy the level transforms
			memcpy(mappedData.pData, &instanceDataObject.Bullets, matSize);
			context->Unmap(stagingBuffer.Get(), 0);
		}
		else { return; }

		// Copy data from the staging buffer to the default buffer
		context->CopyResource(matrixBuffer.Get(), stagingBuffer.Get());

		// Create Shader Resource View (SRV) for the DEFAULT buffer
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = MAX_INSTANCES;//lvl->levelTransforms.size() + MAX_INSTANCES + MAX_INSTANCES;

		hr = creator->CreateShaderResourceView(matrixBuffer.Get(), &srvDesc, &matrixBufferSRV);
		if (FAILED(hr))
		{
			// Handle error
			return;
		}
	}


public:
	void RenderUI(ESG::LevelScroll ls)
	{
		DirectX::SimpleMath::Vector2 origin;
		const wchar_t* output;

		// TEMPORARY PLAYER 2 VARIABLE

		// Player 1
		//bool player1{ true };
		//
		////int p1TokenCount{ 1 };
		//bool p1Speed{ true };
		//bool p1Missle{ true };
		//bool p1Double{ true };
		//bool p1Laser{ true };
		//bool p1Option{ true };
		//bool p1Shield{ true };
		//bool p1Beam{ true };
		//// Player 2
		//bool player2{ true };
		//
		////int p2TokenCount{ 20 };
		//bool p2Speed{ true };
		//bool p2Missle{ true };
		//bool p2Double{ true };
		//bool p2Laser{ true };
		//bool p2Option{ true };
		//bool p2Shield{ true };
		//bool p2Beam{ true };





		ID3D11Device* creator = nullptr;
		d3d.GetDevice((void**)&creator);
		PipelineHandles curHandles = GetCurrentPipelineHandles();
		// UI TRIAL
		HRESULT hr;
		m_states = std::make_unique<CommonStates>(creator);
		m_spriteBatch = std::make_unique<SpriteBatch>(curHandles.context);


		UINT clientWidth, clientHeight;
		win.GetClientWidth(clientWidth);
		win.GetClientHeight(clientHeight);

		// Set Rect to full screen
		m_fullscreenRect.top = 0;
		m_fullscreenRect.bottom = float(clientHeight);
		m_fullscreenRect.left = 0;
		m_fullscreenRect.right = float(clientWidth);

		m_origin.x = float(lifeDesc.Width / 2);
		m_origin.y = float(lifeDesc.Height / 2);

		// FONT TRIAL
		m_font = std::make_unique<SpriteFont>(creator, L"../Fonts/myfile.spritefont");

		// Bind texture resource and sampler
		curHandles.context->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
		curHandles.context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
		curHandles.context->OMSetDepthStencilState(depthStencilState, 1);
		curHandles.context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

		if (GetAsyncKeyState('3'))
		{
			splitscreen = true;
		}
		// Begin Batch Draw
		m_spriteBatch->Begin();

		if (mainMenu)
		{
			ls.Activate(false);
			if (GetAsyncKeyState('C') || GetAsyncKeyState(VK_GAMEPAD_Y))
			{
				mainMenu = false;
				credits = true;
			}
			if (GetAsyncKeyState('O') || GetAsyncKeyState(VK_GAMEPAD_X))
			{
				mainMenu = false;
				optionMenu = true;
			}
			if (GetAsyncKeyState('1') || GetAsyncKeyState(VK_GAMEPAD_A))
			{
				ls.Activate(true);
				mainMenu = false;
				player1 = true;
			}
			if (GetAsyncKeyState(VK_TAB) || GetAsyncKeyState(VK_GAMEPAD_B))
			{
				optionMenu = false;
				mainMenu = true;
			}
			// Draw Main Menu Background
			m_spriteBatch->Draw(m_mainMenu.Get(), m_fullscreenRect);
			// Draw Title
			output = L"Earth Defense Pro";
			origin = m_font->MeasureString(output) / 2.f;
			// Middle Screen POSITION
			m_fontPos.x = float(clientWidth) / 2.f;
			m_fontPos.y = float(clientHeight) / 2.f - 100.f;
			// Font Drop Shadow
			/*m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin);*/
				// Font Outline
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, -1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, -1.f), Colors::Red, 0.f, origin);
			// Draw the font
			m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Blue, 0.f, origin);

			// Draw Title
			output = L"Press 1 To Play";
			origin = m_font->MeasureString(output) / 2.f;
			// Middle Screen POSITION
			m_fontPos.x = float(clientWidth) / 2.f;
			m_fontPos.y = float(clientHeight) / 2.f;
			// Font Drop Shadow
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin, 0.5f);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin, 0.5f);
			// Font Outline
			/*m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, -1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, -1.f), Colors::Red, 0.f, origin);*/
				// Draw the font
			m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Blue, 0.f, origin, 0.5f);

			// Draw Title
			output = L"Press O For Options";
			origin = m_font->MeasureString(output) / 2.f;
			// Middle Screen POSITION
			m_fontPos.x = float(clientWidth) / 2.f;
			m_fontPos.y = float(clientHeight) / 2.f + 180.f;
			// Font Drop Shadow
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin, 0.3f);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin, 0.3f);
			// Font Outline
			/*m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, -1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, -1.f), Colors::Red, 0.f, origin);*/
				// Draw the font
			m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Yellow, 0.f, origin, 0.3f);

			// Draw Title
			output = L"Press C For Credits";
			origin = m_font->MeasureString(output) / 2.f;
			// Middle Screen POSITION
			m_fontPos.x = float(clientWidth) / 2.f;
			m_fontPos.y = float(clientHeight) / 2.f + 150.f;
			// Font Drop Shadow
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin, 0.3f);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin, 0.3f);
			// Font Outline
			/*m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, -1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, -1.f), Colors::Red, 0.f, origin);*/
				// Draw the font
			m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Yellow, 0.f, origin, 0.3f);
		}
		else if (paused)
		{
			// Draw Title
			output = L"Paused";
			origin = m_font->MeasureString(output) / 2.f;
			// Middle Screen POSITION
			m_fontPos.x = float(clientWidth) / 2.f;
			m_fontPos.y = float(clientHeight) / 2.f;
			// Font Drop Shadow
			/*m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin);*/
				// Font Outline
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, -1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, -1.f), Colors::Red, 0.f, origin);
			// Draw the font
			m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Blue, 0.f, origin);

			// Draw Title
			output = L"Press Enter To Continue";
			origin = m_font->MeasureString(output) / 2.f;
			// Middle Screen POSITION
			m_fontPos.x = float(clientWidth) / 2.f;
			m_fontPos.y = float(clientHeight) / 2.f + 50.f;
			// Font Drop Shadow
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin, 0.5f);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin, 0.5f);
			// Font Outline
			/*m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, -1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, -1.f), Colors::Red, 0.f, origin);*/
				// Draw the font
			m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Blue, 0.f, origin, 0.5f);
		}
		else if (optionMenu)
		{
			if (GetAsyncKeyState(VK_TAB) || GetAsyncKeyState(VK_GAMEPAD_B))
			{
				optionMenu = false;
				mainMenu = true;
			}
			m_spriteBatch->Draw(m_mainMenu.Get(), m_fullscreenRect);
			// Draw Title
			output = L"Options";
			origin = m_font->MeasureString(output) / 2.f;
			// Middle Screen POSITION
			m_fontPos.x = float(clientWidth) / 2.f;
			m_fontPos.y = float(clientHeight) / 2.f - 100.f;
			// Font Drop Shadow
			/*m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin, 0.5f);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin, 0.5f);*/
				// Font Outline
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, -1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, -1.f), Colors::Red, 0.f, origin);
			// Draw the font
			m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Blue, 0.f, origin);

			// Draw Title
			output = L"Music Volume : ";
			origin = m_font->MeasureString(output) / 2.f;
			// Middle Screen POSITION
			m_fontPos.x = float(clientWidth) / 2.f - 170.f;
			m_fontPos.y = float(clientHeight) / 2.f - 50.f;
			// Font Drop Shadow
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin, 0.3f);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin, 0.3f);
			m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Yellow, 0.f, origin, 0.3f);
			// Draw bars
			m_puPosP1.x = float(clientWidth) / 2.f - 70.f;
			m_puPosP1.y = float(clientHeight) / 2.f - 50.f;
			for (int i = 0; i < musicLevel; ++i)
			{
				m_spriteBatch->Draw(m_powerupBar.Get(), m_puPosP1, nullptr, Colors::Yellow, 0.f, m_origin, 0.05f);
				m_puPosP1.x += 30.f;
			}

			// Draw Title
			output = L"Sound Volume : ";
			origin = m_font->MeasureString(output) / 2.f;
			// Middle Screen POSITION
			m_fontPos.x = float(clientWidth) / 2.f - 170.f;
			m_fontPos.y = float(clientHeight) / 2.f - 25.f;
			// Font Drop Shadow
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin, 0.3f);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin, 0.3f);
			m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Yellow, 0.f, origin, 0.3f);
			// Draw bars
			m_puPosP1.x = float(clientWidth) / 2.f - 70.f;
			m_puPosP1.y = float(clientHeight) / 2.f - 25.f;
			for (int i = 0; i < musicLevel; ++i)
			{
				m_spriteBatch->Draw(m_powerupBar.Get(), m_puPosP1, nullptr, Colors::Yellow, 0.f, m_origin, 0.05f);
				m_puPosP1.x += 30.f;
			}

			// Draw Title
			output = L"Press TAB To Return To Main Menu";
			origin = m_font->MeasureString(output) / 2.f;
			// Middle Screen POSITION
			m_fontPos.x = float(clientWidth) / 2.f;
			m_fontPos.y = float(clientHeight) / 2.f + 180.f;
			// Font Drop Shadow
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin, 0.3f);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin, 0.3f);
			// Font Outline
			/*m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, -1.f), Colors::Red, 0.f, origin);
			m_font->DrawString(m_spriteBatch.get(), output,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, -1.f), Colors::Red, 0.f, origin);*/
				// Draw the font
			m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Yellow, 0.f, origin, 0.3f);
		}
		else if (credits)
		{
			if (GetAsyncKeyState(VK_TAB))
			{
				credits = false;
				mainMenu = true;
			}
			m_spriteBatch->Draw(m_mainMenu.Get(), m_fullscreenRect);
			const wchar_t* credits = L"Lead Programmer"
				"\nJordan Ireland"
				"\n\n\nGamePlay Programmer"
				"\nJestin McMahon"
				"\n\n\nGeneralist/Gameplay Programmer"
				"\nJacob Lacey"
				"\n\n\nGraphics Programmer"
				"\nArnn Powell"
				"\n\n\nGuilherme Silva"
				"\n\n\n\n\nModels"
				"\nMissile"
				"\nhttps://poly.pizza/m/1Xid2Qhqn2s"
				"\nBullet"
				"\nhttps://poly.pizza/m/dDgEYvR1mLP"
				"\nSynty Studios SciFi Pack"
				"\n\n\nPress TAB To Return To Main Menu";
			// Draw Title
			output = L"Press TAB To Return To Main Menu";
			origin = m_font->MeasureString(output) / 2.f;
			// Middle Screen POSITION
			if (!scroll)
			{
				m_fontPos.x = float(clientWidth) / 2.f;
				m_fontPos.y = float(clientHeight) - 50;
				scroll = true;
			}
			// Font Outline
			m_font->DrawString(m_spriteBatch.get(), credits,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin, 0.5f);
			m_font->DrawString(m_spriteBatch.get(), credits,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin, 0.5f);
			m_font->DrawString(m_spriteBatch.get(), credits,
				m_fontPos + DirectX::SimpleMath::Vector2(-1.f, -1.f), Colors::Red, 0.f, origin, 0.5f);
			m_font->DrawString(m_spriteBatch.get(), credits,
				m_fontPos + DirectX::SimpleMath::Vector2(1.f, -1.f), Colors::Red, 0.f, origin, 0.5f);
			// Draw the font
			m_font->DrawString(m_spriteBatch.get(), credits, m_fontPos, Colors::Blue, 0.f, origin, 0.5f);
			m_fontPos.y -= 1.0f;
		}
		else
		{
			if (player1)
			{
				// Player One Label Text
				m_fontPos.x = 0.f + m_origin.x / 2.f;
				m_fontPos.y = 0.f + m_origin.y / 8.f;
				output = L"PLAYER 1 : ";
				origin = m_font->MeasureString(output) / 2.f;
				// Draw the P1 Label
				m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Blue, 0.f, origin, 0.3f);
				// Player One Score Text
				m_fontPos.x += 105.f;
				m_fontPos.y = 0.f + m_origin.y / 8.f;
				output = player1ScoreString;			// TEMPORARY NEEDS TO LINK TO INI
				// Draw the score
				m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Blue, 0.f, origin, 0.3f);

				// Draw Life Bars - LINK TO LIVES/HEALTH IN CONFIG?
				//Player 1 - Top Left Location? Blue?
				// P1 LifeBar Position
				m_screenPosP1.x = 0.f + m_origin.x / 4.5f;
				m_screenPosP1.y = 0.f + m_origin.y / 4.f;
				for (int i = 0; i < p1Lives; ++i)
				{
					//					texture			, screenPOS,		,  ColorTine,   , Rotation, origin, scale
					m_spriteBatch->Draw(m_texture.Get(), m_screenPosP1, nullptr, Colors::Blue, 0.f, m_origin, 0.05f);
					m_screenPosP1.x += m_origin.x / 8.5f;
				}
				// Draw Player 1 PowerUp Bar
				m_puPosP1.x = 0.f + 30.f;
				m_puPosP1.y = float(clientHeight) - 20.f;
				//					texture			, screenPOS,		,  ColorTine,   , Rotation, origin, scale
				for (int i = 1; i < 8; i++)
				{
					if (i == p1TokenCount)
						m_spriteBatch->Draw(m_powerupBar.Get(), m_puPosP1, nullptr, Colors::Yellow, 0.f, m_origin, 0.08f);
					else
						m_spriteBatch->Draw(m_powerupBar.Get(), m_puPosP1, nullptr, Colors::White, 0.f, m_origin, 0.08f);

					m_puPosP1.x += 40.f;
				}

				if (p1Speed)
				{
					// Draw Speed Text
					m_puPosP1.x = 0.f + 32.f;
					m_puPosP1.y = float(clientHeight) - 15.f;
					m_spriteBatch->Draw(m_puSpeed.Get(), m_puPosP1, nullptr, Colors::Blue, 0.f, m_origin, 0.07f);
				}
				if (p1Missile)
				{
					// Draw Missle Text
					m_puPosP1.x = 0.f + 32.f * 2.f + 8.f;
					m_puPosP1.y = float(clientHeight) - 15.f;
					m_spriteBatch->Draw(m_puMissle.Get(), m_puPosP1, nullptr, Colors::Blue, 0.f, m_origin, 0.07f);
				}
				if (p1Double)
				{
					// Draw Double Text
					m_puPosP1.x = 0.f + 32.f * 3.f + 15.f;
					m_puPosP1.y = float(clientHeight) - 15.f;
					m_spriteBatch->Draw(m_puDouble.Get(), m_puPosP1, nullptr, Colors::Blue, 0.f, m_origin, 0.07f);
				}
				if (p1Laser)
				{
					// Draw Laser Text
					m_puPosP1.x = 0.f + 32.f * 4.f + 26.f;
					m_puPosP1.y = float(clientHeight) - 15.f;
					m_spriteBatch->Draw(m_puLaser.Get(), m_puPosP1, nullptr, Colors::Blue, 0.f, m_origin, 0.07f);
				}
				if (p1Option)
				{
					// Draw Option Text
					m_puPosP1.x = 0.f + 32.f * 5.f + 32.f;
					m_puPosP1.y = float(clientHeight) - 15.f;
					m_spriteBatch->Draw(m_puOption.Get(), m_puPosP1, nullptr, Colors::Blue, 0.f, m_origin, 0.07f);
				}
				if (p1Shield)
				{
					// Draw Shield Text
					m_puPosP1.x = 0.f + 32.f * 6.f + 42.f;
					m_puPosP1.y = float(clientHeight) - 15.f;
					m_spriteBatch->Draw(m_puShield.Get(), m_puPosP1, nullptr, Colors::Blue, 0.f, m_origin, 0.07f);
				}
				if (p1Beam)
				{
					// Draw Beam Text
					m_puPosP1.x = 0.f + 32.f * 7.f + 54.f;
					m_puPosP1.y = float(clientHeight) - 15.f;
					m_spriteBatch->Draw(m_puBeam.Get(), m_puPosP1, nullptr, Colors::Blue, 0.f, m_origin, 0.07f);
				}
			}

			if (player2)
			{
				// Player Two Label Text
				m_fontPos.x = clientWidth - m_origin.x;
				m_fontPos.y = 0.f + m_origin.y / 8.f;
				output = L"PLAYER 2 : ";
				origin = m_font->MeasureString(output) / 2.f;
				// Draw the P2 Label
				m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Red, 0.f, origin, 0.3f);
				// Player Two Score Text
				m_fontPos.x += 110.f;
				m_fontPos.y = 0.f + m_origin.y / 8.f;
				output = player2ScoreString;			// TEMPORARY NEEDS TO LINK TO INI

				// Draw the score
				m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Red, 0.f, origin, 0.3f);
				// Player 2 - Top Right? Orange?
				// P2 LifeBar Position
				m_screenPosP2.x = clientWidth - 195.f;
				m_screenPosP2.y = 0.f + m_origin.x / 4.f;
				for (int i = 0; i < p2Lives; ++i)
				{
					//					texture			, screenPOS,		,  ColorTine,   , Rotation, origin, scale
					m_spriteBatch->Draw(m_texture.Get(), m_screenPosP2, nullptr, Colors::Red, 0.f, m_origin, 0.05f);
					m_screenPosP2.x += m_origin.x / 8.5f;
				}

				// Draw Player 2 PowerUp Bar
				m_puPosP2.x = float(clientWidth) - 42.f * 7.f;
				m_puPosP2.y = float(clientHeight) - 20.f;
				//					texture			, screenPOS,		,  ColorTine,   , Rotation, origin, scale
				for (int i = 1; i < 8; i++)
				{
					if (i == p2TokenCount)
						m_spriteBatch->Draw(m_powerupBar.Get(), m_puPosP2, nullptr, Colors::Yellow, 0.f, m_origin, 0.08f);
					else if (i != p2TokenCount)
						m_spriteBatch->Draw(m_powerupBar.Get(), m_puPosP2, nullptr, Colors::White, 0.f, m_origin, 0.08f);

					m_puPosP2.x += 40.f;
				}
				if (p2Speed)
				{
					// Draw Speed Text
					m_puPosP2.x = float(clientWidth) - 42.f * 7.f + 2.f;
					m_puPosP2.y = float(clientHeight) - 15.f;
					m_spriteBatch->Draw(m_puSpeed.Get(), m_puPosP2, nullptr, Colors::Red, 0.f, m_origin, 0.07f);
				}
				if (p2Missile)
				{
					// Draw Missle Text
					m_puPosP2.x = float(clientWidth) - 42.f * 6.f;
					m_puPosP2.y = float(clientHeight) - 15.f;
					m_spriteBatch->Draw(m_puMissle.Get(), m_puPosP2, nullptr, Colors::Red, 0.f, m_origin, 0.07f);
				}
				if (p2Double)
				{
					// Draw Double Text
					m_puPosP2.x = float(clientWidth) - 42.f * 5.f - 3.f;
					m_puPosP2.y = float(clientHeight) - 15.f;
					m_spriteBatch->Draw(m_puDouble.Get(), m_puPosP2, nullptr, Colors::Red, 0.f, m_origin, 0.07f);
				}
				if (p2Laser)
				{
					// Draw Laser Text
					m_puPosP2.x = float(clientWidth) - 42.f * 4.f - 1.f;
					m_puPosP2.y = float(clientHeight) - 15.f;
					m_spriteBatch->Draw(m_puLaser.Get(), m_puPosP2, nullptr, Colors::Red, 0.f, m_origin, 0.07f);
				}
				if (p2Option)
				{
					// Draw Option Text
					m_puPosP2.x = float(clientWidth) - 42.f * 3.f - 6.f;
					m_puPosP2.y = float(clientHeight) - 15.f;
					m_spriteBatch->Draw(m_puOption.Get(), m_puPosP2, nullptr, Colors::Red, 0.f, m_origin, 0.07f);
				}
				if (p2Shield)
				{
					// Draw Shield Text
					m_puPosP2.x = float(clientWidth) - 42.f * 2.f - 6.f;
					m_puPosP2.y = float(clientHeight) - 15.f;
					m_spriteBatch->Draw(m_puShield.Get(), m_puPosP2, nullptr, Colors::Red, 0.f, m_origin, 0.07f);
				}
				if (p2Beam)
				{
					// Draw Beam Text
					m_puPosP2.x = float(clientWidth) - 42.f - 3.f;
					m_puPosP2.y = float(clientHeight) - 15.f;
					m_spriteBatch->Draw(m_puBeam.Get(), m_puPosP2, nullptr, Colors::Red, 0.f, m_origin, 0.07f);
				}
			}
			else
			{
				// Player Two Start Text
				m_fontPos.x = clientWidth - m_origin.x;
				m_fontPos.y = 0.f + m_origin.y / 8.f;
				output = L"Press Start To Play";
				origin = m_font->MeasureString(output) / 2.f;
				// Draw the P2 Label
				m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Red, 0.f, origin, 0.3f);
			}

			if (gameover)
			{
				// FONT
				output = L"Game Over";
				origin = m_font->MeasureString(output) / 2.f;
				// Middle Screen POSITION
				m_fontPos.x = float(clientWidth) / 2.f;
				m_fontPos.y = float(clientHeight) / 2.f;
				// Font Drop Shadow
				/*m_font->DrawString(m_spriteBatch.get(), output,
					m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin);
				m_font->DrawString(m_spriteBatch.get(), output,
					m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin);*/
					// Font Outline
				m_font->DrawString(m_spriteBatch.get(), output,
					m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin);
				m_font->DrawString(m_spriteBatch.get(), output,
					m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin);
				m_font->DrawString(m_spriteBatch.get(), output,
					m_fontPos + DirectX::SimpleMath::Vector2(-1.f, -1.f), Colors::Red, 0.f, origin);
				m_font->DrawString(m_spriteBatch.get(), output,
					m_fontPos + DirectX::SimpleMath::Vector2(1.f, -1.f), Colors::Red, 0.f, origin);
				// Draw the font
				m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Blue, 0.f, origin);

				// Player One HiScore Label Text
				m_fontPos.x = float(clientWidth) / 2.f - 40.f;
				m_fontPos.y = float(clientHeight) / 2.f + 100.f;
				output = L"Player 1 High Score : ";
				origin = m_font->MeasureString(output) / 2.f;
				// Draw the P1 Label
				m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Blue, 0.f, origin, 0.3f);
				// Player One HiScore Text
				m_fontPos.x = float(clientWidth) / 2.f + 185.f;
				m_fontPos.y = float(clientHeight) / 2.f + 100.f;
				int score1 = _wtoi(player1ScoreString);
				if (score1 > highscorePlayer1)
				{
					output = player1ScoreString;
					highscorePlayer1 = score1;
				}
				else
				{
					const wchar_t* highscore1 = widen(std::to_string(highscorePlayer1));
					output = highscore1;			// TEMPORARY NEEDS TO LINK TO INI
				}
				// Draw the score
				m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Blue, 0.f, origin, 0.3f);

				// Player Two HiScore Label Text
				m_fontPos.x = float(clientWidth) / 2.f - 38.f;
				m_fontPos.y = float(clientHeight) / 2.f + 120.f;
				output = L"Player 2 High Score : ";
				origin = m_font->MeasureString(output) / 2.f;
				// Draw the P1 Label
				m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Red, 0.f, origin, 0.3f);
				// Player One HiScore Text
				m_fontPos.x = float(clientWidth) / 2.f + 188.f;
				m_fontPos.y = float(clientHeight) / 2.f + 120.f;
				int score2 = _wtoi(player2ScoreString);
				if (score2 > highscorePlayer2)
				{
					output = player2ScoreString;
					highscorePlayer2 = score2;
				}
				else
				{
					const wchar_t* highscore2 = widen(std::to_string(highscorePlayer2));
					output = highscore2;			// TEMPORARY NEEDS TO LINK TO INI
				}
				// Draw the score
				m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Red, 0.f, origin, 0.3f);
			}
			if (gameWin)
			{
				// FONT
				output = L"YOU WIN!!!";
				origin = m_font->MeasureString(output) / 2.f;
				// Middle Screen POSITION
				m_fontPos.x = float(clientWidth) / 2.f;
				m_fontPos.y = float(clientHeight) / 2.f;
				// Font Drop Shadow
				/*m_font->DrawString(m_spriteBatch.get(), output,
					m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin);
				m_font->DrawString(m_spriteBatch.get(), output,
					m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin);*/
					// Font Outline
				m_font->DrawString(m_spriteBatch.get(), output,
					m_fontPos + DirectX::SimpleMath::Vector2(1.f, 1.f), Colors::Red, 0.f, origin);
				m_font->DrawString(m_spriteBatch.get(), output,
					m_fontPos + DirectX::SimpleMath::Vector2(-1.f, 1.f), Colors::Red, 0.f, origin);
				m_font->DrawString(m_spriteBatch.get(), output,
					m_fontPos + DirectX::SimpleMath::Vector2(-1.f, -1.f), Colors::Red, 0.f, origin);
				m_font->DrawString(m_spriteBatch.get(), output,
					m_fontPos + DirectX::SimpleMath::Vector2(1.f, -1.f), Colors::Red, 0.f, origin);
				// Draw the font
				m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Blue, 0.f, origin);

				// Player One HiScore Label Text
				m_fontPos.x = float(clientWidth) / 2.f - 40.f;
				m_fontPos.y = float(clientHeight) / 2.f + 100.f;
				output = L"Player 1 High Score : ";
				origin = m_font->MeasureString(output) / 2.f;
				// Draw the P1 Label
				m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Blue, 0.f, origin, 0.3f);
				// Player One HiScore Text
				m_fontPos.x = float(clientWidth) / 2.f + 185.f;
				m_fontPos.y = float(clientHeight) / 2.f + 100.f;
				int score1 = _wtoi(player1ScoreString);
				if (score1 > highscorePlayer1)
				{
					output = player1ScoreString;
					highscorePlayer1 = score1;
				}
				else
				{
					const wchar_t* highscore1 = widen(std::to_string(highscorePlayer1));
					output = highscore1;			// TEMPORARY NEEDS TO LINK TO INI
				}
				// Draw the score
				m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Blue, 0.f, origin, 0.3f);

				// Player Two HiScore Label Text
				m_fontPos.x = float(clientWidth) / 2.f - 38.f;
				m_fontPos.y = float(clientHeight) / 2.f + 120.f;
				output = L"Player 2 High Score : ";
				origin = m_font->MeasureString(output) / 2.f;
				// Draw the P1 Label
				m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Red, 0.f, origin, 0.3f);
				// Player One HiScore Text
				m_fontPos.x = float(clientWidth) / 2.f + 188.f;
				m_fontPos.y = float(clientHeight) / 2.f + 120.f;
				int score2 = _wtoi(player2ScoreString);
				if (score2 > highscorePlayer2)
				{
					output = player2ScoreString;
					highscorePlayer2 = score2;
				}
				else
				{
					const wchar_t* highscore2 = widen(std::to_string(highscorePlayer2));
					output = highscore2;			// TEMPORARY NEEDS TO LINK TO INI
				}
				// Draw the score
				m_font->DrawString(m_spriteBatch.get(), output, m_fontPos, Colors::Red, 0.f, origin, 0.3f);
			}
		}
		// End Batch Draw
		m_spriteBatch->End();
	}
	bool setupDrawCall()
	{
		struct RenderingSystem {};
		auto rs = game->entity("Rendering System").add<RenderingSystem>();

		startDraw = game->system<RenderingSystem>().kind(flecs::PreUpdate)
			.each([this](flecs::entity e, RenderingSystem& s) {
			// reset the draw counter only once per frame
			draw_counter = 0;
				});

		updateDraw = game->system<ESG::ModelTransform, ESG::Mesh, ESG::Instance, ESG::Model, ESG::Renderable>().kind(flecs::OnUpdate)
			.each([&](flecs::entity e, ESG::ModelTransform& p, ESG::Mesh& m, ESG::Instance& i, ESG::Model& model, ESG::Renderable&)
				{
					int dc = draw_counter;
					//std::cout <<"Draw Counter"<< dc << std::endl;
					//std::cout << "Renderable" << game->count<ESG::Renderable>() << std::endl;

					//for debugging
					//int entityCount = game->count<ESG::Renderable>();
					//bool entityRender = e.has<ESG::Renderable>();
					//bool esgld = e.has<ESG::lvlData>();
					//bool ld = e.has<Level_Data>();

					//auto lvlDataWorld = game->lookup("LevelData");

					if (e.has<ESG::Renderable>() && e.get<ESG::Renderable>()->value) {
						/*if (e.get_mut<ESG::BlenderName>()->name == "Spaceship_RaeTheRedPanda")
							std::cout << "rendering" << std::endl;*/
						instanceDataObject.Bullets[draw_counter] = p.matrix;
						instanceDataObject.vertexCount[draw_counter] = model.vertexCount;
						instanceDataObject.indexCount[draw_counter] = m.indexCount;
						instanceDataObject.indexStart[draw_counter] = model.indexStart;
						instanceDataObject.indexOffset[draw_counter] = m.indexOffset;
						instanceDataObject.vertexStart[draw_counter] = model.vertexStart;
						instanceDataObject.materialStart[draw_counter] = model.materialStart;
						instanceDataObject.materialIndex[draw_counter] = m.materialIndex;
						instanceDataObject.modelIndex[draw_counter] = i.modelIndex;
						instanceDataObject.meshStart[draw_counter] = model.meshStart;
						instanceDataObject.meshCount[draw_counter] = model.meshCount;
						instanceDataObject.useTextures[draw_counter] = model.useTextures;
						int v = static_cast<int>(240) - static_cast<int>(draw_counter + 2);
						int sign = 1 ^ ((unsigned int)v >> (sizeof(int) * CHAR_BIT - 1));

						draw_counter += sign;
					}
					//std::cout << "draw counter is : " << draw_counter << std::endl;
				});

		completeDraw = game->system<RenderingSystem>().kind(flecs::OnStore)
			.each([this](flecs::entity e, RenderingSystem&)
				{
					PipelineHandles curHandles = GetCurrentPipelineHandles();
					SetUpPipeline(curHandles);
					auto count = game->count<ESG::Renderable>();
					UpdateMatrixBuffer(curHandles);
					camera.UpdateViewMatrix();
					cb_Object.cbViewMatrix = camera.GetViewMatrix();
					cb_Object.cbProjectionMatrix = camera.GetPerspectiveMatrix();

					static float temp = 0.0001f;
					cb_Frame.time = temp;

					RotateLights();

					frameBuffer.Update(curHandles.context, cb_Frame);

					objectBuffer.BindVS(curHandles.context, 0);
					objectBuffer.BindPS(curHandles.context, 0);
					frameBuffer.BindPS(curHandles.context, 1);
					sceneBuffer.BindPS(curHandles.context, 2);


					InitializeScene(*lvl, curHandles);

					if (splitscreen)
					{
						//Left viewport
						D3D11_VIEWPORT left_vp;
						left_vp.Width = windowWidth / 2;
						left_vp.Height = windowHeight / 2;
						left_vp.MinDepth = 0;
						left_vp.MaxDepth = 1;
						left_vp.TopLeftX = 0;
						left_vp.TopLeftY = 0;
						curHandles.context->RSSetViewports(1u, &left_vp);

						for (size_t i = 0; i < draw_counter; i++)
						{


							cb_Object.materialIndex.z = i;
							cb_Object.materialIndex.x = instanceDataObject.materialStart[i] + instanceDataObject.materialIndex[i];
							cb_Object.useTexture = instanceDataObject.useTextures[i];
							objectBuffer.Update(curHandles.context, cb_Object);

							curHandles.context->PSSetShaderResources(0, 4, loadedTextures[instanceDataObject.materialIndex[i]].smrv);

							curHandles.context->DrawIndexed(instanceDataObject.indexCount[i],
								instanceDataObject.indexStart[i] + instanceDataObject.indexOffset[i], instanceDataObject.vertexStart[i]);

						}

						//You can adjust these numbers as you like
	//Left viewport
						D3D11_VIEWPORT right_vp;
						right_vp.Width = windowWidth / 2;
						right_vp.Height = windowHeight / 2;
						right_vp.MinDepth = 0;
						right_vp.MaxDepth = 1;
						right_vp.TopLeftX = windowWidth / 2;
						right_vp.TopLeftY = 0;
						curHandles.context->RSSetViewports(1u, &right_vp);

						for (size_t i = 0; i < draw_counter; i++)
						{


							cb_Object.materialIndex.z = i;
							cb_Object.materialIndex.x = instanceDataObject.materialStart[i] + instanceDataObject.materialIndex[i];
							cb_Object.useTexture = instanceDataObject.useTextures[i];
							objectBuffer.Update(curHandles.context, cb_Object);

							curHandles.context->PSSetShaderResources(0, 4, loadedTextures[instanceDataObject.materialIndex[i]].smrv);

							curHandles.context->DrawIndexed(instanceDataObject.indexCount[i],
								instanceDataObject.indexStart[i] + instanceDataObject.indexOffset[i], instanceDataObject.vertexStart[i]);

						}

						D3D11_VIEWPORT full_vp;
						full_vp.Width = windowWidth;
						full_vp.Height = windowHeight;
						full_vp.MinDepth = 0;
						full_vp.MaxDepth = 1;
						full_vp.TopLeftX = 0;
						full_vp.TopLeftY = 0;
						curHandles.context->RSSetViewports(1u, &full_vp);
					}
					else
					{
						for (size_t i = 0; i < draw_counter; i++)
						{


							cb_Object.materialIndex.z = i;
							cb_Object.materialIndex.x = instanceDataObject.materialStart[i] + instanceDataObject.materialIndex[i];
							cb_Object.useTexture = instanceDataObject.useTextures[i];
							objectBuffer.Update(curHandles.context, cb_Object);

							curHandles.context->PSSetShaderResources(0, 4, loadedTextures[instanceDataObject.materialIndex[i]].smrv);

							curHandles.context->DrawIndexed(instanceDataObject.indexCount[i],
								instanceDataObject.indexStart[i] + instanceDataObject.indexOffset[i], instanceDataObject.vertexStart[i]);

						}
					}

					ReleasePipelineHandles(curHandles);
				});
#if 1

		renderDistance = game->system<ESG::ModelTransform>().kind(flecs::PostUpdate)
			.iter([this](flecs::iter it, ESG::ModelTransform* m)
				{
					auto player = game->lookup("Spaceship_RaeTheRedPanda");
					auto playerPos = player.get_mut<ESG::ModelTransform>();
					for (auto i = 0; i < it.count(); i++)
					{
						if (!it.entity(i).has<ESG::Player>()) {
							if (ApplyRenderDistance(playerPos->matrix, m[i].matrix) < 25.0f)
							{
								it.entity(i).set<ESG::Renderable>({ true });
								it.entity(i).add<ESG::Collidable>();
							}
							else
							{
								it.entity(i).set<ESG::Renderable>({ false });
								it.entity(i).remove<ESG::Collidable>();
							}
						}

					}
				});
#endif
#if 1

		updateBullet = game->system<ESG::Bullet>().kind(flecs::PostUpdate)
			.iter([](flecs::iter it, ESG::Bullet*)
				{
					for (auto i = 0; i < it.count(); i++)
					{

						it.entity(i).add<ESG::Renderable>();

					}
				});
#endif

#if 1

		updateLaser = game->system<ESG::Lasers>().kind(flecs::PostUpdate)
			.iter([](flecs::iter it)
				{
					for (auto i = 0; i < it.count(); i++)
					{


						it.entity(i).add<ESG::Renderable>();

					}
				});
#endif
#if 1

		updateMissile = game->system<ESG::Missiles>().kind(flecs::PostUpdate)
			.iter([](flecs::iter it)
				{
					for (auto i = 0; i < it.count(); i++)
					{
						it.entity(i).add<ESG::Renderable>();

					}
				});
#endif
#if 1
		updateDouble = game->system<ESG::Double>().kind(flecs::PostUpdate)
			.iter([](flecs::iter it, ESG::Double*)
				{
					for (auto i = 0; i < it.count(); i++)
					{
						it.entity(i).add<ESG::Renderable>();

					}
				});
#endif
#if 1
		updateBeam = game->system<ESG::Beams>().kind(flecs::PostUpdate)
			.iter([](flecs::iter it, ESG::Beams*)
				{
					for (auto i = 0; i < it.count(); i++)
					{
						it.entity(i).add<ESG::Renderable>();

					}
				});
#endif
#if 1
		updateTokens = game->system<ESG::token>().kind(flecs::PostUpdate)
			.iter([](flecs::iter it, ESG::token*)
				{
					for (auto i = 0; i < it.count(); i++)
					{
						it.entity(i).add<ESG::Renderable>();

					}
				});
#endif
#if 1
		PlayerUI = game->system<ESG::Player, ESG::Renderable, ESG::ControllerID>().kind(flecs::PostUpdate)
			.iter([&](flecs::iter it, ESG::Player*, ESG::Renderable* r, ESG::ControllerID* c)
				{
					for (auto& i : it)
					{
						if (c[i].index == 0 && r[i].value == true)
						{
							player1 = true;
						}
						if (c[i].index == 1 && r[i].value == true)
						{
							player2 = true;

						}
					}

				});
#endif
#if 1
		PowerMeterUI = game->system< ESG::ControllerID, ESG::PowerMeterTokens>().kind(flecs::OnUpdate)
			.iter([&](flecs::iter it, ESG::ControllerID* c, ESG::PowerMeterTokens* t)
				{

					//std::this_thread::sleep_for(std::chrono::milliseconds(100));
					for (auto i : it)
					{
						if (c[i].index == 0)
						{
							p1TokenCount = t[i].value;
							/*p1TokenCount += t[i].value;
							if (p1TokenCount >7)
							{
								p1TokenCount = 0;
							}*/

						}
						if (c[i].index == 1)
						{
							p2TokenCount = t[i].value;
							/*p2TokenCount += t[i].value;
							if (p2TokenCount > 7)
							{
								p2TokenCount = 0;
							}*/

						}
					}

				});
#endif
#if 1
		updateScore = game->system< ESG::ControllerID, ESG::playerScore>().kind(flecs::PreUpdate)
			.iter([&](flecs::iter it, ESG::ControllerID* c, ESG::playerScore* s)
				{

					//std::this_thread::sleep_for(std::chrono::milliseconds(100));
					for (auto i : it)
					{
						if (c[i].index == 0)
						{
							p1Score = s[i].score;
							player1ScoreString = widen(std::to_string(s[i].score));
							/*p1TokenCount += t[i].value;
							if (p1TokenCount >7)
							{
								p1TokenCount = 0;
							}*/

						}
						if (c[i].index == 1)
						{
							p2Score = s[i].score;
							player2ScoreString = widen(std::to_string(s[i].score));
							/*p2TokenCount += t[i].value;
							if (p2TokenCount > 7)
							{
								p2TokenCount = 0;
							}*/

						}
					}

				});
#endif
#if 1
		updateLives = game->system< ESG::ControllerID, ESG::Lives>().kind(flecs::PreUpdate)
			.iter([&](flecs::iter it, ESG::ControllerID* c, ESG::Lives* l)
				{

					//std::this_thread::sleep_for(std::chrono::milliseconds(100));
					for (auto i : it)
					{
						if (c[i].index == 0)
						{

							p1Lives = l[i].value;




						}
						if (c[i].index == 1)
						{

							p2Lives = l[i].value;



						}
					}

				});
#endif
#if 1
		speedUI = game->system< ESG::ControllerID, ESG::Speed>().kind(flecs::PreUpdate)
			.iter([&](flecs::iter it, ESG::ControllerID* c, ESG::Speed* s)
				{

					//std::this_thread::sleep_for(std::chrono::milliseconds(100));
					for (auto i : it)
					{
						if (c[i].index == 0 && s[i].counter > 2)
						{
							p1Speed = false;

						}
						if (c[i].index == 1 && s[i].counter > 2)
						{

							p2Speed = false;
						}
					}

				});
#endif
#if 1
		missileUI = game->system< ESG::ControllerID, ESG::Missiles>().kind(flecs::PreUpdate)
			.iter([&](flecs::iter it, ESG::ControllerID* c, ESG::Missiles* m)
				{

					//std::this_thread::sleep_for(std::chrono::milliseconds(100));
					for (auto i : it)
					{
						if (c[i].index == 0)
						{
							p1Missile = false;

						}
						if (c[i].index == 1)
						{

							p2Missile = false;
						}
					}

				});
#endif
#if 1
		laserUI = game->system< ESG::ControllerID, ESG::Lasers>().kind(flecs::PreUpdate)
			.iter([&](flecs::iter it, ESG::ControllerID* c, ESG::Lasers* l)
				{

					//std::this_thread::sleep_for(std::chrono::milliseconds(100));
					for (auto i : it)
					{
						if (c[i].index == 0)
						{
							p1Laser = false;

						}
						if (c[i].index == 1)
						{

							p2Laser = false;
						}
					}

				});
#endif
#if 1
		doubleUI = game->system< ESG::ControllerID, ESG::Double>().kind(flecs::PreUpdate)
			.iter([&](flecs::iter it, ESG::ControllerID* c, ESG::Double* d)
				{

					//std::this_thread::sleep_for(std::chrono::milliseconds(100));
					for (auto i : it)
					{
						if (c[i].index == 0)
						{
							p1Double = false;

						}
						if (c[i].index == 1)
						{

							p2Double = false;
						}
					}

				});
#endif
#if 1
		optionUI = game->system< ESG::ControllerID, ESG::Option>().kind(flecs::PreUpdate)
			.iter([&](flecs::iter it, ESG::ControllerID* c, ESG::Option* o)
				{

					//std::this_thread::sleep_for(std::chrono::milliseconds(100));
					for (auto i : it)
					{
						if (c[i].index == 0)
						{
							p1Option = false;

						}
						if (c[i].index == 1)
						{

							p2Option = false;
						}
					}

				});
#endif
#if 1
		shieldUI = game->system< ESG::ControllerID, ESG::Shield>().kind(flecs::PreUpdate)
			.iter([&](flecs::iter it, ESG::ControllerID* c, ESG::Shield* s)
				{

					//std::this_thread::sleep_for(std::chrono::milliseconds(100));
					for (auto i : it)
					{
						if (c[i].index == 0)
						{
							p1Shield = false;

						}
						if (c[i].index == 1)
						{

							p2Shield = false;
						}
					}

				});
#endif
#if 1
		beamUI = game->system< ESG::ControllerID, ESG::Beams>().kind(flecs::PreUpdate)
			.iter([&](flecs::iter it, ESG::ControllerID* c, ESG::Beams* b)
				{

					//std::this_thread::sleep_for(std::chrono::milliseconds(100));
					for (auto i : it)
					{
						if (c[i].index == 0)
						{
							p1Beam = false;

						}
						if (c[i].index == 1)
						{

							p2Beam = false;
						}
					}

				});
#endif
		return true;
	}

private:
	struct PipelineHandles
	{
		ID3D11DeviceContext* context;
		ID3D11RenderTargetView* targetView;
		ID3D11DepthStencilView* depthStencil;
	};
	//Render helper functions
	PipelineHandles GetCurrentPipelineHandles()
	{
		PipelineHandles retval;
		d3d.GetImmediateContext((void**)&retval.context);
		d3d.GetRenderTargetView((void**)&retval.targetView);
		d3d.GetDepthStencilView((void**)&retval.depthStencil);
		return retval;
	}

	void SetUpPipeline(PipelineHandles handles)
	{
		SetRenderTargets(handles);
		SetShaders(handles);
		SetRasterizerState(handles, rsDefault.Get());

		handles.context->IASetInputLayout(vertexFormat.Get());
		handles.context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void SetRasterizerState(PipelineHandles handles, ID3D11RasterizerState* newState)
	{
		handles.context->RSSetState(newState);
	}

	void SetRenderTargets(PipelineHandles handles)
	{
		ID3D11RenderTargetView* const views[] = { handles.targetView };
		handles.context->OMSetRenderTargets(ARRAYSIZE(views), views, handles.depthStencil);
	}

	void SetShaders(PipelineHandles handles)
	{
		handles.context->VSSetShader(vertexShader.Get(), nullptr, 0);
		handles.context->PSSetShader(pixelShader.Get(), nullptr, 0);
	}

	void ReleasePipelineHandles(PipelineHandles toRelease)
	{
		toRelease.depthStencil->Release();
		toRelease.targetView->Release();
		toRelease.context->Release();
	}

	void LevelChangeBufferUpdate()
	{
		ID3D11Device* creator = nullptr;
		d3d.GetDevice((void**)&creator);
		PipelineHandles curHandles = GetCurrentPipelineHandles();

		InitializeScene(*lvl, curHandles);

		creator->Release();
	}

	void RotateLights()
	{
		for (int i = 0; i < lvl->levelSpotLights.size(); ++i)
		{
			if (i == 0)
				continue;
			if (cb_Frame.slights[i].watts >= 1000)
				cb_Frame.slights[i].watts = 0;
			else if (cb_Frame.slights[i].watts <= 0)
				cb_Frame.slights[i].watts = 1000;

			if (i % 2)
				cb_Frame.slights[i].watts -= 50;
			else if (i == 3)
				cb_Frame.slights[i].watts += 50;
			else
				cb_Frame.slights[i].watts += 10;

			cb_Frame.slights[i].watts += 1;


			if (i == 1 || i == 4)
			{
				GW::MATH::GMatrix::RotateXGlobalF(cb_Frame.slights[i].transform, XMConvertToRadians(1), cb_Frame.slights[i].transform);
				GW::MATH::GMatrix::RotateYGlobalF(cb_Frame.slights[i].transform, XMConvertToRadians(1), cb_Frame.slights[i].transform);
			}
			else if (i == 3)
			{
				GW::MATH::GMatrix::RotateXGlobalF(cb_Frame.slights[i].transform, XMConvertToRadians(-1), cb_Frame.slights[i].transform);
				GW::MATH::GMatrix::RotateYGlobalF(cb_Frame.slights[i].transform, XMConvertToRadians(-1), cb_Frame.slights[i].transform);
			}

		}
	}

	void InitializeScene(Level_Data& lvl, PipelineHandles curHandles)
	{

		ID3D11Device* creator = nullptr;
		d3d.GetDevice((void**)&creator);

		//// Initialize Scene Buffers
		//vertexBuffer.Initialize(creator, lvl.levelVertices.data(), sizeof(H2B::VERTEX) * lvl.levelVertices.size());
		//indexBuffer.Initialize(creator, lvl.levelIndices.data(), sizeof(UINT) * lvl.levelIndices.size());
		////instanceBuffer.Initialize(creator, lvl->levelTransforms.data(), sizeof(XMFLOAT4X4) * lvl->levelTransforms.size());

		//InitializePipeline(creator);
		//PipelineHandles currHandles = GetCurrentPipelineHandles();

		// PerObject Data
		cb_Object.cbViewMatrix = camera.GetViewMatrix();
		cb_Object.cbProjectionMatrix = camera.GetPerspectiveMatrix();
		XMStoreFloat4(&cb_Object.materialIndex, XMVectorReplicate(0.0f));

		// PerFrame Data
		XMStoreFloat4(&cb_Frame.sunLightColor, XMLoadFloat4(&sunLightColor));
		XMStoreFloat3(&cb_Frame.sunLightDir, XMVector3Normalize(XMLoadFloat3(&sunLightDirection)));

		// Light Data
		for (size_t i = 0; i < lvl.levelPointLights.size(); i++)
		{
			cb_Frame.plights[i] = lvl.levelPointLights[i];
		}
		for (size_t i = 0; i < lvl.levelSpotLights.size(); i++)
		{
			cb_Frame.slights[i] = lvl.levelSpotLights[i];
		}
		cb_Scene.pointlights = lvl.levelPointLights.size();
		cb_Scene.spotlights = lvl.levelSpotLights.size();

		// PerScene Data
		for (size_t i = 0; i < lvl.Materials.size(); i++)
		{
			cb_Scene.attributes[i] = lvl.Materials[i].attrib;
		}
		cb_Scene.ambient = ambient;

		// Bind buffers
		vertexBuffer.Bind(curHandles.context, sizeof(H2B::VERTEX), 0);
		indexBuffer.Bind(curHandles.context, DXGI_FORMAT_R32_UINT, 0);
		//instanceBuffer.Bind(curHandles.context, sizeof(InstanceData), 0);

		// Create constant buffers
		/*objectBuffer.Initialize(creator);
		frameBuffer.Initialize(creator);
		sceneBuffer.Initialize(creator);
		InitializeStructuredBuffer(creator, curHandles.context);*/

		// Update Scene Buffer
		sceneBuffer.Update(curHandles.context, cb_Scene);

		// Create Texture from file - models
		//HRESULT hr = E_NOTIMPL;
		//for (size_t i = 0; i < lvl.Materials.size(); i++)
		//{
		//	std::string folder = "../Textures/";
		//	//std::wstring folder = L"../Textures/";
		//	/*const wchar_t* map_Kd = (folder + std::wstring(lvl.Materials[i].map_Kd.begin(), lvl.Materials[i].map_Kd.end())).c_str();
		//	const wchar_t* map_Ks = (folder + std::wstring(lvl.Materials[i].map_Ks.begin(), lvl.Materials[i].map_Ks.end())).c_str();
		//	const wchar_t* map_Ka = (folder + std::wstring(lvl.Materials[i].map_Ka.begin(), lvl.Materials[i].map_Ka.end())).c_str();
		//	const wchar_t* map_Ns = (folder + std::wstring(lvl.Materials[i].map_Ns.begin(), lvl.Materials[i].map_Ns.end())).c_str();*/

		//	std::string filePaths[4] = { folder + lvl.Materials[i].map_Kd, folder + lvl.Materials[i].map_Ks, folder + lvl.Materials[i].map_Ka, folder + lvl.Materials[i].map_Ns };
		//	loadedTextures.push_back(ESG::TextureArray(creator, filePaths));
		//}
		////std::wstring texturePath;
		////texturePath = L"../Textures/Space.dds";
		////hr = CreateDDSTextureFromFile(creator, texturePath.c_str(), nullptr, shaderResourceView.GetAddressOf());
		//// Create sampler for model textures
		//CD3D11_SAMPLER_DESC samp_desc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
		//creator->CreateSamplerState(&samp_desc, samplerState.GetAddressOf());

		// Bind texture resource and sampler
		//curHandles.context->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
		curHandles.context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
		curHandles.context->VSSetShaderResources(2, 1, matrixBufferSRV.GetAddressOf());

		// free temporary handle
		creator->Release();
	}

public:
	~Renderer()
	{
		// ComPtr will auto release so nothing to do here yet
		auto score = game->entity<ESG::Highscore>("Highscore").set_override<ESG::Highscore>({highscorePlayer1, highscorePlayer2});
		gameConfig->UpdateScore(highscorePlayer1, highscorePlayer2);
	}
	void Update()
	{
		PipelineHandles currHandles = GetCurrentPipelineHandles();
		UpdateMatrixBuffer(currHandles);
	}

	void UpdateMatrixBuffer(PipelineHandles handles)
	{
		unsigned int instanceSize = sizeof(XMFLOAT4X4) * MAX_INSTANCES;
		unsigned int matSize = instanceSize;//lvlSize +instanceSize + MissileSize;

		// Map the staging buffer, write new data, then unmap
		D3D11_MAPPED_SUBRESOURCE mappedData;
		HRESULT hr = handles.context->Map(stagingBuffer.Get(), 0, D3D11_MAP_WRITE, 0, &mappedData);
		if (SUCCEEDED(hr))
		{
			memcpy(mappedData.pData, &instanceDataObject.Bullets, matSize);

			// Copy the instance data
			handles.context->Unmap(stagingBuffer.Get(), 0);
		}
		else { return; }

		// Copy data from the staging buffer to the default buffer
		handles.context->CopyResource(matrixBuffer.Get(), stagingBuffer.Get());
	}
};