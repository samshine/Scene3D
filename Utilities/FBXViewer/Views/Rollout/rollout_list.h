
#pragma once

class RolloutListItemView;

class RolloutList : public clan::View
{
public:
	RolloutList();

	std::shared_ptr<RolloutListItemView> selection();

	clan::Signal<void()> &sig_selection_changed() { return selection_changed; }
	clan::Signal<void()> &sig_edit_saved() { return edit_saved; }
	clan::Signal<void()> &sig_selection_clicked() { return selection_clicked; }

	void clear();
	std::shared_ptr<RolloutListItemView> add_item(const std::string &item_name);

	bool is_edit_allowed() const { return allow_edit; }
	void set_allow_edit(bool enable) { allow_edit = enable; }

private:
	clan::Signal<void()> selection_changed;
	clan::Signal<void()> selection_clicked;
	clan::Signal<void()> edit_saved;
	bool allow_edit = true;
};

class RolloutListItemView : public clan::View
{
public:
	RolloutListItemView(size_t index);

	std::string text() const;

	bool selected() const;
	void set_selected(bool value = true, bool animate = true);
	void set_text(const std::string &text);
	void set_bold(bool enable);

	size_t index;

private:
	void begin_edit();
	void save_edit();
	void cancel_edit();

	bool is_selected = false;
	std::shared_ptr<clan::LabelView> label;
	std::shared_ptr<clan::TextFieldView> textfield;
};
