
#pragma once

#include "Scene3D/SceneEngine/resource.h"

class Model;
class ModelInstanceUniforms;
class ModelMeshVisitor;
class ModelDataBone;

class ModelReplacedMaterial
{
public:
	size_t texture_index;
	Resource<uicore::TexturePtr> replaced_texture;
};

class ModelAnimationTime
{
public:
	void play_animation(std::shared_ptr<Model> &renderer, const std::string &anim1, const std::string &anim2, bool instant);

	void update(std::shared_ptr<Model> &renderer, float time_elapsed);
	void moved(std::shared_ptr<Model> &renderer, float units_moved);

	void update_animation_index(std::shared_ptr<Model> &renderer);

	const std::string &get_animation() const { return animation_name; }
	int get_animation_index() const { return animation_index; }
	float get_animation_time() const { return animation_time; }

private:
	void next_animation(std::shared_ptr<Model> &renderer);

	std::string animation_name = "default";
	std::string next_animation_name;
	std::string next_animation_name2;

	int animation_index = -1;
	float animation_time = 0.0f;
};

class ModelInstance
{
public:
	ModelInstance();
	std::shared_ptr<Model> get_renderer() const { return renderer; }
	void set_renderer(std::shared_ptr<Model> renderer);
	void play_animation(const std::string &name, bool instant);
	void play_transition(const std::string &anim1, const std::string &anim2, bool instant);
	void update(float time_elapsed);
	void moved(float units_moved);

	const std::string &get_animation() const { return cur_anim.get_animation(); }
	int get_animation_index() const { return cur_anim.get_animation_index(); }
	float get_animation_time() const { return cur_anim.get_animation_time(); }

	void get_attachment_location(const std::string &name, uicore::Vec3f &position, uicore::Quaternionf &orientation) const;

	void get_bone_transform(size_t bone_index, const ModelDataBone &bone, uicore::Vec3f &position, uicore::Quaternionf &orientation) const;

	std::vector<uicore::Vec3f> ragdoll_bone_positions;
	std::vector<uicore::Quaternionf> ragdoll_bone_orientations;

private:
	ModelAnimationTime last_anim;
	ModelAnimationTime cur_anim;
	float transition = 1.0f;

	std::vector<ModelReplacedMaterial> replaced_materials;
	std::shared_ptr<Model> renderer;

	// To do: add selection id

	friend class Model;
};
