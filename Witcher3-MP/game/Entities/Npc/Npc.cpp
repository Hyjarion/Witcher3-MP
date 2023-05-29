#include "Npc.h"

Npc::Npc(uint32 _ID, uint32 _ResID, Vector4F pos, float health)
{
	ID = _ID;
	ResID = _ResID;
	Position = pos;
	currHealth = health;
	MaxHealth = health;
}

Npc::~Npc()
{

}

void Npc::Update()
{

}

uint32 Npc::GetID()
{
	return ID;
}

Vector4F Npc::GetPosition()
{
	return Position;
}

void Npc::UpdatePosition(Vector4F newPos)
{
	Position = newPos;
}