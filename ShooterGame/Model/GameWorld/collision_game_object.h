
#pragma once

class CollisionGameObject : public GameObject, public Physics3DDataObject
{
public:
	~CollisionGameObject()
	{
		detach();
		set_collision_object(nullptr);
	}

	void attach(const std::shared_ptr<GameObject> &game_obj)
	{
		auto base = std::dynamic_pointer_cast<CollisionGameObject>(game_obj);
		detach();
		if (base)
		{
			base->_attachments.push_back(this);
			_base = base;
		}
	}

	void detach()
	{
		auto base_ptr = _base.lock();
		if (base_ptr)
		{
			auto &attachments = base_ptr->_attachments;
			attachments.erase(std::find(attachments.begin(), attachments.end(), this));
		}
		_base.reset();
	}

	void set_collision_object(const Physics3DObjectPtr &obj)
	{
		if (_object == obj)
			return;

		if (_object)
			_object->set_data(nullptr);

		_object = obj;
		if (_object)
		{
			_object->set_data(this);
			_object->set_position(_position);
		}
	}

	void set_collision_position(const uicore::Vec3f &new_position)
	{
		auto delta = new_position - _position;
		for (const auto &attach : _attachments)
		{
			attach->base_movement(delta);
		}

		_position = new_position;
		if (_object)
			_object->set_position(new_position);
	}

private:
	void base_movement(const uicore::Vec3f &delta)
	{
		set_collision_position(_position + delta);
	}

	std::weak_ptr<CollisionGameObject> _base;
	std::vector<CollisionGameObject*> _attachments;
	uicore::Vec3f _position;
	Physics3DObjectPtr _object;
};
