
#pragma once

#include "Physics3D/physics3d_constraint.h"
#include "Physics3D/physics3d_object.h"
#include "Physics3D/physics3d_shape.h"
#include "Physics3D/physics3d_world.h"

#if defined(_MSC_VER)
	#if !defined(_DEBUG)
		#pragma comment(lib, "Scene3D.lib")
	#else
		#pragma comment(lib, "Scene3Dd.lib")
	#endif
#endif
