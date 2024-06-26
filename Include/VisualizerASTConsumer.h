#pragma once

#include "clang/AST/ASTConsumer.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include <Core.h>

class VisualizerASTConsumer : public clang::ASTConsumer
{
public:
	VisualizerASTConsumer(clang::Rewriter& rewriter, SymbolTable& symbolTable);
	void HandleTranslationUnit(clang::ASTContext& context) override;

private:
	clang::Rewriter& mRewriter;
	SymbolTable& mSymbolTable;
};
