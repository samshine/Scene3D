
#pragma once

class GameObject;

class GameObjectCollision
{
public:
	GameObjectCollision(GameObject *obj) : obj(obj) { }

	GameObject *obj;
};
