#include "VisualizerAction.h"

#include "clang/Frontend/CompilerInstance.h"

#include <CodeBuilder.h>
#include <VisualizerASTConsumer.h>
#include <filesystem>

extern std::map<std::string, std::string> FILES;

VisualizerAction::VisualizerAction(FileTable& fileTable) : mFileTable(fileTable) {}

std::unique_ptr<clang::ASTConsumer> VisualizerAction::CreateASTConsumer(clang::CompilerInstance& compilerInstance, llvm::StringRef filename)
{
	return std::make_unique<VisualizerASTConsumer>(mRewriter);
}

bool VisualizerAction::BeginSourceFileAction(clang::CompilerInstance& compilerInstance)
{
	clang::SourceManager& sourceManager = compilerInstance.getSourceManager();
	mRewriter.setSourceMgr(sourceManager, compilerInstance.getLangOpts());
	return true;
}

void VisualizerAction::EndSourceFileAction()
{
	clang::SourceManager& sourceManager = mRewriter.getSourceMgr();
	clang::FileID currentSourceFileId = sourceManager.getMainFileID();

	std::string sourceFilename = sourceManager.getFileEntryForID(currentSourceFileId)->getName().str();
	std::string relativePath = UniversalLocalPath(sourceFilename).string();

	clang::RewriteBuffer& rewriteBuffer = mRewriter.getEditBuffer(currentSourceFileId);
	std::string text(rewriteBuffer.begin(), rewriteBuffer.end());

	CodeBuilder codeBuilder(relativePath, text);
	std::string codeCluster = codeBuilder.BuildCodeCluster();

	mFileTable[relativePath] = codeCluster;
}
