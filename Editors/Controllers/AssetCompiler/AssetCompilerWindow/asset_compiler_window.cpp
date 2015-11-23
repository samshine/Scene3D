
#include "precomp.h"
#include "asset_compiler_window.h"

using namespace uicore;

AssetCompilerWindow::AssetCompilerWindow()
{
	set_title("Scene3D Asset Compiler");
	set_frame_size({ 950.0f, 576.0f });
	set_icon({ "Icons/App/AppIcon-256.png", "Icons/App/AppIcon-16.png" });
	set_root_view(view);

	view->style()->set("background: rgb(240,240,240)");

	slots.connect(view->sig_close(), [this](CloseEvent &e)
	{
		dismiss();
	});

	/*
	AssetCompiler compiler;
	compiler.compile(MapAppModel::instance()->open_filename, [](const CompilerMessage &msg) {});
	*/

}
