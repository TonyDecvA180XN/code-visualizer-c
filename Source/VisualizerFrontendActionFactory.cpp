#include "VisualizerFrontendActionFactory.h"

std::unique_ptr<clang::FrontendAction> VisualizerFrontendActionFactory::create()
{
	return std::make_unique<VisualizerAction>();
}
