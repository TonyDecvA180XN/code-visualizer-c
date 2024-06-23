#pragma once

#include "clang/AST/ASTConsumer.h"
#include "clang/Rewrite/Core/Rewriter.h"

class VisualizerASTConsumer : public clang::ASTConsumer
{
public:
	VisualizerASTConsumer(clang::Rewriter& rewriter);
	void HandleTranslationUnit(clang::ASTContext& context) override;

private:
	clang::Rewriter& mRewriter;
};
