#include "CodeBuilder.h"

#include "algorithm"
#include "sstream"

#include <Core.h>
#include <MarkupTree.h>
#include <cassert>
#include <regex>

CodeBuilder::CodeBuilder(std::string filename, std::string text) : mFilename(filename), mText(text) {}

std::string CodeBuilder::BuildCodeCluster()
{
	std::vector<std::string> lines = SplitTextIntoLines(mText);
	std::vector<size_t> indents = BuildIndentTable(lines);
	StripLines(lines);

	std::map<size_t, size_t> brackets = BuildBracketsTable(lines);
	FilterShortBrackets(brackets);
	StyleSystemHeaderLiterals(lines);
	StyleMultilineElements(lines);
	FixQuotes(lines);

	MarkupTree cluster("div");
	MarkupTreeNode& clusterRoot = cluster.GetRoot();
	clusterRoot.AddAttribute("class", "code-content hidden");
	clusterRoot.AddAttribute("id", mFilename);

	for (size_t i = 0; i != lines.size(); ++i)
	{
		MarkupTreeNode& line = clusterRoot.AppendChild("p");

		MarkupTreeNode& indent = line.AppendChild("span");
		indent.AddAttribute("class", "indent");
		indent.AddAttribute("level", std::format("{}", indents[i]));

		MarkupTreeNode& content = line.AppendChild("span");
		content.AddAttribute("class", "code");
		content.SetText(lines[i]);
	}

	return cluster.ToString();
}

std::vector<std::string> CodeBuilder::SplitTextIntoLines(std::string text)
{
	std::stringstream stream(text);
	std::vector<std::string> lines;
	while (!stream.eof())
	{
		std::string line;
		std::getline(stream, line);
		lines.emplace_back(std::move(line));
	}
	return lines;
}

std::vector<size_t> CodeBuilder::BuildIndentTable(const std::vector<std::string>& lines)
{
	std::vector<size_t> table;
	table.reserve(lines.size());
	std::transform(
		lines.begin(),
		lines.end(),
		std::back_inserter(table),
		[](const std::string& line)
		{
			size_t indent = line.find_first_not_of(" \t");
			if (indent == std::string::npos || indent == 0)
			{
				return 0ULL;
			}
			if (line.front() == ' ')
				return indent / 4;
			else
				return indent;
		});
	return table;
}

void CodeBuilder::StripLines(std::vector<std::string>& lines)
{
	std::for_each(
		lines.begin(),
		lines.end(),
		[](std::string& line)
		{
			const size_t home = line.find_first_not_of(" \t");
			if (home != std::string::npos)
			{
				line.erase(0, home);
			}
			const size_t end = line.find_last_not_of(" \t\r\n") + 1;
			line.erase(end, line.size());
		});
}

std::map<size_t, size_t> CodeBuilder::BuildBracketsTable(const std::vector<std::string>& lines)
{
	std::map<size_t, size_t> table;
	std::vector<size_t> opened;
	for (size_t lineIdx = 0; lineIdx != lines.size(); ++lineIdx)
	{
		const std::string& line = lines[lineIdx];
		for (size_t colIdx = 0; colIdx != line.size(); ++colIdx)
		{
			if (line[colIdx] == '{')
			{
				opened.emplace_back(lineIdx);
			}
			else if (line[colIdx] == '}')
			{
				table[opened.back()] = lineIdx;
				opened.pop_back();
			}
		}
	}
	return table;
}

void CodeBuilder::FilterShortBrackets(std::map<size_t, size_t>& ranges)
{
	std::erase_if(ranges, [](const std::pair<size_t, size_t>& range) { return range.second == range.first; });
}

void CodeBuilder::StyleMultilineElements(std::vector<std::string>& lines)
{
	using namespace Quote;

	bool isOpenComment = false;
	bool isOpenLiteral = false;
	for (auto lineIt = lines.begin(); lineIt != lines.end(); ++lineIt)
	{
		std::string& line = *lineIt;

		if (size_t directiveStart = line.find('#'); directiveStart != std::string::npos)
		{
			size_t directiveEnd = line.find(' ', directiveStart);
			std::string directive = line.substr(directiveStart, directiveEnd);
			line.replace(directiveStart, directive.size(), MakeSpan("code-directive", directive));
		}

		if (size_t commentStart = line.find("//"); commentStart != std::string::npos)
		{
			std::string comment = line.substr(commentStart);
			line.replace(commentStart, comment.size(), MakeSpan("code-comment", comment));
		}

		size_t commentSearchOffset = 0;
		size_t commentStart = isOpenComment ? 0 : line.find("/*", commentSearchOffset);
		while (commentStart != std::string::npos)
		{
			size_t commentEnd = line.find("*/", commentSearchOffset);
			std::string comment =
				line.substr(commentStart, commentEnd == std::string::npos ? std::string::npos : commentEnd + std::string("*/").size() - commentStart);
			std::string commentSpan = MakeSpan("code-comment", comment);
			line.replace(commentStart, comment.size(), commentSpan);
			commentSearchOffset = commentStart + commentSpan.size();
			isOpenComment = commentEnd == std::string::npos;

			commentStart = line.find("/*", commentSearchOffset);
		}
	}
}

void CodeBuilder::StyleSystemHeaderLiterals(std::vector<std::string>& lines)
{
	// \\w is alpha char, \\/ is a forward slash, \\\\ - backslash, \\. is a period char
	const std::regex includeRegex("#include ([<\"])([\\w\\/\\\\\\.]+)[>\"]");
	std::for_each(
		lines.begin(),
		lines.end(),
		[&includeRegex](std::string& line)
		{
			// do not run regex on each line unless it is very likely an include directive present
			if (line.find("#include") != std::string::npos)
			{
				std::smatch regexMatch;
				if (std::regex_match(line, regexMatch, includeRegex))
				{
					assert(regexMatch.size() == 3);
					std::string encloser = regexMatch[1], content = regexMatch[2];

					std::string filename;
					if (encloser == "<")
					{
						filename = std::format("&lt;{}&gt;", content);
					}
					else
					{
						filename = std::format("&quot;{}&quot;", content);
					}
					line = std::format("#include {}", Quote::MakeSpan("code-literal-str", filename));
				}
			}
		});
}

void CodeBuilder::FixQuotes(std::vector<std::string>& lines)
{
	std::for_each(
		lines.begin(),
		lines.end(),
		[](std::string& line)
		{
			using namespace Quote;
			line = ReplaceAll(line, TAG_QUOTE, TRUE_QUOTE);
			line = ReplaceAll(line, SPECIAL_STRING, TAG_QUOTE);
		});
}
