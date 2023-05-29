#pragma once

enum class MessageTypes : uint32_t
{
	TC_REQUEST_PLAYERDATA,
	TS_SEND_PLAYERDATA, // vec4f, uint8 characterId

	TC_CREATE_PLAYER, // uint32 id, vec4f, uint8 characterId
	TC_MASS_CREATE_PLAYER,

	TC_DESTROY_PLAYER, // uint32 id

	TS_NOTIFY_PLAYER_POS_CHANGE, // vec4f, movetype

	TC_UPDATE_POS, // uint32 id, vec4f, movetype

	TC_SET_ACTOR_HEALTH, // id, bool player, float currhealthvalue, float maxhealthvalue 

	TC_CREATE_NPC, // uint32 npcID, uint32 resID, vec4f, health

	TS_HIT_NPC, // uint32 npcID
	TS_GOT_HIT,

	TC_NPC_DEAD, // uint32 npcID

	TC_PLAYER_DEAD,

	TS_CHAT_MESSAGE, // char message[100] message
	TC_CHAT_MESSAGE	// uint32 playerId, char message[100]
};
