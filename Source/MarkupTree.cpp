#include "MarkupTree.h"

#include <algorithm>
#include <format>
#include <numeric>

constexpr size_t INDENT_SIZE = 2;

MarkupTreeNode::MarkupTreeNode(std::string tag, const bool isSingle) : mTag(tag), mIsSingle(isSingle), mText(std::nullopt), mChildren(), mAttributes()
{
}

MarkupTreeNode::MarkupTreeNode(MarkupTreeNodeConstructToken, std::string tag, const bool isSingle) : MarkupTreeNode(tag, isSingle) {}

void MarkupTreeNode::SetText(std::string text)
{
	mText = std::make_optional(text);
}

MarkupTreeNode& MarkupTreeNode::AppendChild(std::string tag, const bool isSingle)
{
	mChildren.emplace_back(MarkupTreeNodeConstructToken(), tag, isSingle);
	return mChildren.back();
}

void MarkupTreeNode::AddAttribute(std::string name)
{
	mAttributes[name] = std::nullopt;
}

void MarkupTreeNode::AddAttribute(std::string name, std::string value)
{
	mAttributes[name] = std::make_optional(value);
}

std::string MarkupTreeNode::ToString(const bool prettyPrint) const
{
	std::vector lines = ToStrings();
	return std::accumulate(
		lines.begin(),
		lines.end(),
		std::string(),
		[prettyPrint](const std::string& a, const std::string& b) { return a + b + (prettyPrint ? "\n" : ""); });
}

std::vector<std::string> MarkupTreeNode::ToStrings() const
{
	std::string openingTag = BuildOpeningTag();
	if (mIsSingle)
	{
		return {openingTag};
	}
	std::vector<std::string> content = GetInnerContent();
	std::string closingTag = BuildClosingTag();
	if (content.size() > 1)
	{
		std::vector<std::string> result;
		result.emplace_back(openingTag);
		std::transform(
			content.begin(),
			content.end(),
			std::back_inserter(result),
			[](const std::string& line) { return std::string(INDENT_SIZE, ' ') + line; });
		result.emplace_back(closingTag);
		return result;
	}
	else
	{
		return {std::format("{}{}{}", openingTag, content.empty() ? "" : content.front(), content.empty() && mTag == "input" ? "" : closingTag)};
	}
}

std::string MarkupTreeNode::BuildOpeningTag() const
{
	std::string attributes;
	for (auto& [name, value] : mAttributes)
	{
		if (value.has_value())
		{
			attributes += std::format(" {}=\"{}\"", name, value.value());
		}
		else
		{
			attributes += std::format(" {}", name);
		}
	}

	return std::format("<{}{}>", mTag, attributes);
}

std::string MarkupTreeNode::BuildClosingTag() const
{
	return std::format("</{}>", mTag);
}

std::vector<std::string> MarkupTreeNode::GetInnerContent() const
{
	//assert(mText.has_value() == mChildren.empty());

	std::vector<std::string> innerLines;
	for (const MarkupTreeNode& child : mChildren)
	{
		std::vector<std::string> childLines = child.ToStrings();
		innerLines.reserve(innerLines.size() + childLines.size());
		std::copy(childLines.begin(), childLines.end(), std::back_inserter(innerLines));
	}

	if (mText.has_value())
	{
		innerLines.emplace_back(mText.value());
	}
	return innerLines;
}

MarkupTree::MarkupTree(std::string tag) : mRoot(tag, false) {}

MarkupTreeNode& MarkupTree::GetRoot()
{
	return mRoot;
}

std::string MarkupTree::ToString(const bool prettyPrint) const
{
	return mRoot.ToString(prettyPrint);
}
