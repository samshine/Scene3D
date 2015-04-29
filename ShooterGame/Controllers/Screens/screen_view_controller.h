
#pragma once

class ScreenViewController : public clan::ViewController
{
public:
	ScreenViewController(clan::Canvas &canvas);

	std::shared_ptr<clan::TextureView> texture_view();

	virtual void update_desktop(clan::Canvas &canvas) { }
};

class Screen
{
public:
	static void set(const std::shared_ptr<ScreenViewController> &active);
	static std::shared_ptr<ScreenViewController> get();

private:
	static std::shared_ptr<ScreenViewController> active;
};
