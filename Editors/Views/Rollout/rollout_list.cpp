
#include "precomp.h"
#include "rollout_list.h"

using namespace clan;

RolloutList::RolloutList()
{
	style()->set("flex-direction: column");
	style()->set("padding: 5px 0");
}

std::shared_ptr<RolloutListItemView> RolloutList::selection()
{
	for (auto &subview : subviews())
	{
		auto item = std::dynamic_pointer_cast<RolloutListItemView>(subview);
		if (item->selected()) return item;
	}
	return std::shared_ptr<RolloutListItemView>();
}

void RolloutList::clear()
{
	auto s = subviews();
	for (const auto &v : s)
		v->remove_from_super();
}

std::shared_ptr<RolloutListItemView> RolloutList::add_item(const std::string &item_name)
{
	auto item = std::make_shared<RolloutListItemView>(subviews().size());
	item->set_text(item_name);

	add_subview(item);
	return item;
}

/////////////////////////////////////////////////////////////////////////////

RolloutListItemView::RolloutListItemView(size_t index) : index(index)
{
	style()->set("flex-direction: column");
	style()->set("padding: 3px");
	style()->set("border-radius: 2px");

	label = std::make_shared<LabelView>();
	label->style()->set("font: 12px/18px 'Lato'");
	label->style()->set("color: white");
	add_subview(label);

	textfield = std::make_shared<TextFieldView>();
	textfield->set_hidden();
	textfield->style()->set("font: 12px/18px 'Lato'");
	textfield->style()->set("color: white");
	add_subview(textfield);

	slots.connect(label->sig_pointer_release(), [this](PointerEvent &e)
	{
		set_focus();
		if (selected())
		{
			RolloutList *list = dynamic_cast<RolloutList*>(superview());
			if (list)
			{
				list->sig_selection_clicked()();
				if (list->is_edit_allowed())
					begin_edit();
			}
		}
		else
		{
			set_selected(true);
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

std::string RolloutListItemView::text() const
{
	return label->text();
}

void RolloutListItemView::set_text(const std::string &text)
{
	label->set_text(text);
}

void RolloutListItemView::set_bold(bool enable)
{
	if (enable)
		label->style()->set("font-weight: 900");
	else
		label->style()->set("font-weight: normal");
}

bool RolloutListItemView::selected() const
{
	return is_selected;
}

void RolloutListItemView::set_selected(bool value, bool animate_change)
{
	if (is_selected != value)
	{
		if (superview() && value)
		{
			for (auto view : superview()->subviews())
			{
				if (view.get() != this)
					std::dynamic_pointer_cast<RolloutListItemView>(view)->set_selected(false);
			}
		}

		if (!value)
			cancel_edit();

		is_selected = value;

		if (animate_change)
		{
			stop_animations();
			if (value)
				animate(0.0f, 0.3f, [this](float t) { style()->set(string_format("background-color: rgba(255,255,255,%1)", t)); }, 400, Easing::easeinout);
			else
				animate(0.3f, 0.0f, [this](float t) { style()->set(string_format("background-color: rgba(255,255,255,%1)", t)); }, 400, Easing::easeinout, [this]() { style()->set("background-color: none"); });
		}
		else
		{
			if (value)
				style()->set("background-color: rgba(255,255,255,0.3)");
			else
				style()->set("background-color: none");
		}

		if (value)
			static_cast<RolloutList*>(superview())->sig_selection_changed()();
	}
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

	static_cast<RolloutList*>(superview())->sig_edit_saved()();
}

void RolloutListItemView::cancel_edit()
{
	label->set_hidden(false);
	textfield->set_hidden(true);
}
