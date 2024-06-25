#pragma once

#include <map>
#include <string>
#include <vector>

class CodeBuilder
{
public:
	CodeBuilder(std::string filename, std::string text);

	std::string BuildCodeCluster(std::map<size_t, size_t>& bracketsTable);

private:
	std::vector<std::string> SplitTextIntoLines(std::string text);
	std::vector<size_t> BuildIndentTable(const std::vector<std::string>& lines);
	void StripLines(std::vector<std::string>& lines);
	std::map<size_t, size_t> BuildBracketsTable(const std::vector<std::string>& lines);
	void FilterShortBrackets(std::map<size_t, size_t>& ranges);
	void StyleMultilineElements(std::vector<std::string>& lines);
	void StyleSystemHeaderLiterals(std::vector<std::string>& lines);
	void FixQuotes(std::vector<std::string>& lines);

private:
	std::string mFilename;
	std::string mText;
};
