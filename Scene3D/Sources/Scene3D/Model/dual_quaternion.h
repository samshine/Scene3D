
#pragma once

template<typename Type>
class DualQuaternionx
{
public:
	DualQuaternionx();
	DualQuaternionx(const uicore::Vec3f translate, const uicore::Quaternionf orientation);
	uicore::Mat4<Type> to_matrix();

	uicore::Quaternionx<Type> first;
	uicore::Quaternionx<Type> second;
};

template<typename Type>
DualQuaternionx<Type>::DualQuaternionx()
{
}

template<typename Type>
DualQuaternionx<Type>::DualQuaternionx(const uicore::Vec3f translate, const uicore::Quaternionf orientation)
{
	first = orientation;
	second.x = Type( 0.5) * ( translate.x * orientation.w + translate.y * orientation.z - translate.z * orientation.y);
	second.y = Type( 0.5) * (-translate.x * orientation.z + translate.y * orientation.w + translate.z * orientation.x);
	second.z = Type( 0.5) * ( translate.x * orientation.y - translate.y * orientation.x + translate.z * orientation.w);
	second.w = Type(-0.5) * ( translate.x * orientation.x + translate.y * orientation.y + translate.z * orientation.z);
}

template<typename Type>
uicore::Mat4<Type> DualQuaternionx<Type>::to_matrix()
{
	Type length = first.magnitude();
	Type x = first.x;
	Type y = first.y;
	Type z = first.z;
	Type w = first.w;
	Type t1 = second.x;
	Type t2 = second.y;
	Type t3 = second.z;
	Type t0 = second.w;

	uicore::Mat4<Type> result;
	result.m[0][0] = w*w + x*x - y*y - z*z; 
	result.m[1][0] = 2*x*y - 2*w*z;
	result.m[2][0] = 2*x*z + 2*w*y;
	result.m[0][1] = 2*x*y + 2*w*z;
	result.m[1][1] = w*w + y*y - x*x - z*z;
	result.m[2][1] = 2*y*z - 2*w*x;
	result.m[0][2] = 2*x*z - 2*w*y;
	result.m[1][2] = 2*y*z + 2*w*x;
	result.m[2][2] = w*w + z*z - x*x - y*y;

	result.m[3][0] = -2*t0*x + 2*t1*w - 2*t2*z + 2*t3*y;
	result.m[3][1] = -2*t0*y + 2*t1*z + 2*t2*w - 2*t3*x;
	result.m[3][2] = -2*t0*z - 2*t1*y + 2*t2*x + 2*t3*w;

	result.m[0][3] = Type(0);
	result.m[1][3] = Type(0);
	result.m[2][3] = Type(0);
	result.m[3][3] = Type(1);

	result /= length;
	return result;
}

typedef DualQuaternionx<float> DualQuaternionf;
typedef DualQuaternionx<double> DualQuaterniond;
