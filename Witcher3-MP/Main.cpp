#include "networking/net_message.h"
#include "networking/net_tsqueue.h"
#include "networking/net_connection.h"
#include "networking/net_client.h"
#include "networking/net_server.h"
#include "networking/PacketTypes.h"

#include "Common.h"
#include <vector>
#include <map>
#include <sstream>
#include <fstream>

#include "game/Entities/Player/Player.h"
#include "game/Entities/Npc/Npc.h"
#include "database/ResourceNames.h"

uint32 highestPlayerID = 1;
std::vector<Player*> PlayerList;

uint32 highestNpcID = 1;
std::vector<Npc*> NpcList;

class Witcher3MPServer : public Networking::server_interface<MessageTypes>
{
public:
	Witcher3MPServer(uint16_t nPort) : Networking::server_interface<MessageTypes>(nPort)
	{}

protected:
	virtual bool OnClientConnect(std::shared_ptr<Networking::connection<MessageTypes>> client)
	{
		Networking::message<MessageTypes> msg;
		msg.header.id = MessageTypes::TC_REQUEST_PLAYERDATA;
		MessageClient(client, msg);
		return true;
	}

	virtual void OnClientDisconnect(std::shared_ptr<Networking::connection<MessageTypes>> client)
	{
		std::cout << "Client disconnected [" << client->GetID() << "]\n";

		for (uint8 i = 0; i < PlayerList.size(); ++i)
		{
			Player* ply = PlayerList[i];

			if (ply != nullptr)
			{
				if (ply->ownerClient == client)
				{
					PlayerList[i] = nullptr;	// I will delete the element from the vector next update

					Networking::message<MessageTypes> msg; // temporary solution: sending a position update to cords 0,0,0 cause I am lazy to create an entity destroy message xD
					msg.header.id = MessageTypes::TC_UPDATE_POS;
					uint32 id = ply->GetID();
					Vector4F pos;
					uint8 movetype = 1;
					msg << id << pos << movetype;
					for (auto i : PlayerList)
					{
						if (i == nullptr)
							continue;
						MessageClient(i->ownerClient, msg);
					}

					std::cout << "Kicking Player: " << id << std::endl;
					delete PlayerList[i];
				}
			}
		}
	}

