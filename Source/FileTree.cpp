#include "FileTree.h"

#include <MarkupTree.h>
#include <queue>

FileTree::FileTree(std::string name)
{
	mRoot.mName = name;
}

std::string FileTree::GetRootName() const
{
	return mRoot.mName;
}

void FileTree::AddPath(std::string filename)
{
	std::filesystem::path path = std::filesystem::path(filename).relative_path().make_preferred();
	Node* current = &mRoot;
	for (auto it = path.begin(); it != path.end(); ++it)
	{
		std::string element = it->string();
		if (!current->mChildren.contains(element))
		{
			current->mChildren[element].mName = element;
		}
		current = &(current->mChildren[element]);
	}
}

std::string FileTree::BuildMarkupTree() const
{
	MarkupTree explorer("div");
	MarkupTreeNode& root = explorer.GetRoot();
	root.AddAttribute("class", "explorer");

	std::queue<const Node*> fileQueue;
	std::queue<MarkupTreeNode*> tagQueue;

	// do not put project root into tree
	for (auto& [name, node] : mRoot.mChildren)
	{
		fileQueue.push(&node);
		tagQueue.push(&root);
	}

	while (!fileQueue.empty())
	{
		const Node* currentFile = fileQueue.front();
		MarkupTreeNode* currentTag = tagQueue.front();

		if (currentFile->mChildren.empty()) // is a file
		{
			MarkupTreeNode& tag = currentTag->AppendChild("a");
			tag.AddAttribute("class", "file");
			tag.SetText(currentFile->mName);
		}
		else // is a folder
		{
			MarkupTreeNode& tag = currentTag->AppendChild("details");
			tag.AddAttribute("class", "folder");
			tag.AddAttribute("open");

			MarkupTreeNode& summary = tag.AppendChild("summary");
			summary.SetText(currentFile->mName);

			MarkupTreeNode& inner = tag.AppendChild("div");
			inner.AddAttribute("class", "filetree-indent");

			for (auto& [name, node] : currentFile->mChildren)
			{
				fileQueue.push(&node);
				tagQueue.push(&inner);
			}
		}

		fileQueue.pop();
		tagQueue.pop();
	}

	return explorer.ToString(true);
}
