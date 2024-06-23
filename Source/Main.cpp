#include "cmrc/cmrc.hpp"

#include "clang/Tooling/CommonOptionsParser.h"

#include <Core.h>
#include <VisualizerFrontendActionFactory.h>
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

int main(int argc, const char** argv)
{
	using namespace clang::tooling;
	llvm::Expected<CommonOptionsParser> commandLineParser = CommonOptionsParser::create(argc, argv, VisualizerSpecificCategory);

	if (!commandLineParser)
	{
		llvm::errs() << commandLineParser.takeError();
		return -1;
	}

	FileTable files;

	ClangTool tool(commandLineParser.get().getCompilations(), commandLineParser.get().getSourcePathList());

	std::unique_ptr<FrontendActionFactory> factory = std::make_unique<VisualizerFrontendActionFactory>(files);

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

	cmrc::file jsPage = filesystem.open("Resources/Script.js");
	std::string jsContent(jsPage.begin(), jsPage.end());

	std::string filename = std::filesystem::relative(commandLineParser.get().getSourcePathList()[0]).string();

	std::string output = skeletonContent;
	output = ReplaceAll(output, "<link rel=\"stylesheet\" href=\"styles.css\" />", std::format("<style>\n{}\n</style>\n", stylesContent));
	output = ReplaceAll(output, "<link rel=\"stylesheet\" href=\"theme.css\" />", std::format("<style>\n{}\n</style>\n", themeContent));
	output = ReplaceAll(output, "<script src=\"script.js\"></script>", std::format("<script>\n{}\n</script>\n", jsContent));
	output = ReplaceAll(output, "{CODE}", files[filename]);

	std::string outputFilename = "Output/Output.html";
	std::error_code error;
	llvm::raw_fd_ostream outFile(outputFilename, error);
	outFile.write(output.c_str(), output.size());
	outFile.close();

	return 0;
}
