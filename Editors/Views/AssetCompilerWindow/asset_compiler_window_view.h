
#pragma once

#include "Views/Theme/theme_views.h"

class AssetCompilerWindowView : public uicore::ColumnView
{
public:
	AssetCompilerWindowView()
	{
		using namespace uicore;

		auto asset_list_row = add_subview<RowView>();
		auto asset_list_label = asset_list_row->add_subview<LabelView>();
		asset_list_edit = asset_list_row->add_subview<TextFieldView>();
		asset_list_browse = asset_list_row->add_subview<ButtonView>();

		auto output_row = add_subview<RowView>();
		auto output_label = output_row->add_subview<LabelView>();
		output_edit = output_row->add_subview<TextFieldView>();
		output_browse = output_row->add_subview<ButtonView>();

		log_scroll = add_subview<ThemeScrollView>();

		auto buttons_row = add_subview<RowView>();
		buttons_row->add_subview<SpacerView>();
		build_button = buttons_row->add_subview<ButtonView>();
		clean_button = buttons_row->add_subview<ButtonView>();
		cancel_button = buttons_row->add_subview<ButtonView>();

		style()->set("background: rgb(240,240,240)");
		style()->set("font: 12px/15px 'Segoe UI'");
		style()->set("padding: 11px");
		asset_list_row->style()->set("margin: 3px 0; flex: none");
		output_row->style()->set("margin: 3px 0; flex: none");
		buttons_row->style()->set("margin: 3px 0; flex: none");
		asset_list_label->style()->set("width: 150px; padding: 3px");
		output_label->style()->set("width: 150px; padding: 3px");
		asset_list_edit->style()->set("flex: auto; border: 1px solid #ddd; background: white; padding: 2px; margin: 0 5px");
		output_edit->style()->set("flex: auto; border: 1px solid #ddd; background: white; padding: 2px; margin: 0 5px");
		asset_list_browse->style()->set("flex: none; border: 1px solid #ddd; padding: 5px 15px;");
		output_browse->style()->set("flex: none; border: 1px solid #ddd; padding: 5px 15px;");
		build_button->style()->set("flex: none; border: 1px solid #ddd; padding: 5px 15px; margin-right: 5px");
		clean_button->style()->set("flex: none; border: 1px solid #ddd; padding: 5px 15px; margin-right: 5px");
		cancel_button->style()->set("flex: none; border: 1px solid #ddd; padding: 5px 15px;");
		asset_list_browse->style("hot")->set("background: #eee");
		output_browse->style("hot")->set("background: #eee");
		build_button->style("hot")->set("background: #eee");
		clean_button->style("hot")->set("background: #eee");
		cancel_button->style("hot")->set("background: #eee");
		log_scroll->style()->set("flex: auto; border: 1px solid #ddd; background: white; padding: 2px; margin: 5px 0");
		log_scroll->content_view()->style()->set("flex-direction: column");

		asset_list_label->set_text("Assets Directory");
		output_label->set_text("Output Directory");
		asset_list_browse->label()->set_text("...");
		output_browse->label()->set_text("...");
		build_button->label()->set_text("Build");
		clean_button->label()->set_text("Clean");
		cancel_button->label()->set_text("Cancel");
	}

	void clear()
	{
		while (!log_scroll->content_view()->subviews().empty())
			log_scroll->content_view()->subviews().back()->remove_from_super();
	}

	void log(const CompilerMessage &msg)
	{
		auto log_text = log_scroll->content_view()->add_subview<uicore::TextBlockView>();
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

	std::shared_ptr<uicore::ScrollView> log_scroll;

	std::shared_ptr<uicore::TextFieldView> asset_list_edit;
	std::shared_ptr<uicore::ButtonView> asset_list_browse;

	std::shared_ptr<uicore::TextFieldView> output_edit;
	std::shared_ptr<uicore::ButtonView> output_browse;

	std::shared_ptr<uicore::ButtonView> build_button;
	std::shared_ptr<uicore::ButtonView> clean_button;
	std::shared_ptr<uicore::ButtonView> cancel_button;
};
