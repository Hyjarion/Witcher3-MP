#pragma once

#include "../../../Common.h"

#include "../../../networking/net_message.h"
#include "../../../networking/net_tsqueue.h"
#include "../../../networking/net_connection.h"
#include "../../../networking/net_client.h"
#include "../../../networking/net_server.h"
#include "../../../networking/PacketTypes.h"

class Npc
{
public:
	Npc(uint32 _ID, uint32 _ResID, Vector4F pos, float health);
	~Npc();

	void Update();

	uint32 GetID();
	Vector4F GetPosition();
	float GetHealth() { return currHealth; };
	float GetMaxHealth() { return MaxHealth; };

	void SetHealth(float health) { currHealth = health; };
	void SetMaxHealth(float maxhealth) { MaxHealth = maxhealth; };
	void UpdatePosition(Vector4F newPos);

private:
	uint32 ID;
	uint32 ResID;
	Vector4F Position;
	float currHealth;
	float MaxHealth;
};