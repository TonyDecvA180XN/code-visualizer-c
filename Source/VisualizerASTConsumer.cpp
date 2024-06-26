#include "VisualizerASTConsumer.h"

#include <VisualizerVisitor.h>

VisualizerASTConsumer::VisualizerASTConsumer(clang::Rewriter& rewriter, SymbolTable& symbolTable) : mRewriter(rewriter), mSymbolTable(symbolTable) {}

void VisualizerASTConsumer::HandleTranslationUnit(clang::ASTContext& context)
{
	VisualizerVisitor visitor(context, mRewriter, mSymbolTable);
	visitor.TraverseDecl(context.getTranslationUnitDecl());
}
