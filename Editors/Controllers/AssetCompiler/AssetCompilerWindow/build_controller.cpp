
#include "precomp.h"
#include "build_controller.h"

using namespace uicore;

BuildController::BuildController()
{
	slots.connect(view->project_folder_browse->sig_clicked(), [this] { project_folder_browse_clicked(); });
	slots.connect(view->build_button->sig_clicked(), [this] { build_button_clicked(); });
	slots.connect(view->clean_button->sig_clicked(), [this] { clean_button_clicked(); });
	slots.connect(view->cancel_button->sig_clicked(), [this] { cancel_button_clicked(); });
}

void BuildController::project_folder_browse_clicked()
{
	BrowseFolderDialog dialog(view.get());
	dialog.set_title("Select Project Folder");
	dialog.set_initial_directory(view->project_folder_edit->text());
	if (dialog.show())
		view->project_folder_edit->set_text(dialog.selected_path());
}

void BuildController::build_button_clicked()
{
	create_compiler();
	compiler->build();
}

void BuildController::clean_button_clicked()
{
	create_compiler();
	compiler->clean();
}

void BuildController::cancel_button_clicked()
{
	if (compiler)
		compiler->cancel();
}

void BuildController::create_compiler()
{
	view->clear();
	std::string project_folder = view->project_folder_edit->text();
	compiler = AssetCompiler::create(FilePath::combine(project_folder, "Assets"), FilePath::combine(project_folder, "Resources/Assets"), [this](const CompilerMessage &msg)
	{
		auto msg_copy = msg;
		RunLoop::main_thread_task([=] {
			view->log(msg_copy);
		});
	});
}
