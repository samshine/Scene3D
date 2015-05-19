
#pragma once

#include <vector>

class Team
{
public:
	std::string name;
	int score;
};

class TeamList
{
public:
	virtual ~TeamList() { }

	std::vector<Team> teams;
};
