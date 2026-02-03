#pragma once

#include <imgui.h>
#include <vector>
#include <string>

class FileTreeView {
public:
	struct Node {
		int				 id = -1;
		int				 parent_id = -1; // -1 for root
		bool			 is_dir = true;
		std::string		 name;
		std::vector<int> children; // ids
	};

	// For now: dummy model injection
	void setModel(std::vector<Node> nodes, int root_id);

	// View state
	int	 selectedId() const { return selected_id_; }
	void setSelectedId(int id) { selected_id_ = id; }

	// Draw into a fixed rectangle (your current layout style)
	void draw(ImVec2 pos, ImVec2 size);

private:
	const Node *findNode(int id) const;
	void		drawNodeRecursive(int id);

private:
	std::vector<Node> nodes_;
	int				  root_id_ = -1;
	int				  selected_id_ = -1;
};