	virtual void OnMessageReceived(std::shared_ptr<Networking::connection<MessageTypes>> client, Networking::message<MessageTypes>& msg)
	{
		switch (msg.header.id)
		{
			case MessageTypes::TS_SEND_PLAYERDATA:
			{
				Vector4F recPosition;
				uint8 characterId;
				msg >> characterId >> recPosition;

				uint32 newPlyID = highestPlayerID++;

				Networking::message<MessageTypes> msg;
				msg.header.id = MessageTypes::TC_CREATE_PLAYER;
				msg << newPlyID << recPosition << characterId;

				for (auto i : PlayerList)
				{
					if (i->ownerClient != client)
						MessageClient(i->ownerClient, msg);
				}

				if (PlayerList.size())
				{
					Networking::message<MessageTypes> massCreate;
					massCreate.header.id = MessageTypes::TC_MASS_CREATE_PLAYER;

					for (auto i : PlayerList)
					{
						uint32 ID = i->GetID();
						Vector4F pos = i->GetPosition();
						
						massCreate << ID << pos << i->characterId;
					}

					MessageClient(client, massCreate);
				}

				Player* newPly = new Player(newPlyID, recPosition, 1000.f, characterId, client);
				PlayerList.push_back(newPly);

				std::cout << "New Player created with ID: " + std::to_string(newPlyID) << std::endl;

				Networking::message<MessageTypes> setHealth;
				setHealth.header.id = MessageTypes::TC_SET_ACTOR_HEALTH;
				uint32 setHealth_id = 0;
				bool setHealth_isPlayer = true;
				float setHealth_currhealthvalue = 1000;
				float setHealth_maxhealthvalue = 1000;
				setHealth << setHealth_id << setHealth_isPlayer << setHealth_currhealthvalue << setHealth_maxhealthvalue;
				MessageClient(client, setHealth);
				break;
			}
			case MessageTypes::TS_NOTIFY_PLAYER_POS_CHANGE:
			{
				Player* affected = nullptr;
				for (Player* i : PlayerList)
					if (i->ownerClient == client)
					{
						affected = i;
						break;
					}

				if (affected != nullptr)
				{
					Vector4F newPos;
					uint8 MoveType;
					msg >> MoveType >> newPos;

					affected->UpdatePosition(newPos);

					Networking::message<MessageTypes> updatePos;
					updatePos.header.id = MessageTypes::TC_UPDATE_POS;
					uint32 playerId = affected->GetID();
					updatePos << playerId << newPos << MoveType;

					MessageAllClients(updatePos, affected->ownerClient);
				}
				break;
			}
			case MessageTypes::TS_HIT_NPC:
			{
				uint32 Id;
				msg >> Id;

				for (auto i : NpcList)
				{
					if (i->GetID() == Id)
					{
						float damage = 50.f;

						if (i->GetHealth() > damage)
						{
							i->SetHealth(i->GetHealth() - damage);

							Networking::message<MessageTypes> healthMsg;
							healthMsg.header.id = MessageTypes::TC_SET_ACTOR_HEALTH;

							uint32 healthMsg_id = Id;
							bool healthMsg_isPlayer = false;
							float healthMsg_currhealthvalue = i->GetHealth();
							float healthMsg_maxhealthvalue = i->GetMaxHealth(); 
							healthMsg << healthMsg_id << healthMsg_isPlayer << healthMsg_currhealthvalue << healthMsg_maxhealthvalue;

							for (auto j : PlayerList)
								MessageClient(j->ownerClient, healthMsg);
						}
						else
						{
							i->SetHealth(0.f);

							Networking::message<MessageTypes> death_msg;
							death_msg.header.id = MessageTypes::TC_NPC_DEAD;
							death_msg << Id;

							for (auto j : PlayerList)
								MessageClient(j->ownerClient, death_msg);
						}
						break;
					}
				}
				break;
			}
			case MessageTypes::TS_GOT_HIT:
			{
				for (auto i : PlayerList)
				{
					if (i->ownerClient == client)
					{
						float damage = 100.f;

						if (i->GetHealth() > damage)
						{
							i->SetHealth(i->GetHealth() - damage);

							Networking::message<MessageTypes> healthMsg;
							healthMsg.header.id = MessageTypes::TC_SET_ACTOR_HEALTH;

							uint32 healthMsg_id = 0;
							bool healthMsg_isPlayer = true;
							float healthMsg_currhealthvalue = i->GetHealth();
							float healthMsg_maxhealthvalue = i->GetMaxHealth();
							healthMsg << healthMsg_id << healthMsg_isPlayer << healthMsg_currhealthvalue << healthMsg_maxhealthvalue;

							MessageClient(client, healthMsg);
						}
						else
						{
							i->SetHealth(i->GetMaxHealth());

							Networking::message<MessageTypes> msg;
							msg.header.id = MessageTypes::TC_PLAYER_DEAD;
							MessageClient(client, msg);
						}
						break;
					}
				}
				break;
			}
			case MessageTypes::TS_CHAT_MESSAGE:
			{
				for (auto i : PlayerList)
				{
					if (i->ownerClient == client)
					{
						std::string chat_message;
						for (uint8 i = 0; i < 100; ++i)
						{
							if (msg.size())
							{
								char ch;
								msg >> ch;
								chat_message += ch;
							}
						}

						if (chat_message.length() == 0)
							break;

						size_t length = chat_message.length();
						for (size_t i = 0; i < length / 2; i++)
							std::swap(chat_message[i], chat_message[length - i - 1]);

						Networking::message<MessageTypes> msg;
						msg.header.id = MessageTypes::TC_CHAT_MESSAGE;
						uint32 playerId = i->GetID();
						msg << playerId;
						for (auto i : chat_message)
							msg << i;

						for(auto i : PlayerList)
							if(i->ownerClient != client)
								MessageClient(i->ownerClient, msg);


						Networking::message<MessageTypes> msg2;
						msg2.header.id = MessageTypes::TC_CHAT_MESSAGE;
						uint32 playerId2 = 0;
						msg2 << playerId2;
						for (auto i : chat_message)
							msg2 << i;
						playerId = 0;
						MessageClient(client, msg2);
						break;
					}
				}
				break;
			}
		}
	}
};

