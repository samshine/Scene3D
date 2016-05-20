
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
	impl->root_item = std::make_shared<TreeItem>();
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
