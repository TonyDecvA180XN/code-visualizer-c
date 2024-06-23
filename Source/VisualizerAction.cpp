#include "VisualizerAction.h"

#include "clang/Frontend/CompilerInstance.h"

#include <VisualizerASTConsumer.h>
#include <filesystem>

extern std::map<std::string, std::string> FILES;

VisualizerAction::VisualizerAction(FileTable& fileTable) : mFileTable(fileTable) {}

std::unique_ptr<clang::ASTConsumer> VisualizerAction::CreateASTConsumer(clang::CompilerInstance& compilerInstance, llvm::StringRef filename)
{
	return std::make_unique<VisualizerASTConsumer>();
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

	clang::RewriteBuffer& rewriteBuffer = mRewriter.getEditBuffer(currentSourceFileId);
	std::string text(rewriteBuffer.begin(), rewriteBuffer.end());

	std::filesystem::path relativePath = std::filesystem::relative(sourceFilename);
	mFileTable[relativePath.string()] = text;
}
