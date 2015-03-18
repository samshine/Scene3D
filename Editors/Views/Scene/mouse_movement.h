
#pragma once

class MouseMovement
{
public:
	MouseMovement();
	~MouseMovement();

	clan::Point pos() const { return clan::Point(x(), y()); }
	int x() const { return _x; }
	int y() const { return _y; }

private:
	MouseMovement(const MouseMovement &);
	MouseMovement &operator=(const MouseMovement &);
	LRESULT window_proc(HWND window_handle, UINT message_id, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK static_window_proc(HWND window_handle, UINT message_id, WPARAM wparam, LPARAM lparam);
	void worker_main();

	std::thread thread;
	HANDLE stop_event;
	std::atomic_int _x, _y;
	HWND window_handle;
};
