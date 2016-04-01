
#pragma once

#include "Views/Theme/theme_views.h"

class RolloutListItemView;

class RolloutList : public ThemeScrollView
{
public:
	RolloutList();

	uicore::Signal<void()> &sig_selection_changed() { return _selection_changed; }
	uicore::Signal<void()> &sig_edit_saved() { return _edit_saved; }
	uicore::Signal<void()> &sig_selection_clicked() { return _selection_clicked; }

	void clear();
	int add_item(const std::string &text);
	void remove_item(int index);

	std::string item_text(int index) const;
	void set_item_text(int index, std::string text);

	int selected_item() const;
	void clear_selection();
	void set_selected(int index);

	void set_bold(int index, bool value = true);

	bool is_edit_allowed() const { return _allow_edit; }
	void set_allow_edit(bool enable) { _allow_edit = enable; }

private:
	int find_index(const RolloutListItemView *item) const;

	int _selected_item = -1;
	std::vector<std::shared_ptr<RolloutListItemView>> _items;
	uicore::Signal<void()> _selection_changed;
	uicore::Signal<void()> _selection_clicked;
	uicore::Signal<void()> _edit_saved;
	bool _allow_edit = true;

	friend class RolloutListItemView;
};

class RolloutListItemView : public uicore::ColumnView
{
public:
	RolloutListItemView(RolloutList *list);

	RolloutList *list = nullptr;
	std::shared_ptr<uicore::LabelBaseView> label;
	std::shared_ptr<uicore::TextFieldBaseView> textfield;

	void set_selected();
	void clear_selected();
	void set_bold();
	void clear_bold();

	void begin_edit();
	void save_edit();
	void cancel_edit();
};