Witcher3MPServer* w3server;

std::vector<std::string> commandQueue;

void receive_commands()
{
	while (true)
	{
		std::string command = "";
		std::getline(std::cin, command);

		commandQueue.push_back(command);
	}
}

void handle_commands()
{
	if (!commandQueue.size())
		return;

	for (uint8 i = 0; i < commandQueue.size(); ++i)
	{
		std::string command = commandQueue[i];

		if (command == "")
			continue;

		std::stringstream stream(command);
		std::string segment;
		std::vector<std::string> segments;

		while (std::getline(stream, segment, ' '))
			segments.push_back(segment);

		// spawn: resourcename, toPlayerID
		if (segments[0] == "spawn" && segments.size() == 3)
		{
			if (uint32 ResID = Database::ResourceNames::FindResID(segments[1]))
			{
				uint32 toPlayerID = std::atoi(segments[2].c_str());

				Vector4F SpawnTo;

				for (auto i : PlayerList)
				{
					if (i->GetID() == toPlayerID)
					{
						SpawnTo = i->GetPosition();
						break;
					}
				}

				if (!SpawnTo.null())
				{
					uint32 newNpcID = highestNpcID++;
					float newNpcHealth = 500.f;
					if (ResID == 243)
						newNpcHealth = 3000.f;
					if (ResID == 244)
						newNpcHealth = 5000.f;
					if (ResID == 242)
						newNpcHealth = 10000.f;
					Npc* newNpc = new Npc(newNpcID, ResID, SpawnTo, newNpcHealth);
					NpcList.push_back(newNpc);

					std::cout << "NPC Spawned" << std::endl;

					Networking::message<MessageTypes> msg;
					msg.header.id = MessageTypes::TC_CREATE_NPC;
					msg << newNpcID << ResID << SpawnTo << newNpcHealth;
					w3server->MessageAllClients(msg);
				}
			}
		}

		commandQueue[i] = "";
	}

	commandQueue.erase(std::remove(commandQueue.begin(),
		commandQueue.end(), ""), commandQueue.end());
}

int main()
{
	system("Color 03");
	setlocale(LC_ALL, "");

	std::cout << R"(
 __        ___ _       _               ____                   
 \ \      / (_) |_ ___| |__   ___ _ __/ ___| _   _ _ __   ___ 
  \ \ /\ / /| | __/ __| '_ \ / _ \ '__\___ \| | | | '_ \ / __|
   \ V  V / | | || (__| | | |  __/ |   ___) | |_| | | | | (__ 
    \_/\_/  |_|\__\___|_| |_|\___|_|  |____/ \__, |_| |_|\___|
                                             |___/                
                 2022.02 - 04, Developer: Hyjarion
                                                      v0.0.1  )" << "\n";

	std::ifstream portfile("Port.cfg", std::ifstream::in);
	std::string portstr;
	std::getline(portfile, portstr);
	uint16 port = std::atoi(portstr.c_str());
	portfile.close();
	if (port == 0)
		return 0;

	w3server = new Witcher3MPServer(port);

	w3server->Start();

	std::thread commandProcessor(receive_commands);

	while (true)
	{
		handle_commands();

		if (PlayerList.size())
		{
			PlayerList.erase(std::remove(PlayerList.begin(),
			PlayerList.end(), nullptr), PlayerList.end());
		}

		w3server->Update(-1, false);
	}

	return 0;
}