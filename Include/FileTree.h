#pragma once
#include <filesystem>
#include <map>
#include <string>

class FileTree
{
public:
	struct Node
	{
		std::string mName;
		std::map<std::string, Node> mChildren;
	};

	FileTree(std::string name);

	std::string GetRootName() const;
	void AddPath(std::string filename);
	std::string BuildMarkupTree() const;

private:
	Node mRoot;
};
