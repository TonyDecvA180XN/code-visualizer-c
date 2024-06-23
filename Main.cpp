#include "cmrc/cmrc.hpp"

#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

#include <filesystem>

// Apply a custom category to all command-line options so
// that they are the only ones displayed.
static llvm::cl::OptionCategory VisualizerSpecificCategory("Code Visualizer options");

// CommonOptionsParser declares HelpMessage with a
// description of the common command-line options related to
// the compilation database and input files. It's nice to
// have this help message in all tools.
static llvm::cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);

CMRC_DECLARE(VisualizerResources);

std::map<std::string, std::string> FILES;

class VisualizerASTConsumer : public clang::ASTConsumer
{
public:
	//void HandleTranslationUnit(clang::ASTContext& context) override
	//{
	//}
};

class VisualizerAction : public clang::ASTFrontendAction
{
public:
	std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& compilerInstance, llvm::StringRef filename) override
	{
		return std::make_unique<VisualizerASTConsumer>();
	}

	bool BeginSourceFileAction(clang::CompilerInstance& compilerInstance) override
	{
		clang::SourceManager& sourceManager = compilerInstance.getSourceManager();
		mRewriter.setSourceMgr(sourceManager, compilerInstance.getLangOpts());
		return true;
	}

	void EndSourceFileAction() override
	{
		clang::SourceManager& sourceManager = mRewriter.getSourceMgr();
		clang::FileID currentSourceFileId = sourceManager.getMainFileID();

		std::string sourceFilename = sourceManager.getFileEntryForID(currentSourceFileId)->getName().str();

		clang::RewriteBuffer& rewriteBuffer = mRewriter.getEditBuffer(currentSourceFileId);
		std::string text(rewriteBuffer.begin(), rewriteBuffer.end());

		std::filesystem::path relativePath = std::filesystem::relative(sourceFilename);
		FILES[relativePath.string()] = text;
	}

private:
	clang::Rewriter mRewriter;
};

class VisualizerFrontendActionFactory : public clang::tooling::FrontendActionFactory
{
public:
	std::unique_ptr<clang::FrontendAction> create() override { return std::make_unique<VisualizerAction>(); }
};

std::string ReplaceAll(const std::string& source, const std::string& find, const std::string& replace)
{
	std::string result = source;
	size_t offset = result.find(find);
	while (offset != std::string::npos)
	{
		result.replace(offset, std::string(find).size(), replace);
		offset += replace.size();
		offset = result.find(find, offset);
	}
	return result;
}

int main(int argc, const char** argv)
{
	using namespace clang::tooling;
	llvm::Expected<CommonOptionsParser> commandLineParser = CommonOptionsParser::create(argc, argv, VisualizerSpecificCategory);

	if (!commandLineParser)
	{
		llvm::errs() << commandLineParser.takeError();
		return -1;
	}

	ClangTool tool(commandLineParser.get().getCompilations(), commandLineParser.get().getSourcePathList());

	std::unique_ptr<FrontendActionFactory> factory = std::make_unique<VisualizerFrontendActionFactory>();

	const int status = tool.run(factory.get());

	if (status)
		return status;

	cmrc::embedded_filesystem filesystem = cmrc::VisualizerResources::get_filesystem();

	cmrc::file skeletonPage = filesystem.open("Resources/Skeleton.html");
	std::string skeletonContent(skeletonPage.begin(), skeletonPage.end());

	cmrc::file stylesPage = filesystem.open("Resources/Styles.css");
	std::string stylesContent(stylesPage.begin(), stylesPage.end());

	cmrc::file themePage = filesystem.open("Resources/Theme.css");
	std::string themeContent(themePage.begin(), themePage.end());

	std::string filename = std::filesystem::relative(commandLineParser.get().getSourcePathList()[0]).string();

	std::string output = skeletonContent;
	output = ReplaceAll(output, "<link rel=\"stylesheet\" href=\"styles.css\" />", std::format("<style>\n{}\n</style>\n", stylesContent));
	output = ReplaceAll(output, "<link rel=\"stylesheet\" href=\"theme.css\" />", std::format("<style>\n{}\n</style>\n", themeContent));
	output = ReplaceAll(output, "{CODE}", FILES[filename]);

	std::string outputFilename = "Examples/Output.html";
	std::error_code error;
	llvm::raw_fd_ostream outFile(outputFilename, error);
	outFile.write(output.c_str(), output.size());
	outFile.close();

	return 0;
}
