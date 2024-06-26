#include "cmrc/cmrc.hpp"

#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"

#include <Core.h>
#include <FileTree.h>
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

std::string GetCodeContent(FileTable& files)
{
	std::stringstream filesContent;
	for (auto& [name, fileData] : files)
	{
		filesContent << fileData.mText << "\n";
	}
	return filesContent.str();
}

std::string GetBracketsContent(FileTable& files)
{
	std::string bracketsContent;
	llvm::raw_string_ostream bracketsStream(bracketsContent);
	llvm::json::OStream bracketsObject(bracketsStream, 4);

	bracketsObject.objectBegin();
	for (auto& [name, fileData] : files)
	{
		bracketsObject.attributeBegin(name);
		bracketsObject.arrayBegin();
		for (auto& [from, to] : fileData.mBracketsTable)
		{
			bracketsObject.objectBegin();
			bracketsObject.attribute("from", from);
			bracketsObject.attribute("to", to);
			bracketsObject.objectEnd();
		}
		bracketsObject.arrayEnd();
		bracketsObject.attributeEnd();
	}
	bracketsObject.objectEnd();

	return bracketsContent;
}

std::string GetDefinitionsContent(SymbolTable& symbolTable)
{
	std::string symbolContent;
	llvm::raw_string_ostream symbolStream(symbolContent);
	llvm::json::OStream symbolObject(symbolStream, 4);

	symbolObject.objectBegin();
	for (auto& [name, definitionLocation] : symbolTable)
	{
		symbolObject.attributeBegin(name);
		symbolObject.objectBegin();
		symbolObject.attribute("filename", definitionLocation.mFilename);
		symbolObject.attribute("type", definitionLocation.mType);
		symbolObject.attribute("from", definitionLocation.mFrom);
		symbolObject.attribute("to", definitionLocation.mTo);
		symbolObject.objectEnd();
		symbolObject.attributeEnd();
	}
	symbolObject.objectEnd();

	return symbolContent;
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

	std::vector<std::string> sourcePathList = commandLineParser.get().getSourcePathList();

	FileTree fileTree(UniversalPath(std::filesystem::current_path().string()).string());
	for (std::string& path : sourcePathList)
	{
		fileTree.AddPath(UniversalLocalPath(path).string());
	}
	std::string fileTreeHTML = fileTree.BuildMarkupTree();
	std::string projectTitle = fileTree.GetRootName();

	ClangTool tool(commandLineParser.get().getCompilations(), sourcePathList);

	FileTable files;
	SymbolTable symbols;
	std::unique_ptr<FrontendActionFactory> factory = std::make_unique<VisualizerFrontendActionFactory>(files, symbols);

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

	std::string output = skeletonContent;
	output = ReplaceAll(output, "<link rel=\"stylesheet\" href=\"styles.css\" />", std::format("<style>\n{}\n</style>\n", stylesContent));
	output = ReplaceAll(output, "<link rel=\"stylesheet\" href=\"theme.css\" />", std::format("<style>\n{}\n</style>\n", themeContent));
	output = ReplaceAll(output, "<script src=\"script.js\"></script>", std::format("<script>\n{}\n</script>\n", jsContent));
	output = ReplaceAll(output, "<div class=\"explorer\"></div>", fileTreeHTML);
	output = ReplaceAll(output, "{PROJECT_TITLE}", projectTitle);
	output = ReplaceAll(output, "{CODE}", GetCodeContent(files));
	output = ReplaceAll(output, "{BRACKETS}", std::format("<script>\nvar brackets =\n{}\n</script>\n", GetBracketsContent(files)));
	output = ReplaceAll(output, "{SYMBOLS}", std::format("<script>\nvar symbols =\n{}\n</script>\n", GetDefinitionsContent(symbols)));

	std::string outputFilename = "Output/Output.html";
	std::error_code error;
	llvm::raw_fd_ostream outFile(outputFilename, error);
	outFile.write(output.c_str(), output.size());
	outFile.close();

	return 0;
}
