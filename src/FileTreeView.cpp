#include "FileTreeView.hpp"

void FileTreeView::setModel(std::vector<Node> nodes, int root_id) {
	nodes_ = std::move(nodes);
	root_id_ = root_id;
	if (selected_id_ < 0)
		selected_id_ = root_id_;
}

const FileTreeView::Node *FileTreeView::findNode(int id) const {
	// O(n) is fine for now; later we can add an id->index map.
	for (const auto &n : nodes_)
		if (n.id == id)
			return &n;
	return nullptr;
}

void FileTreeView::draw(ImVec2 pos, ImVec2 size) {
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(size);

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
							 | ImGuiWindowFlags_NoCollapse
							 | ImGuiWindowFlags_NoSavedSettings;

	ImGui::Begin("fileTree", nullptr, flags);

	// Qt-like header
	ImGui::TextUnformatted("Name");
	ImGui::Separator();

	// Scrollable area
	ImGui::BeginChild("fileTree_scroll", ImVec2(0, 0), true,
					  ImGuiWindowFlags_AlwaysVerticalScrollbar);

	if (root_id_ >= 0)
		drawNodeRecursive(root_id_);

	ImGui::EndChild();
	ImGui::End();
}

void FileTreeView::drawNodeRecursive(int id) {
	const Node *n = findNode(id);
	if (!n)
		return;

	const bool is_leaf = !n->is_dir || n->children.empty();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth
							   | ImGuiTreeNodeFlags_OpenOnArrow
							   | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (is_leaf)
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	if (selected_id_ == id)
		flags |= ImGuiTreeNodeFlags_Selected;

	bool open =
		ImGui::TreeNodeEx(n->name.c_str(), flags, "%s", n->name.c_str());

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		selected_id_ = id;

	if (!is_leaf && open) {
		for (int child_id : n->children)
			drawNodeRecursive(child_id);
		ImGui::TreePop();
	}
}
