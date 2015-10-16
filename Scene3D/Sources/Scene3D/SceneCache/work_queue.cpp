
#include "precomp.h"
#include "work_queue.h"
#include <algorithm>
#include <atomic>
#include <thread>
#include <condition_variable>

using namespace uicore;

class WorkItemProcess : public WorkItem
{
public:
	WorkItemProcess(const std::function<void()> &func) : func(func) { }

	void process_work() override { func(); }

private:
	std::function<void()> func;
};

class WorkItemWorkCompleted : public WorkItem
{
public:
	WorkItemWorkCompleted(const std::function<void()> &func) : func(func) { }

	void process_work() override { }
	void work_completed() override { func(); }

private:
	std::function<void()> func;
};

class WorkQueueImpl
{
public:
	WorkQueueImpl(bool serial_queue);
	~WorkQueueImpl();

	void queue(WorkItem *item); // transfers ownership
	void work_completed(WorkItem *item); // transfers ownership

	int get_items_queued() const { return items_queued; }

	void process_work_completed();

private:
	void worker_main();

	bool serial_queue = false;
	std::vector<std::thread> threads;
	std::mutex mutex;
	std::condition_variable worker_event;
	bool stop_flag = false;
	std::vector<WorkItem *> queued_items;
	std::vector<WorkItem *> finished_items;
	std::atomic_int items_queued;
};

WorkQueue::WorkQueue(bool serial_queue)
	: impl(std::make_shared<WorkQueueImpl>(serial_queue))
{
}

WorkQueue::~WorkQueue()
{
}

void WorkQueue::queue(WorkItem *item) // transfers ownership
{
	impl->queue(item);
}

void WorkQueue::queue(const std::function<void()> &func)
{
	impl->queue(new WorkItemProcess(func));
}

void WorkQueue::work_completed(const std::function<void()> &func)
{
	impl->work_completed(new WorkItemWorkCompleted(func));
}

int WorkQueue::get_items_queued() const
{
	return impl->get_items_queued();
}

void WorkQueue::process_work_completed()
{
	impl->process_work_completed();
}

/////////////////////////////////////////////////////////////////////////////

WorkQueueImpl::WorkQueueImpl(bool serial_queue)
	: serial_queue(serial_queue)
{
}

WorkQueueImpl::~WorkQueueImpl()
{
	std::unique_lock<std::mutex> mutex_lock(mutex);
	stop_flag = true;
	mutex_lock.unlock();
	worker_event.notify_all();

	for (auto & elem : threads)
		elem.join();
	for (auto & elem : queued_items)
		delete elem;
	for (auto & elem : finished_items)
		delete elem;
}

void WorkQueueImpl::queue(WorkItem *item) // transfers ownership
{
	if (threads.empty())
	{
		int num_cores = serial_queue ? 1 : uicore::max(System::get_num_cores() - 1, 1);
		for (int i = 0; i < num_cores; i++)
		{
			threads.push_back(std::thread(&WorkQueueImpl::worker_main, this));
		}
	}

	std::unique_lock<std::mutex> mutex_lock(mutex);
	queued_items.push_back(item);
	++items_queued;
	mutex_lock.unlock();
	worker_event.notify_one();
}

void WorkQueueImpl::work_completed(WorkItem *item) // transfers ownership
{
	std::unique_lock<std::mutex> mutex_lock(mutex);
	finished_items.push_back(item);
	++items_queued;
}

void WorkQueueImpl::process_work_completed()
{
	std::unique_lock<std::mutex> mutex_lock(mutex);
	std::vector<WorkItem *> items;
	items.swap(finished_items);
	mutex_lock.unlock();
	for (size_t i = 0; i < items.size(); i++)
	{
		try
		{
			items[i]->work_completed();
		}
		catch (...)
		{
			mutex_lock.lock();
			finished_items.insert(finished_items.begin(), items.begin() + i, items.end());
			throw;
		}
		delete items[i];
		--items_queued;
	}
}

void WorkQueueImpl::worker_main()
{
	while (true)
	{
		std::unique_lock<std::mutex> mutex_lock(mutex);
		worker_event.wait(mutex_lock, [&]() { return stop_flag || !queued_items.empty(); });

		if (stop_flag)
			break;

		WorkItem *item = queued_items.front();
		queued_items.erase(queued_items.begin());
		mutex_lock.unlock();

		item->process_work();

		mutex_lock.lock();
		finished_items.push_back(item);
		mutex_lock.unlock();
	}
}
