#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

#include "cmrc/cmrc.hpp"

// Apply a custom category to all command-line options so
// that they are the only ones displayed.
static llvm::cl::OptionCategory VisualizerSpecificCategory("Code Visualizer options");

// CommonOptionsParser declares HelpMessage with a
// description of the common command-line options related to
// the compilation database and input files. It's nice to
// have this help message in all tools.
static llvm::cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);

CMRC_DECLARE(VisualizerResources);

int main(int argc, const char** argv)
{
	using namespace clang::tooling;
	llvm::Expected<CommonOptionsParser> commandLineParser = CommonOptionsParser::create(argc, argv, VisualizerSpecificCategory);

	if (!commandLineParser)
	{
		llvm::errs() << commandLineParser.takeError();
		return -1;
	}

	cmrc::embedded_filesystem filesystem = cmrc::VisualizerResources::get_filesystem();
	cmrc::file responsePage = filesystem.open("Templates/ResponsePage.html");
	std::string content(responsePage.begin(), responsePage.end());

	ClangTool Tool(commandLineParser.get().getCompilations(), commandLineParser.get().getSourcePathList());
	return Tool.run(newFrontendActionFactory<clang::SyntaxOnlyAction>().get());
}
