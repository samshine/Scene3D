
#include "precomp.h"
#include "asset_compiler_window.h"

using namespace uicore;

AssetCompilerWindow::AssetCompilerWindow()
{
	set_title("Scene3D Asset Compiler");
	set_frame_size({ 750.0f, 476.0f });
	set_icon({ "Icons/App/AppIcon-256.png", "Icons/App/AppIcon-16.png" });
	set_root_view(view);

	slots.connect(view->sig_close(), [this](CloseEvent *)
	{
		dismiss();
	});

	slots.connect(view->project_folder_browse->sig_clicked(), [this] { project_folder_browse_clicked(); });
	slots.connect(view->build_button->sig_clicked(), [this] { build_button_clicked(); });
	slots.connect(view->clean_button->sig_clicked(), [this] { clean_button_clicked(); });
	slots.connect(view->cancel_button->sig_clicked(), [this] { cancel_button_clicked(); });
}

void AssetCompilerWindow::project_folder_browse_clicked()
{
	BrowseFolderDialog dialog(view.get());
	dialog.set_title("Select Project Folder");
	dialog.set_initial_directory(view->project_folder_edit->text());
	if (dialog.show())
		view->project_folder_edit->set_text(dialog.selected_path());
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
	std::string project_folder = view->project_folder_edit->text();
	compiler = AssetCompiler::create(PathHelp::combine(project_folder, "Assets"), PathHelp::combine(project_folder, "Resources/Assets"), [this](const CompilerMessage &msg)
	{
		auto msg_copy = msg;
		RunLoop::main_thread_task([=]{
			view->log(msg_copy);
		});
	});
}
