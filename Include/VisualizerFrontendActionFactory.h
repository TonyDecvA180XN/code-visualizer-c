#pragma once

#include "VisualizerAction.h"

#include "clang/Tooling/Tooling.h"

#include <memory>

class VisualizerFrontendActionFactory : public clang::tooling::FrontendActionFactory
{
public:
	VisualizerFrontendActionFactory(FileTable& fileTable);
	std::unique_ptr<clang::FrontendAction> create() override;

private:
	FileTable& mFileTable;
};
