
#pragma once

class SceneMoveAction : public uicore::ViewAction
{
public:
	SceneMoveAction();

	void track_keys(const std::vector<uicore::Key> &keys);

	void deactivated(uicore::ActivationChangeEvent &e) override;
	void focus_lost(uicore::FocusChangeEvent &e) override;
	void key_press(uicore::KeyEvent &e) override;
	void key_release(uicore::KeyEvent &e) override;

	std::function<void(std::map<uicore::Key, bool> &keys, float time_elapsed)> func_key_repeat;

private:
	uicore::TimerPtr timer = uicore::Timer::create();
	uicore::GameTime gametime;
	std::map<uicore::Key, bool> keys;
};
