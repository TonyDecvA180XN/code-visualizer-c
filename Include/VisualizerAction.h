#pragma once

#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/FrontendActions.h"

#include <memory>
#include <Core.h>

class VisualizerAction : public clang::ASTFrontendAction
{
public:
	VisualizerAction(FileTable& fileTable);

	std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& compilerInstance, llvm::StringRef filename) override;

	bool BeginSourceFileAction(clang::CompilerInstance& compilerInstance) override;

	void EndSourceFileAction() override;

private:
	clang::Rewriter mRewriter;
	FileTable& mFileTable;
};
