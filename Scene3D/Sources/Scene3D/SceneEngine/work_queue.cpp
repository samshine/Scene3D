
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

class WorkQueueImpl : public WorkQueue
{
public:
	WorkQueueImpl(bool serial_queue);
	~WorkQueueImpl();

	void queue_item(std::shared_ptr<WorkItem> item) override;
	void queue(const std::function<void()> &func) override { queue_item(std::make_shared<WorkItemProcess>(func)); }
	void work_completed(const std::function<void()> &func) override { work_completed(std::static_pointer_cast<WorkItem>(std::make_shared<WorkItemWorkCompleted>(func))); }
	int items_queued() const override { return _items_queued; }
	void process_work_completed() override;

private:
	void work_completed(std::shared_ptr<WorkItem> item);

	void worker_main();

	bool serial_queue = false;
	std::vector<std::thread> threads;
	std::mutex mutex;
	std::condition_variable worker_event;
	bool stop_flag = false;
	std::vector<std::shared_ptr<WorkItem>> queued_items;
	std::vector<std::shared_ptr<WorkItem>> finished_items;
	std::atomic_int _items_queued;
};

std::shared_ptr<WorkQueue> WorkQueue::create(bool serial_queue)
{
	return std::make_shared<WorkQueueImpl>(serial_queue);
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

	for (auto &thread : threads)
		thread.join();
}

void WorkQueueImpl::queue_item(std::shared_ptr<WorkItem> item)
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
	++_items_queued;
	mutex_lock.unlock();
	worker_event.notify_one();
}

void WorkQueueImpl::work_completed(std::shared_ptr<WorkItem> item)
{
	std::unique_lock<std::mutex> mutex_lock(mutex);
	finished_items.push_back(item);
	++_items_queued;
}

void WorkQueueImpl::process_work_completed()
{
	std::unique_lock<std::mutex> mutex_lock(mutex);
	std::vector<std::shared_ptr<WorkItem>> items;
	items.swap(finished_items);
	mutex_lock.unlock();
	for (size_t i = 0; i < items.size(); i++)
	{
		--_items_queued;
		try
		{
			items[i]->work_completed();
		}
		catch (...)
		{
			mutex_lock.lock();
			finished_items.insert(finished_items.begin(), items.begin() + i + 1, items.end());
			throw;
		}
	}
}

void WorkQueueImpl::worker_main()
{
	try
	{
		while (true)
		{
			std::unique_lock<std::mutex> mutex_lock(mutex);
			worker_event.wait(mutex_lock, [&]() { return stop_flag || !queued_items.empty(); });

			if (stop_flag)
				break;

			std::shared_ptr<WorkItem> item = queued_items.front();
			queued_items.erase(queued_items.begin());
			mutex_lock.unlock();

			item->process_work();

			mutex_lock.lock();
			finished_items.push_back(item);
			mutex_lock.unlock();
		}
	}
	catch (...)
	{
		ExceptionDialog::show(std::current_exception());
	}
}
