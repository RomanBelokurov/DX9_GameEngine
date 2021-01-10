#ifndef __PLAYER_CLASS_H_
#define __PLAYER_CLASS_H_
#pragma once

#include	<d3dx9math.h>

#define MOVEMENT_SPEED 0.2f
#define MOUSE_SENSITIVITY_X 0.1f
#define MOUSE_SENSITIVITY_Y 0.1f

class PlayerClass {
	
public:
	PlayerClass(D3DXVECTOR3, D3DXVECTOR3);
	~PlayerClass();

	D3DXVECTOR3 GetPlayerPosition() { return m_playerPosition; }
	D3DXVECTOR3 GetPlayerRotation() { return m_playerRotation; }

	// Actions:
	void MoveForward(float);
	void MoveBackward(float);

	void StrafeLeft(float);
	void StrafeRight(float);

	void LookAround(float, float, float);


private:
	D3DXVECTOR3 m_playerPosition;
	D3DXVECTOR3 m_playerRotation;
};

PlayerClass::PlayerClass(D3DXVECTOR3 playerPosition, D3DXVECTOR3 playerRotation) {
	m_playerPosition = playerPosition;
	m_playerRotation = playerRotation;
}

PlayerClass::~PlayerClass() 
{
}

void PlayerClass::MoveForward(float frameTime) 
{
	float radians = m_playerRotation.y * 0.0174532925f;
	float movementSpeed = MOVEMENT_SPEED * frameTime;

	// Update the position.
	m_playerPosition.x += sinf(radians) * movementSpeed;
	m_playerPosition.z += cosf(radians) * movementSpeed;
}

void PlayerClass::MoveBackward(float frameTime) 
{
	float radians = m_playerRotation.y * 0.0174532925f;
	float movementSpeed = MOVEMENT_SPEED * frameTime;

	// Update the position.
	m_playerPosition.x -= sinf(radians) * movementSpeed;
	m_playerPosition.z -= cosf(radians) * movementSpeed;
}

void PlayerClass::StrafeLeft(float frameTime) 
{
	float radians = (m_playerRotation.y + 90) * 0.0174532925f;
	float movementSpeed = MOVEMENT_SPEED * frameTime;

	// Update the position.
	m_playerPosition.x -= sinf(radians) * movementSpeed;
	m_playerPosition.z -= cosf(radians) * movementSpeed;
}

void PlayerClass::StrafeRight(float frameTime) 
{
	float radians = (m_playerRotation.y + 90) * 0.0174532925f;
	float movementSpeed = MOVEMENT_SPEED * frameTime;

	// Update the position.
	m_playerPosition.x += sinf(radians) * movementSpeed;
	m_playerPosition.z += cosf(radians) * movementSpeed;
}

void PlayerClass::LookAround(float frameTime, float dx, float dy)
{
	if (dx != 0)
	{
		m_playerRotation.y += dx * MOUSE_SENSITIVITY_X * frameTime;
		if (m_playerRotation.y < 0.0f) m_playerRotation.y -= 360.0f;
	}

	if (dy != 0)
	{
		m_playerRotation.x += dy * MOUSE_SENSITIVITY_Y * frameTime;

		if (m_playerRotation.x > 90.0f)
			m_playerRotation.x = 90.0f;

		if (m_playerRotation.x < -90.0f)
			m_playerRotation.x = -90.0f;
	}
}

#endif