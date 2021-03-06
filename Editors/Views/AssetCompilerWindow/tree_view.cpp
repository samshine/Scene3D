
#include "precomp.h"
#include "tree_view.h"
#include "tree_item.h"
#include "tree_node_view.h"

using namespace uicore;

class TreeBaseViewImpl
{
public:
	TreeItemPtr root_item;
	TreeItemPtr first_visible;
	TreeItemPtr last_visible;
	TreeItemPtr first_selected;
	TreeItemPtr drop_target;

	int item_count = 0;
	int selected_count = 0;
	float indent = 18.0f;

	Signal<void()> sig_begin_drag;
	Signal<void()> sig_begin_label_edit;
	Signal<void()> sig_remove_item;
	Signal<void()> sig_end_label_edit;
	Signal<void()> sig_item_changed;
	Signal<void()> sig_item_changing;
	Signal<void()> sig_item_expanded;
	Signal<void()> sig_item_expanding;
	Signal<void()> sig_key_down;
	Signal<void()> sig_selection_changed;
	Signal<void()> sig_selection_changing;
	Signal<void()> sig_single_expand;
};

TreeBaseView::TreeBaseView() : impl(new TreeBaseViewImpl())
{
	content_view()->style()->set("flex-direction: column");

	impl->root_item = std::make_shared<TreeItem>("#root");
	impl->root_item->_tree_view = this;
}

TreeBaseView::~TreeBaseView()
{
}

TreeItemPtr TreeBaseView::root_item()
{
	return impl->root_item;
}

TreeItemPtr TreeBaseView::first_visible()
{
	return impl->first_visible;
}

TreeItemPtr TreeBaseView::next_visible(const TreeItemPtr &cur)
{
	return nullptr;
}

TreeItemPtr TreeBaseView::last_visible()
{
	return impl->last_visible;
}

TreeItemPtr TreeBaseView::first_selected()
{
	return impl->first_selected;
}

TreeItemPtr TreeBaseView::next_selected(const TreeItemPtr &cur)
{
	return nullptr;
}

TreeItemPtr TreeBaseView::drop_target()
{
	return impl->drop_target;
}

int TreeBaseView::item_count() const
{
	return impl->item_count;
}

int TreeBaseView::selected_count() const
{
	return impl->selected_count;
}

void TreeBaseView::clear_selection()
{
}

void TreeBaseView::clear_drop_target()
{
}

float TreeBaseView::indent() const
{
	return impl->indent;
}

void TreeBaseView::set_indent(float pixels)
{
}

void TreeBaseView::set_insert_mark_before(TreeItem *location)
{
}

void TreeBaseView::set_insert_mark_after(TreeItem *location)
{
}

void TreeBaseView::clear_insert_mark()
{
}

Signal<void()> &TreeBaseView::sig_begin_drag()
{
	return impl->sig_begin_drag;
}

Signal<void()> &TreeBaseView::sig_begin_label_edit()
{
	return impl->sig_begin_label_edit;
}

Signal<void()> &TreeBaseView::sig_remove_item()
{
	return impl->sig_remove_item;
}

Signal<void()> &TreeBaseView::sig_end_label_edit()
{
	return impl->sig_end_label_edit;
}

Signal<void()> &TreeBaseView::sig_item_changed()
{
	return impl->sig_item_changed;
}

Signal<void()> &TreeBaseView::sig_item_changing()
{
	return impl->sig_item_changing;
}

Signal<void()> &TreeBaseView::sig_item_expanded()
{
	return impl->sig_item_expanded;
}

Signal<void()> &TreeBaseView::sig_item_expanding()
{
	return impl->sig_item_expanding;
}

Signal<void()> &TreeBaseView::sig_key_down()
{
	return impl->sig_key_down;
}

Signal<void()> &TreeBaseView::sig_selection_changed()
{
	return impl->sig_selection_changed;
}

Signal<void()> &TreeBaseView::sig_selection_changing()
{
	return impl->sig_selection_changing;
}

Signal<void()> &TreeBaseView::sig_single_expand()
{
	return impl->sig_single_expand;
}

void TreeBaseView::item_added(const TreeItemPtr &item, int level)
{
	item->_node_view = std::make_shared<TreeNodeView>();
	item->_node_view->label->set_text(item->name());
	item->_node_view->set_ident(impl->indent * level);

	content_view()->insert_before(item->_node_view, find_next_view(item));

	for (auto child = item->first_child(); child; child = child->next_sibling())
		item_added(child, level + 1);
}

void TreeBaseView::item_updated(const TreeItemPtr &item)
{
	if (item->_node_view)
		item->_node_view->label->set_text(item->name());
}

void TreeBaseView::item_removed(const TreeItemPtr &item)
{
	for (auto child = item->first_child(); child; child = child->next_sibling())
		item_removed(child);

	if (item->_node_view)
		item->_node_view->remove_from_parent();
	item->_node_view = nullptr;
}

std::shared_ptr<TreeNodeView> TreeBaseView::find_next_view(const TreeItemPtr &item)
{
	if (item->first_child())
		return item->first_child()->_node_view;
	else if (item->next_sibling())
		return item->next_sibling()->_node_view;

	for (auto parent = item->parent(); parent != nullptr; parent = parent->parent())
	{
		if (parent->next_sibling())
			return parent->next_sibling()->_node_view;
	}

	return nullptr;
}
