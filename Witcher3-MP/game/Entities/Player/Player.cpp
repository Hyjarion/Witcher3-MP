#include "Player.h"

Player::Player(uint32 _ID, Vector4F pos, float health, uint8 _characterId, std::shared_ptr<Networking::connection<MessageTypes>> _ownerClient)
{
	ID = _ID;
	Position = pos;
	ownerClient = _ownerClient;
	currHealth = health;
	MaxHealth = health;
	characterId = _characterId;
}

Player::~Player()
{

}

void Player::Update()
{

}

uint32 Player::GetID()
{
	return ID;
}

Vector4F Player::GetPosition()
{
	return Position;
}

void Player::UpdatePosition(Vector4F newPos)
{
	Position = newPos;
}