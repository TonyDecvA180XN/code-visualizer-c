#pragma once

#include <filesystem>
#include <format>
#include <map>
#include <string>

struct FileData
{
	std::string mText;
	std::map<size_t, size_t> mBracketsTable;
};

struct DefinitionLocation
{
	std::string mFilename;
	std::string mType;
	size_t mFrom, mTo;
};

using FileTable = std::map<std::string, FileData>;
using SymbolTable = std::map<std::string, DefinitionLocation>;

inline std::string ReplaceAll(const std::string& source, const std::string& find, const std::string& replace)
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

inline std::filesystem::path UniversalPath(std::filesystem::path path)
{
	return ReplaceAll(path.string(), "\\", "/");
}

inline std::filesystem::path UniversalPath(std::string filename)
{
	std::filesystem::path path(filename);
	return UniversalPath(path);
}

inline std::filesystem::path UniversalLocalPath(std::filesystem::path filename)
{
	return UniversalPath(std::filesystem::relative(filename, std::filesystem::current_path()));
}

namespace Quote
{
	// Source HTML
	//   "     _
	//   "     31
	// &quot;  31
	// &quot;  "
	constexpr char SPECIAL_CHARACTER = 31;
	const std::string SPECIAL_STRING(1, SPECIAL_CHARACTER);
	const std::string TRUE_QUOTE = "&quot;";
	const std::string TAG_QUOTE = "\"";

	inline std::string FrameInQuotes(const std::string& str)
	{
		return std::format("{}{}{}", SPECIAL_CHARACTER, str, SPECIAL_CHARACTER);
	}

	inline std::string MakeOpenSpan(const std::string& spanClass)
	{
		return std::format("<span class={}>", FrameInQuotes(spanClass));
	}

	inline std::string MakeSpan(const std::string& spanClass, const std::string& body)
	{
		return std::format("<span class={}>{}</span>", FrameInQuotes(spanClass), body);
	}
} //namespace Quote
