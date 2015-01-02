
#pragma once

class CharacterController;
class SceneModelAttachment;

class EditorScene
{
public:
	virtual ~EditorScene() { }

	virtual CharacterController *get_character_controller() = 0;

	virtual void set_map_model(const std::string &map_model) = 0;
	virtual void set_model_data(std::shared_ptr<clan::ModelData> model_data) = 0;
	virtual void set_attachments(std::vector<SceneModelAttachment> attachments) = 0;

	virtual void update(clan::Scene &scene, clan::GraphicContext &gc, clan::InputContext &ic, bool has_focus) = 0;
};

class SceneModelAttachment
{
public:
	SceneModelAttachment() { }
	SceneModelAttachment(std::string attachment_name, std::string model_name, float model_scale) : attachment_name(std::move(attachment_name)), model_name(std::move(model_name)), model_scale(model_scale) { }

	std::string attachment_name;
	std::string model_name;
	float model_scale = 1.0f;

	clan::SceneModel model;
	clan::SceneObject object;
};
