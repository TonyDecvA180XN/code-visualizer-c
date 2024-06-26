#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

class MarkupTree;

class MarkupTreeNode
{
private:
	class MarkupTreeNodeConstructToken
	{
	};

public:
	friend class MarkupTree;
	MarkupTreeNode(MarkupTreeNodeConstructToken, std::string tag, const bool isSingle = false);
	MarkupTreeNode(const MarkupTreeNode& other) = delete;
	MarkupTreeNode(MarkupTreeNode&& other) noexcept = default;

	void SetText(std::string text);
	MarkupTreeNode& AppendChild(std::string tag, const bool isSingle = false);
	void AddAttribute(std::string name);
	void AddAttribute(std::string name, std::string value);

	std::string ToString(const bool prettyPrint = true) const;
	std::string BuildOpeningTag() const;
	std::string BuildClosingTag() const;

private:
	MarkupTreeNode(std::string tag, const bool isSingle = false);

	std::vector<std::string> ToStrings() const;

	std::vector<std::string> GetInnerContent() const;

private:
	std::string mTag;
	bool mIsSingle;
	std::optional<std::string> mText;
	std::vector<MarkupTreeNode> mChildren;
	std::map<std::string, std::optional<std::string>> mAttributes;
};

class MarkupTree
{
public:
	MarkupTree(std::string tag);

	MarkupTreeNode& GetRoot();

	std::string ToString(const bool prettyPrint = true) const;

private:
	MarkupTreeNode mRoot;
};
