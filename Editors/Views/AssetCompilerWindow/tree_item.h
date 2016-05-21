
#pragma once

#include <memory>

class TreeBaseView;
class TreeNodeView;

class TreeItem;
typedef std::shared_ptr<TreeItem> TreeItemPtr;
typedef std::weak_ptr<TreeItem> TreeItemWeakPtr;

class TreeItem : std::enable_shared_from_this<TreeItem>
{
public:
	TreeItem(const std::string &name);

	TreeItem *parent() const;

	TreeItemPtr first_child() const;
	TreeItemPtr last_child() const;
	TreeItemPtr previous_sibling() const;
	TreeItemPtr next_sibling() const;

	bool has_child_nodes() const { return (bool)first_child(); }
	int depth() const;

	TreeBaseView *tree_view() const;

	TreeItemPtr insert_before(const TreeItemPtr &new_child, const TreeItemPtr &ref_child);
	TreeItemPtr replace_child(const TreeItemPtr &new_child, const TreeItemPtr &old_child);
	TreeItemPtr add_child(const TreeItemPtr &new_child);

	void remove_from_parent();

	const std::string &name() const;
	void set_name(const std::string &name);

	uicore::PixelBufferPtr create_drag_image();

	void edit_label();
	void end_edit_label_now();

	void ensure_visible();

	void expand();
	void collapse();
	void collapse_reset(); // Collapses the list, removes the child items, and resets expanded_once flag
	void toggle();

	bool expanded_once() const;

	void sort_children(bool recursive);

	void select(bool make_first_visible = false, bool expand = true);
	void select_as_drop_target();

private:
	TreeBaseView *_tree_view = nullptr;
	std::shared_ptr<TreeNodeView> _node_view;

	TreeItem *_parent = nullptr;
	TreeItemPtr _first_child, _last_child;
	TreeItemPtr _next_sibling;
	TreeItemWeakPtr _prev_sibling;

	std::string _name;

	friend class TreeBaseView;
};
