
#pragma once

#include <memory>

class Physics3DWorld;
typedef std::shared_ptr<Physics3DWorld> Physics3DWorldPtr;
class Physics3DObject;
class Physics3DContactPairTestImpl;

class Physics3DContactPairTest
{
public:
	Physics3DContactPairTest();
	Physics3DContactPairTest(const Physics3DWorldPtr &world);

	bool is_null() const;

	bool test(const Physics3DObject &object_a, const Physics3DObject &object_b);

private:
	std::shared_ptr<Physics3DContactPairTestImpl> impl;
};
