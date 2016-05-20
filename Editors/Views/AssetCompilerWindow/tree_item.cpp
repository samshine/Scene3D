
#include "precomp.h"
#include "tree_item.h"
#include "tree_view.h"

using namespace uicore;

TreeItem *TreeItem::parent() const
{
	return _parent;
}

TreeItemPtr TreeItem::first_child() const
{
	return _first_child;
}

TreeItemPtr TreeItem::last_child() const
{
	return _last_child;
}

TreeItemPtr TreeItem::previous_sibling() const
{
	return _prev_sibling.lock();
}

TreeItemPtr TreeItem::next_sibling() const
{
	return _next_sibling;
}

TreeBaseView *TreeItem::tree_view() const
{
	return _tree_view;
}

TreeItemPtr TreeItem::insert_before(const TreeItemPtr &new_child, const TreeItemPtr &ref_child)
{
	return new_child;
}

TreeItemPtr TreeItem::replace_child(const TreeItemPtr &new_child, const TreeItemPtr &old_child)
{
	return old_child;
}

TreeItemPtr TreeItem::remove_child(const TreeItemPtr &old_child)
{
	return old_child;
}

TreeItemPtr TreeItem::append_child(const TreeItemPtr &new_child)
{
	return new_child;
}

PixelBufferPtr TreeItem::create_drag_image()
{
	return nullptr;
}

void TreeItem::edit_label()
{
}

void TreeItem::end_edit_label_now()
{
}

void TreeItem::ensure_visible()
{
}

void TreeItem::expand()
{
}

void TreeItem::collapse()
{
}

void TreeItem::collapse_reset()
{
	// Collapses the list, removes the child items, and resets expanded_once flag
}

void TreeItem::toggle()
{
}

bool TreeItem::expanded_once() const
{
}

void TreeItem::sort_children(bool recursive)
{
}

void TreeItem::select(bool make_first_visible, bool expand)
{
}

void TreeItem::select_as_drop_target()
{
}
