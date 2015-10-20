
#pragma once

#include <memory>

class Physics3DWorld;
typedef std::shared_ptr<Physics3DWorld> Physics3DWorldPtr;
class Physics3DObject;
typedef std::shared_ptr<Physics3DObject> Physics3DObjectPtr;

class Physics3DContactPairTest
{
public:
	static std::shared_ptr<Physics3DContactPairTest> create(const Physics3DWorldPtr &world);

	virtual bool test(const Physics3DObjectPtr &object_a, const Physics3DObjectPtr &object_b) = 0;
};

typedef std::shared_ptr<Physics3DContactPairTest> Physics3DContactPairTestPtr;
