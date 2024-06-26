#include "VisualizerFrontendActionFactory.h"

#include <VisualizerAction.h>

VisualizerFrontendActionFactory::VisualizerFrontendActionFactory(FileTable& fileTable, SymbolTable& symbolTable) :
	mFileTable(fileTable),
	mSymbolTable(symbolTable)
{
}

std::unique_ptr<clang::FrontendAction> VisualizerFrontendActionFactory::create()
{
	return std::make_unique<VisualizerAction>(mFileTable, mSymbolTable);
}
