
#pragma once

class RolloutListItemView;

class RolloutList : public uicore::View
{
public:
	RolloutList();

	std::shared_ptr<RolloutListItemView> selection();

	uicore::Signal<void()> &sig_selection_changed() { return selection_changed; }
	uicore::Signal<void()> &sig_edit_saved() { return edit_saved; }
	uicore::Signal<void()> &sig_selection_clicked() { return selection_clicked; }

	void clear();
	std::shared_ptr<RolloutListItemView> add_item(const std::string &item_name);

	bool is_edit_allowed() const { return allow_edit; }
	void set_allow_edit(bool enable) { allow_edit = enable; }

private:
	uicore::Signal<void()> selection_changed;
	uicore::Signal<void()> selection_clicked;
	uicore::Signal<void()> edit_saved;
	bool allow_edit = true;
};

class RolloutListItemView : public uicore::View
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
	std::shared_ptr<uicore::LabelView> label;
	std::shared_ptr<uicore::TextFieldView> textfield;
};
