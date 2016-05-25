
#include "precomp.h"
#include "build_controller.h"
#include "Model/AssetFolder/asset_folder_model.h"

using namespace uicore;

BuildController::BuildController()
{
	slots.connect(view->build_button->sig_clicked(), [this] { build_button_clicked(); });
	slots.connect(view->clean_button->sig_clicked(), [this] { clean_button_clicked(); });
	slots.connect(view->cancel_button->sig_clicked(), [this] { cancel_button_clicked(); });
}

void BuildController::build_button_clicked()
{
	if (AssetFolderModel::instance()->project_folder.empty())
		return;

	create_compiler();
	compiler->build();
}

void BuildController::clean_button_clicked()
{
	if (AssetFolderModel::instance()->project_folder.empty())
		return;

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
	compiler = AssetCompiler::create(AssetFolderModel::instance()->assets_folder, AssetFolderModel::instance()->output_folder, [this](const CompilerMessage &msg)
	{
		auto msg_copy = msg;
		RunLoop::main_thread_task([=] {
			view->log(msg_copy);
		});
	});
}
