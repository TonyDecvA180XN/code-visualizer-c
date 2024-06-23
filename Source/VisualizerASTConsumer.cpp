#include "VisualizerASTConsumer.h"

#include <VisualizerVisitor.h>

VisualizerASTConsumer::VisualizerASTConsumer(clang::Rewriter& rewriter) : mRewriter(rewriter) {}

void VisualizerASTConsumer::HandleTranslationUnit(clang::ASTContext& context)
{
	VisualizerVisitor visitor(context, mRewriter);
	visitor.TraverseDecl(context.getTranslationUnitDecl());
}
