
#include "precomp.h"
#include "rollout_list.h"

using namespace uicore;

RolloutList::RolloutList()
{
	style()->set(R"(
		border: 1px solid rgb(100,100,100);
		flex: 1 0 auto;
		flex-direction: column;
		height: 300px;
		padding: 5px;
		)");

	content_view()->style()->set("flex-direction: column");
}

void RolloutList::clear()
{
	_selected_item = -1;
	for (auto &item : _items)
		item->remove_from_parent();
	_items.clear();
}

int RolloutList::add_item(const std::string &text)
{
	auto item = content_view()->add_child<RolloutListItemView>(this);
	item->label->set_text(text);
	item->textfield->set_text(text);
	_items.push_back(item);
	return _items.size() - 1;
}

void RolloutList::remove_item(int index)
{
	if (index < 0 || index >= (int)_items.size())
		return;

	if (selected_item() == index)
		clear_selection();

	_items[index]->remove_from_parent();
	_items.erase(_items.begin() + index);
}

std::string RolloutList::item_text(int index) const
{
	if (index < 0 || index >= (int)_items.size())
		return std::string();

	return _items[index]->label->text();
}

void RolloutList::set_item_text(int index, std::string text)
{
	if (index < 0 || index >= (int)_items.size())
		return;

	_items[index]->label->set_text(text);
	_items[index]->textfield->set_text(text);
}

int RolloutList::selected_item() const
{
	return _selected_item;
}

void RolloutList::clear_selection()
{
	if (_selected_item == -1)
		return;

	_items[_selected_item]->clear_selected();
	_selected_item = -1;
}

void RolloutList::set_selected(int index)
{
	clear_selection();

	if (index < 0 || index >= (int)_items.size())
		return;

	_selected_item = index;
	_items[_selected_item]->set_selected();
}

void RolloutList::set_bold(int index, bool value)
{
	if (index < 0 || index >= (int)_items.size())
		return;

	if (value)
		_items[index]->set_bold();
	else
		_items[index]->clear_bold();
}

int RolloutList::find_index(const RolloutListItemView *search_item) const
{
	int i = 0;
	for (auto &item : _items)
	{
		if (item.get() == search_item)
			return i;
		i++;
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////

RolloutListItemView::RolloutListItemView(RolloutList *init_list) : list(init_list)
{
	style()->set("padding: 3px");
	style()->set("border-radius: 2px");

	label = std::make_shared<LabelBaseView>();
	label->style()->set("font: 12px/18px 'Lato'");
	label->style()->set("color: rgb(230,230,230)");
	add_child(label);

	textfield = std::make_shared<TextFieldBaseView>();
	textfield->set_hidden();
	textfield->style()->set("font: 12px/18px 'Lato'");
	textfield->style()->set("color: rgb(230,230,230)");
	add_child(textfield);

	slots.connect(sig_pointer_release(), [this](PointerEvent &e)
	{
		if (e.target() == textfield)
			return;

		set_focus();
		int index = list->find_index(this);
		if (index == list->selected_item())
		{
			list->sig_selection_clicked()();
			if (list->is_edit_allowed())
				begin_edit();
		}
		else
		{
			list->set_selected(index);
			list->sig_selection_changed()();
		}
	});
	slots.connect(textfield->sig_focus_lost(), [this](FocusChangeEvent &e) { cancel_edit(); });
	slots.connect(textfield->sig_enter_pressed(), [this]() { save_edit(); });
	slots.connect(textfield->sig_key_press(), [this](KeyEvent &e)
	{
		if (e.key() == Key::escape)
		{
			cancel_edit();
		}
	});
}

void RolloutListItemView::set_selected()
{
	style()->set("background-color: rgba(255,255,255,0.3)");
	set_needs_render();
}

void RolloutListItemView::clear_selected()
{
	style()->set("background-color: none");
	set_needs_render();
}

void RolloutListItemView::set_bold()
{
	style()->set("font-weight: 900");
	set_needs_render();
}

void RolloutListItemView::clear_bold()
{
	style()->set("font-weight: normal");
	set_needs_render();
}

void RolloutListItemView::begin_edit()
{
	textfield->set_text(label->text());
	textfield->set_hidden(false);
	textfield->set_focus();
	label->set_hidden(true);
}

void RolloutListItemView::save_edit()
{
	label->set_text(textfield->text());
	label->set_hidden(false);
	textfield->set_hidden(true);

	list->sig_edit_saved()();
}

void RolloutListItemView::cancel_edit()
{
	label->set_hidden(false);
	textfield->set_hidden(true);
}
