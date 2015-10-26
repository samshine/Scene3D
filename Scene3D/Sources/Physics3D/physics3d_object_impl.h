
#pragma once

#include "Physics3D/physics3d_object.h"
#include "Physics3D/physics3d_shape.h"
#include "Physics3D/Bullet/btBulletDynamicsCommon.h"

class Physics3DWorldImpl;

class Physics3DObjectImpl : public Physics3DObject
{
public:
	Physics3DObjectImpl(Physics3DWorldImpl *world);
	~Physics3DObjectImpl();

	uicore::Vec3f position() const override;
	uicore::Quaternionf orientation() const override;

	bool static_object() const override;
	bool kinematic_object() const override;
	bool character_object() const override;
	bool debug_drawn() const override;

	Physics3DDataObject *data_object() override;

	void set_position(const uicore::Vec3f &position) override;
	void set_orientation(const uicore::Quaternionf &orientation) override;
	void set_transform(const uicore::Vec3f &position, const uicore::Quaternionf &orientation) override;

	void set_static_object(bool enable) override;
	void set_kinematic_object(bool enable) override;
	void set_character_object(bool enable) override;
	void set_debug_drawn(bool enable) override;

	void set_data(Physics3DDataObject *obj) override;

	void set_mass(float mass, const uicore::Vec3f &local_inertia) override;
	void set_sleeping_thresholds(float linear, float angular) override;

	void set_ccd_swept_sphere_radius(float radius) override;
	void set_ccd_motion_threshold(float motion_threshold) override;

	void activate(bool force_activation = false) override;

	void apply_central_force(const uicore::Vec3f &force) override;
	void apply_torque(const uicore::Vec3f &torque) override;
	void apply_force(const uicore::Vec3f &force, const uicore::Vec3f &relative_pos) override;
	void apply_central_impulse(const uicore::Vec3f &force) override;
	void apply_torque_impulse(const uicore::Vec3f &torque) override;
	void apply_impulse(const uicore::Vec3f &impulse, const uicore::Vec3f &relative_pos) override;
	void clear_forces() override;

	void add_constraint(const Physics3DConstraintPtr &constraint) override;
	void remove_constraint(const Physics3DConstraintPtr &constraint) override;

	Physics3DWorldImpl *world;
	Physics3DShapePtr shape;
	std::unique_ptr<btCollisionObject> object;
	Physics3DDataObject *data = nullptr;
};
