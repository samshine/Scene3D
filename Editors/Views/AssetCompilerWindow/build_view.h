
#pragma once

#include "Views/Theme/theme_views.h"

class BuildView : public uicore::ColumnView
{
public:
	BuildView()
	{
		using namespace uicore;

		log_scroll = add_child<ThemeScrollView>();

		auto buttons_row = add_child<RowView>();
		buttons_row->add_child<SpacerView>();
		build_button = buttons_row->add_child<ButtonBaseView>();
		clean_button = buttons_row->add_child<ButtonBaseView>();
		cancel_button = buttons_row->add_child<ButtonBaseView>();

		style()->set("padding: 11px; flex: auto");
		buttons_row->style()->set("margin: 3px 0; flex: none");
		build_button->style()->set("flex: none; border: 1px solid #ddd; padding: 5px 15px; margin-right: 5px");
		clean_button->style()->set("flex: none; border: 1px solid #ddd; padding: 5px 15px; margin-right: 5px");
		cancel_button->style()->set("flex: none; border: 1px solid #ddd; padding: 5px 15px;");
		build_button->style("hot")->set("background: #eee");
		clean_button->style("hot")->set("background: #eee");
		cancel_button->style("hot")->set("background: #eee");
		log_scroll->style()->set("flex: auto; border: 1px solid #ddd; background: white; padding: 2px; margin: 5px 0");
		log_scroll->content_view()->style()->set("flex-direction: column");

		build_button->label()->set_text("Build");
		clean_button->label()->set_text("Clean");
		cancel_button->label()->set_text("Cancel");
	}

	void clear()
	{
		for (auto v = log_scroll->content_view()->last_child(); v != nullptr; v = log_scroll->content_view()->last_child())
			v->remove_from_parent();
	}

	void log(const CompilerMessage &msg)
	{
		auto log_text = log_scroll->content_view()->add_child<uicore::TextBlockBaseView>();
		if (!msg.file.empty())
			log_text->add_text(msg.file, "file");
		if (msg.line > 0)
		{
			log_text->add_text("(");
			log_text->add_text(uicore::Text::to_string(msg.line));
			log_text->add_text(")");
		}
		if (!msg.file.empty() || msg.line > 0)
			log_text->add_text(": ");
		switch (msg.type)
		{
		case CompilerMessageType::error:
			log_text->add_text("error: ");
			break;
		case CompilerMessageType::info:
			break;
		case CompilerMessageType::warning:
			log_text->add_text("warning: ");
			break;
		}
		log_text->add_text(msg.text);
	}

	std::shared_ptr<uicore::ScrollBaseView> log_scroll;

	std::shared_ptr<uicore::ButtonBaseView> build_button;
	std::shared_ptr<uicore::ButtonBaseView> clean_button;
	std::shared_ptr<uicore::ButtonBaseView> cancel_button;
};
