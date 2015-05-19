
#pragma once

#include <string>
#include <map>

class Game;
class GameWorld;
class GameTick;

class GameObject
{
public:
	GameObject(GameWorld *world);
	virtual ~GameObject();

	virtual void tick(const GameTick &tick) { }
	virtual void frame(float time_elapsed, float interpolated_time) { }

	GameWorld *world() const { return _world; }
	int id() const { return _id; }

private:
	GameWorld *_world;
	int _id = 0;

	friend class GameWorld;
};
