
#pragma once

class TreeItem;
typedef std::shared_ptr<TreeItem> TreeItemPtr;
class TreeBaseViewImpl;

class TreeBaseView : public uicore::ScrollBaseView
{
public:
	TreeBaseView();
	~TreeBaseView();

	TreeItemPtr root_item();

	TreeItemPtr first_visible();
	TreeItemPtr next_visible(const TreeItemPtr &cur);
	TreeItemPtr last_visible();

	TreeItemPtr first_selected();
	TreeItemPtr next_selected(const TreeItemPtr &cur);

	TreeItemPtr drop_target();

	int item_count() const;
	int selected_count() const;

	void clear_selection();
	void clear_drop_target();

	// Indentation relative to parent, in pixels
	float indent() const;
	void set_indent(float pixels);

	void set_insert_mark_before(TreeItem *location);
	void set_insert_mark_after(TreeItem *location);
	void clear_insert_mark();

	uicore::Signal<void()> &sig_begin_drag();
	uicore::Signal<void()> &sig_begin_label_edit();
	uicore::Signal<void()> &sig_remove_item();
	uicore::Signal<void()> &sig_end_label_edit();
	uicore::Signal<void()> &sig_item_changed();
	uicore::Signal<void()> &sig_item_changing();
	uicore::Signal<void()> &sig_item_expanded();
	uicore::Signal<void()> &sig_item_expanding();
	uicore::Signal<void()> &sig_key_down();
	uicore::Signal<void()> &sig_selection_changed();
	uicore::Signal<void()> &sig_selection_changing();
	uicore::Signal<void()> &sig_single_expand();

private:
	void item_added(const TreeItemPtr &item, int level);
	void item_updated(const TreeItemPtr &item);
	void item_removed(const TreeItemPtr &item);

	std::unique_ptr<TreeBaseViewImpl> impl;

	friend class TreeItem;
};
