
#include "precomp.h"
#include "header_view.h"
#include "Model/app_model.h"

using namespace clan;

HeaderView::HeaderView()
{
	box_style.set_layout_hbox();
	box_style.set_flex(0.0f, 0.0f);
	box_style.set_background(Colorf(240, 240, 240));
	box_style.set_background_gradient_to_right(Colorf(15, 50, 77), Colorf(95, 128, 146));
	box_style.set_padding(5.0f, 2.0f);
	box_style.set_border(Colorf(159, 184, 194), 0.0f, 0.0f, 0.0f, 1.0f);

	create_button("Load", bind_member(this, &HeaderView::on_load));
	create_button("Save", bind_member(this, &HeaderView::on_save));
	create_button("Save As", bind_member(this, &HeaderView::on_save_as));

	auto spacer = std::make_shared<View>();
	spacer->box_style.set_flex(1.0f, 1.0f);
	add_subview(spacer);

	create_button("Change Model", bind_member(this, &HeaderView::on_change_model));

	create_button("Animations", bind_member(this, &HeaderView::on_options));
}

void HeaderView::on_load()
{
	OpenFileDialog dialog(this);
	dialog.set_title("Select Model");
	dialog.add_filter("Model description files (*.modeldesc)", "*.modeldesc", true);
	dialog.add_filter("All files (*.*)", "*.*", false);
	dialog.set_filename(AppModel::instance()->open_filename);
	if (dialog.show())
	{
		AppModel::instance()->open(dialog.get_filename());
	}
}

void HeaderView::on_save()
{
	if (AppModel::instance()->open_filename.empty())
	{
		on_save_as();
	}
	else
	{
		AppModel::instance()->save(AppModel::instance()->open_filename);
	}
}

void HeaderView::on_save_as()
{
	SaveFileDialog dialog(this);
	dialog.set_title("Save Model Description");
	dialog.add_filter("Model description files (*.modeldesc)", "*.modeldesc", true);
	dialog.add_filter("All files (*.*)", "*.*", false);
	dialog.set_filename(AppModel::instance()->open_filename);
	if (dialog.show())
	{
		std::string filename = dialog.get_filename();
		if (PathHelp::get_extension(filename).empty())
			filename += ".modeldesc";
		AppModel::instance()->save(filename);
	}
}

void HeaderView::on_change_model()
{
	OpenFileDialog dialog(this);
	dialog.set_title("Select Model");
	dialog.add_filter("Autodesk FBX files (*.fbx)", "*.fbx", true);
	dialog.add_filter("All files (*.*)", "*.*", false);
	dialog.set_filename(AppModel::instance()->desc.fbx_filename);
	if (dialog.show())
	{
		AppModel::instance()->set_fbx_model(dialog.get_filename());
	}
}

void HeaderView::on_options()
{
}

void HeaderView::create_button(const std::string &text, std::function<void()> click)
{
	auto button = std::make_shared<ButtonView>();
	button->box_style.set_flex(0.0f, 0.0f);
	button->box_style.set_margin(5.0f);
	//button->box_style.set_background(Colorf(240, 240, 240));
	//button->box_style.set_border(Colorf(150, 150, 150), 1.0f);
	//button->box_style.set_border_radius(3.0f);
	button->box_style.set_padding(15.0f, 5.0f);
	button->label()->set_text(StringHelp::text_to_upper(text));
	button->label()->text_style().set_font("Lato", 12, 1.4f * 13);
	button->label()->text_style().set_color(Colorf(255, 255, 255));
	slots.connect(button->sig_pointer_release(EventUIPhase::bubbling), [=](PointerEvent &e) { click(); e.stop_propagation(); });
	add_subview(button);
}
