#pragma once

#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/FrontendActions.h"

#include <memory>

class VisualizerAction : public clang::ASTFrontendAction
{
public:
	std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& compilerInstance, llvm::StringRef filename) override;

	bool BeginSourceFileAction(clang::CompilerInstance& compilerInstance) override;

	void EndSourceFileAction() override;

private:
	clang::Rewriter mRewriter;
};
