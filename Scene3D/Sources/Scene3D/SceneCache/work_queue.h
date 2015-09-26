
#pragma once

#include <memory>
#include <functional>

/// \brief Interface for executing work on a worker thread
class WorkItem
{
public:
	virtual ~WorkItem() { }

	/// \brief Called by a worker thread to process work
	virtual void process_work() = 0;

	/// \brief Called by the WorkQueue thread to complete the work
	virtual void work_completed() { }
};

class WorkQueue_Impl;

/// \brief Thread pool for worker threads
class WorkQueue
{
public:
	/// \brief Constructs a work queue
	/// \param serial_queue If true, executes items in the order they are queued, one at a time
	WorkQueue(bool serial_queue = false);
	~WorkQueue();

	/// \brief Queue some work to be executed on a worker thread
	///
	/// Transfers ownership of the item queued. WorkQueue will delete the item.
	void queue(WorkItem *item);

	/// \brief Queue some work to be executed on a worker thread
	void queue(const std::function<void()> &func);

	/// \brief Queue some work to be executed on the main WorkQueue thread
	void work_completed(const std::function<void()> &func);

	/// \brief Returns the number of items currently queued
	int get_items_queued() const;

	/// \brief Process work completed queue
	///
	/// Needs to be called on the main WorkQueue thread periodically to finish queued work
	void process_work_completed();

private:
	std::shared_ptr<WorkQueue_Impl> impl;
};
