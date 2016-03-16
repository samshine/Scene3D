
#include "precomp.h"
#include "asset_compiler_window.h"

using namespace uicore;

AssetCompilerWindow::AssetCompilerWindow()
{
	set_title("Scene3D Asset Compiler");
	set_frame_size({ 750.0f, 476.0f });
	set_icon({ "Icons/App/AppIcon-256.png", "Icons/App/AppIcon-16.png" });
	set_root_view(view);

	slots.connect(view->sig_close(), [this](CloseEvent &e)
	{
		dismiss();
	});

	view->asset_list_browse->func_clicked() = [this] { asset_list_browse_clicked(); };
	view->output_browse->func_clicked() = [this] { output_browse_clicked(); };
	view->build_button->func_clicked() = [this] { build_button_clicked(); };
	view->clean_button->func_clicked() = [this] { clean_button_clicked(); };
	view->cancel_button->func_clicked() = [this] { cancel_button_clicked(); };
}

void AssetCompilerWindow::asset_list_browse_clicked()
{
	BrowseFolderDialog dialog(view.get());
	dialog.set_title("Select Assets Base Folder");
	dialog.set_initial_directory(view->asset_list_edit->text());
	if (dialog.show())
		view->asset_list_edit->set_text(dialog.selected_path());
}

void AssetCompilerWindow::output_browse_clicked()
{
	BrowseFolderDialog dialog(view.get());
	dialog.set_title("Select Output Folder");
	dialog.set_initial_directory(view->output_edit->text());
	if (dialog.show())
		view->output_edit->set_text(dialog.selected_path());
}

void AssetCompilerWindow::build_button_clicked()
{
	create_compiler();
	compiler->build();
}

void AssetCompilerWindow::clean_button_clicked()
{
	create_compiler();
	compiler->clean();
}

void AssetCompilerWindow::cancel_button_clicked()
{
	if (compiler)
		compiler->cancel();
}

void AssetCompilerWindow::create_compiler()
{
	view->clear();
	compiler = AssetCompiler::create(view->asset_list_edit->text(), view->output_edit->text(), [this](const CompilerMessage &msg)
	{
		auto msg_copy = msg;
		RunLoop::main_thread_task([=]{
			view->log(msg_copy);
		});
	});
}
