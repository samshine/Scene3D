
#include "precomp.h"
#include "rollout_list.h"

using namespace clan;

RolloutList::RolloutList()
{
	box_style.set_layout_vbox();
	//box_style.set_border(Colorf(200, 200, 200), 1.0f);
	//box_style.set_background(Colorf(240, 240, 240));
	box_style.set_padding(0.0f, 5.0f);
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

std::shared_ptr<RolloutListItemView> RolloutList::add_animation(const std::string &anim_name)
{
	auto item = std::make_shared<RolloutListItemView>(subviews().size());
	item->set_text(anim_name);

	add_subview(item);
	return item;
}

/////////////////////////////////////////////////////////////////////////////

RolloutListItemView::RolloutListItemView(size_t index) : index(index)
{
	box_style.set_layout_vbox();
	box_style.set_padding(3.0f);
	box_style.set_border_radius(2.0f);

	FontDescription font_desc("Lato");
	font_desc.set_height(12);
	font_desc.set_line_height(1.4f * 13);

	label = std::make_shared<LabelView>();
	label->set_font(Font::resource(UIThread::get_resource_canvas(), font_desc, UIThread::get_resources()));
	label->set_text_color(Colorf(255, 255, 255));
	add_subview(label);

	textfield = std::make_shared<TextFieldView>();
	textfield->set_hidden();
	textfield->set_font(Font::resource(UIThread::get_resource_canvas(), font_desc, UIThread::get_resources()));
	textfield->set_text_color(Colorf(255, 255, 255));
	add_subview(textfield);

	slots.connect(label->sig_pointer_release(), [this](PointerEvent &e)
	{
		set_focus();
		if (selected())
			begin_edit();
		else
			set_selected(true);
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
				animate(0.0f, 255.0f, [this](float t) { box_style.set_background(Colorf(255, 255, 255, (int)(t * 0.3)));  }, 400, Easing::easeinout);
			else
				animate(255.0f, 0.0f, [this](float t) { box_style.set_background(Colorf(255, 255, 255, (int)(t * 0.3))); }, 400, Easing::easeinout, [this]() { box_style.set_background_none(); });
		}
		else
		{
			if (value)
				box_style.set_background(Colorf(255, 255, 255, 76));
			else
				box_style.set_background_none();
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
