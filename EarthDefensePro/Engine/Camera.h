#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
private:
	float aspectRatio;
	float fov{ XMConvertToRadians(65.0f) };
	float nearPlane{ 0.1f};
	float farPlane{ 500 };

	float moveSpeed{ 0.001f }; // Default move speed
	
	float mouseSensitivityX = 1.0f;
	float mouseSensitivityY = 1.0f;

	XMFLOAT3 camPosition;
	XMFLOAT3 camRight;
	XMFLOAT3 camUp;
	XMFLOAT3 camForward;

	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

public:
	Camera(float _aspectRatio);
	~Camera();

	float GetMoveSpeed();
	void SetMoveSpeed(float speed);
	float GetMouseXSensitivity();
	float GetMouseYSensitivity();

	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetPerspectiveMatrix();

	// Set Aspect Ratio
	void SetAspectRatio(float _ratio);
	// Set view matrix
	void UpdateViewMatrix();
	// Set Persepective Matrix which is the actual view
	void SetView();
	void Translate(XMFLOAT3 translation, float deltaTime);
	void Pitch(float _angle, float sensitivity);
	void Yaw(float dx, float sensitivity);
};