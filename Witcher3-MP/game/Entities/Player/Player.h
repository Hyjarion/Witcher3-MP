#pragma once

#include "../../../Common.h"

#include "../../../networking/net_message.h"
#include "../../../networking/net_tsqueue.h"
#include "../../../networking/net_connection.h"
#include "../../../networking/net_client.h"
#include "../../../networking/net_server.h"
#include "../../../networking/PacketTypes.h"

class Player
{
public:
	Player(uint32 _ID, Vector4F pos, float health, uint8 _characterId, std::shared_ptr<Networking::connection<MessageTypes>> _ownerClient);
	~Player();

	void Update();

	uint32 GetID();
	Vector4F GetPosition();
	float GetHealth() { return currHealth; };
	float GetMaxHealth() { return MaxHealth; };

	void SetHealth(float health) { currHealth = health; };
	void SetMaxHealth(float maxhealth) { MaxHealth = maxhealth; };
	void UpdatePosition(Vector4F newPos);

	uint8 characterId;
	std::shared_ptr<Networking::connection<MessageTypes>> ownerClient;

private:
	uint32 ID;
	Vector4F Position;
	float currHealth;
	float MaxHealth;
};
