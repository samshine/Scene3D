
#pragma once

class SceneViewportImpl;

class ScenePass
{
public:
	virtual ~ScenePass() { }

	virtual std::string name() const = 0;
	virtual void run() = 0;
	virtual void update() { }
};
