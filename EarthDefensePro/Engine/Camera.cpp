#include "Camera.h"
#include <cmath>
#include <algorithm>

Camera::Camera(float _aspectRatio)
{

	XMStoreFloat3(&camPosition, { 0.0f, 0.0f, -10.0f });
	XMStoreFloat3(&camRight, { 1.0f, 0.0f, 0.0f });
	XMStoreFloat3(&camUp, { 0.0f, 1.0f, 0.0f });
	XMStoreFloat3(&camForward, { 0.0f, 0.0f, 1.0f });
	XMStoreFloat4x4(&viewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&projectionMatrix, XMMatrixIdentity());

	SetAspectRatio(_aspectRatio);
	SetView();
}

Camera::~Camera()
{
}

float Camera::GetMoveSpeed()
{
	return moveSpeed;
}

void Camera::SetMoveSpeed(float speed)
{
	moveSpeed = speed;
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetPerspectiveMatrix()
{
	return projectionMatrix;
}

void Camera::SetAspectRatio(float _ratio)
{
	aspectRatio = _ratio;
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR right = XMLoadFloat3(&camRight);
	XMVECTOR up = XMLoadFloat3(&camUp);
	XMVECTOR forward = XMLoadFloat3(&camForward);
	XMVECTOR pos = XMLoadFloat3(&camPosition);

	// Normalize look direction
	forward = XMVector3Normalize(forward);

	// Get up
	up = XMVector3Normalize(XMVector3Cross(forward, right));

	// Get right
	right = XMVector3Cross(up, forward);

	float x = -XMVectorGetX(XMVector3Dot(pos, right));
	float y = -XMVectorGetX(XMVector3Dot(pos, up));
	float z = -XMVectorGetX(XMVector3Dot(pos, forward));

	// The new view matrix
	XMFLOAT3 rightVector(camRight.x, camRight.y, camRight.z);
	XMFLOAT3 upVector(camUp.x, camUp.y, camUp.z);
	XMFLOAT3 forwardVector(camForward.x, camForward.y, camForward.z);
	XMFLOAT3 position(x, y, z);

	XMMATRIX temp = XMMatrixSet(
		rightVector.x, upVector.x, forwardVector.x, 0.0f,
		rightVector.y, upVector.y, forwardVector.y, 0.0f,
		rightVector.z, upVector.z, forwardVector.z, 0.0f,
		position.x, position.y, position.z, 1.0f
	);

	// Store the view matrix
	XMStoreFloat4x4(&viewMatrix, temp);
}

void Camera::SetView()
{
	XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane));
}

void Camera::Translate(XMFLOAT3 translation, float deltaTime)
{
	// Camera current position
	XMVECTOR right = XMLoadFloat3(&camRight);
	XMVECTOR up = XMLoadFloat3(&camUp);
	XMVECTOR forward = XMLoadFloat3(&camForward);

	// Calculate movement
	XMVECTOR displacement = (right * translation.x + up * translation.y + forward * translation.z) * deltaTime * moveSpeed;;

	// Update camera position
	XMVECTOR pos = XMLoadFloat3(&camPosition);
	pos = XMVectorAdd(pos, displacement);
	XMStoreFloat3(&camPosition, pos);

	// Update view matrix
	UpdateViewMatrix();
}

void Camera::Pitch(float dy, float sensitivity)
{
	// TODO Find a way to clamp this so it does not flip upside down
	//		cannnot use clamp because GATEWARE does not work with C++ 17....
	//		and we have to use GATEWARE.....
	float angle = XMConvertToRadians(0.1f * dy * sensitivity);
	XMMATRIX rotationX = XMMatrixRotationAxis(XMLoadFloat3(&camRight), angle);
	XMVECTOR newUp = XMVector3TransformNormal(XMLoadFloat3(&camUp), rotationX);
	XMVECTOR newForward = XMVector3TransformNormal(XMLoadFloat3(&camForward), rotationX);
	float newPitch = std::acos(XMVectorGetY(newForward));
	XMStoreFloat3(&camUp, newUp);
	XMStoreFloat3(&camForward, newForward);

	angle = 0;
}


void Camera::Yaw(float dx, float sensitivity)
{
	float angle = XMConvertToRadians(0.1f * dx * sensitivity);
	XMFLOAT3 globalUp(0, 1, 0);	// We want to yaw around the GLOBAL up
	XMMATRIX rotationY = XMMatrixRotationAxis(XMLoadFloat3(&globalUp), angle);
	XMStoreFloat3(&camRight, XMVector3TransformNormal(XMLoadFloat3(&camRight), rotationY));
	XMStoreFloat3(&camUp, XMVector3TransformNormal(XMLoadFloat3(&camUp), rotationY));
	XMStoreFloat3(&camForward, XMVector3TransformNormal(XMLoadFloat3(&camForward), rotationY));
}


float Camera::GetMouseXSensitivity()
{
	return mouseSensitivityX;
}

float Camera::GetMouseYSensitivity()
{
	return mouseSensitivityY;
}