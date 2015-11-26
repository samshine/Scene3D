
#pragma once

class CharacterController;
class SceneModelAttachment;

class EditorScene
{
public:
	virtual ~EditorScene() { }

	virtual CharacterController *get_character_controller() = 0;

	virtual void set_map_model(const std::string &map_model) = 0;
	virtual void set_model_data(std::shared_ptr<ModelData> model_data) = 0;
	virtual void set_attachments(std::vector<SceneModelAttachment> attachments) = 0;

	virtual void update(const ScenePtr &scene, const SceneViewportPtr &viewport, const uicore::GraphicContextPtr &gc, const uicore::DisplayWindowPtr &ic, bool has_focus, const uicore::Vec2i &mouse_delta) = 0;
};

class SceneModelAttachment
{
public:
	SceneModelAttachment() { }
	SceneModelAttachment(std::string attachment_name, std::string model_name, float model_scale) : attachment_name(std::move(attachment_name)), model_name(std::move(model_name)), model_scale(model_scale) { }

	std::string attachment_name;
	std::string model_name;
	float model_scale = 1.0f;

	SceneModelPtr model;
	SceneObjectPtr object;
};
