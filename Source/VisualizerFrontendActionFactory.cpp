#include "VisualizerFrontendActionFactory.h"
#include <VisualizerAction.h>

VisualizerFrontendActionFactory::VisualizerFrontendActionFactory(FileTable& fileTable) : mFileTable(fileTable) {}

std::unique_ptr<clang::FrontendAction> VisualizerFrontendActionFactory::create()
{
	return std::make_unique<VisualizerAction>(mFileTable);
}
