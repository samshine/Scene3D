
#include "precomp.h"
#include "tree_item.h"
#include "tree_view.h"

using namespace uicore;

TreeItem::TreeItem(const std::string &name) : _name(name)
{
}

const std::string &TreeItem::name() const
{
	return _name;
}

void TreeItem::set_name(const std::string &name)
{
	if (_name != name)
	{
		_name = name;
		auto tree = tree_view();
		if (tree)
			tree->item_updated(shared_from_this());
	}
}

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
	if (_parent)
		return _parent->tree_view();
	else
		return _tree_view;
}

TreeItemPtr TreeItem::insert_before(const TreeItemPtr &new_child, const TreeItemPtr &ref_child)
{
	if (!ref_child)
		return add_child(new_child);

	if (!new_child)
		throw Exception("instance not set to an object");

	if (ref_child->parent() != this)
		throw Exception("not parent of reference child node");

	if (new_child->parent())
		new_child->remove_from_parent();

	new_child->_prev_sibling = ref_child->_prev_sibling;
	new_child->_next_sibling = ref_child;
	ref_child->_prev_sibling = new_child;

	if (new_child->previous_sibling())
		new_child->previous_sibling()->_next_sibling = new_child;

	if (_first_child == ref_child)
		_first_child = new_child;

	new_child->_parent = this;

	auto tree = tree_view();
	if (tree)
		tree->item_added(new_child, new_child->depth() - 1);

	return new_child;
}

TreeItemPtr TreeItem::replace_child(const TreeItemPtr &new_child, const TreeItemPtr &old_child)
{
	if (!new_child || !old_child)
		throw Exception("instance not set to an object");

	if (old_child->parent() != this)
		throw Exception("not parent of old child node");

	insert_before(new_child, old_child);
	old_child->remove_from_parent();

	return old_child;
}

void TreeItem::remove_from_parent()
{
	if (!_parent)
		return;

	auto old_child = shared_from_this();

	auto tree = tree_view();
	if (tree)
		tree->item_removed(old_child);

	if (_parent->_first_child == old_child)
		_parent->_first_child = old_child->next_sibling();
	if (_parent->_last_child == old_child)
		_parent->_last_child = old_child->previous_sibling();

	auto prev = old_child->previous_sibling();
	if (prev)
		prev->_next_sibling = old_child->_next_sibling;
	if (old_child->_next_sibling)
		old_child->_next_sibling->_prev_sibling = prev;

	old_child->_prev_sibling.reset();
	old_child->_next_sibling.reset();

	old_child->_parent = nullptr;
}

TreeItemPtr TreeItem::add_child(const TreeItemPtr &new_child)
{
	if (!new_child)
		throw Exception("instance not set to an object");

	if (new_child->parent())
		new_child->remove_from_parent();

	if (_first_child)
	{
		auto last = last_child();
		last->_next_sibling = new_child;
		new_child->_prev_sibling = last;
		_last_child = new_child;
	}
	else
	{
		_first_child = new_child;
		_last_child = new_child;
	}

	new_child->_parent = this;

	auto tree = tree_view();
	if (tree)
		tree->item_added(new_child, new_child->depth() - 1);

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
	return false;
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

int TreeItem::depth() const
{
	int level = 0;
	for (TreeItem *cur = parent(); cur != nullptr; cur = cur->parent())
		level++;
	return level;
}
