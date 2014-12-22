/*
**  ClanLib SDK
**  Copyright (c) 1997-2013 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#pragma once

namespace clan
{

class Model;
class ModelInstanceUniforms;
class ModelMeshVisitor;
class ModelDataBone;
class AxisAlignedBoundingBox;

class ModelReplacedMaterial
{
public:
	size_t texture_index;
	Resource<Texture> replaced_texture;
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

	void get_attachment_location(const std::string &name, Vec3f &position, Quaternionf &orientation) const;

	void get_bone_transform(const ModelDataBone &bone, Vec3f &position, Quaternionf &orientation) const;

private:
	ModelAnimationTime last_anim;
	ModelAnimationTime cur_anim;
	float transition = 1.0f;

	std::vector<ModelReplacedMaterial> replaced_materials;
	std::shared_ptr<Model> renderer;

	// To do: add selection id

	friend class Model;
};

}

