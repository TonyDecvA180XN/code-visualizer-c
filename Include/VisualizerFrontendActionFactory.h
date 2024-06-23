#pragma once

#include "VisualizerAction.h"

#include "clang/Tooling/Tooling.h"

#include <memory>

class VisualizerFrontendActionFactory : public clang::tooling::FrontendActionFactory
{
public:
	std::unique_ptr<clang::FrontendAction> create() override;
};
