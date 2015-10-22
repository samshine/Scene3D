
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

class WorkQueueImpl;

/// \brief Thread pool for worker threads
class WorkQueue
{
public:
	/// \brief Constructs a work queue
	/// \param serial_queue If true, executes items in the order they are queued, one at a time
	static std::shared_ptr<WorkQueue> create(bool serial_queue = false);

	/// \brief Queue some work to be executed on a worker thread
	virtual void queue_item(std::shared_ptr<WorkItem> item) = 0;

	/// \brief Queue some work to be executed on a worker thread
	virtual void queue(const std::function<void()> &func) = 0;

	/// \brief Queue some work to be executed on the main WorkQueue thread
	virtual void work_completed(const std::function<void()> &func) = 0;

	/// \brief Returns the number of items currently queued
	virtual int items_queued() const = 0;

	/// \brief Process work completed queue
	///
	/// Needs to be called on the main WorkQueue thread periodically to finish queued work
	virtual void process_work_completed() = 0;
};

typedef std::shared_ptr<WorkQueue> WorkQueuePtr;
