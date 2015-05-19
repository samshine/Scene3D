
#pragma once

#include <vector>

class Player
{
public:
	std::string peer_id;
	std::string name;
	int score = 0;
	int ping = 0;
	int kills = 0;
	int deaths = 0;
	int friendly_kills = 0;
	int bullet_hit = 0;
	int bullet_missed = 0;
	int flags_picked_up = 0;
	int flags_dropped = 0;
	int flags_returned = 0;
	int flags_captured = 0;
	std::string country;
	int team = 0;

	int health = 0;
	int armor = 0;
	int ammo = 0;
};

class PlayerList
{
public:
	virtual ~PlayerList() { }

	std::vector<Player> players;
	int local_player_index = -1;
};
