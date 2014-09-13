
#pragma once

#include <UICore/View/view.h>
#include <UICore/StandardViews/label_view.h>
#include <UICore/StandardViews/text_field_view.h>

class RolloutListItemView;

class RolloutList : public uicore::View
{
public:
	RolloutList();

	std::shared_ptr<RolloutListItemView> selection();

	uicore::Signal<void()> &sig_selection_changed() { return selection_changed; }
	uicore::Signal<void()> &sig_edit_saved() { return edit_saved; }

	std::shared_ptr<RolloutListItemView> add_animation(const std::string &anim_name);

private:
	uicore::Signal<void()> selection_changed;
	uicore::Signal<void()> edit_saved;
};

class RolloutListItemView : public uicore::View
{
public:
	RolloutListItemView(size_t index);

	std::string text() const;

	bool selected() const;
	void set_selected(bool value = true, bool animate = true);
	void set_text(const std::string &text);

	size_t index;

private:
	void begin_edit();
	void save_edit();
	void cancel_edit();

	bool is_selected = false;
	std::shared_ptr<uicore::LabelView> label;
	std::shared_ptr<uicore::TextFieldView> textfield;
};
