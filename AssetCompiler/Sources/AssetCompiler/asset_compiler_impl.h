
#pragma once

class AssetCompilerImpl
{
public:
	~AssetCompilerImpl()
	{
		cancel();
	}

	void cancel()
	{
		if (!worker_thread.joinable())
			return;

		std::unique_lock<std::mutex> lock(mutex);
		cancel_flag = true;
		lock.unlock();
		cond_var.notify_all();
		worker_thread.join();
	}

	void build();
	void clean();

private:
	void execute(std::function<void()> work)
	{
		cancel();
		std::unique_lock<std::mutex> lock(mutex);
		cancel_flag = false;
		lock.unlock();
		worker_thread = std::thread(work);
	}

	std::string asset_directory;
	std::string build_directory;
	std::function<void(const CompilerMessage&)> log_message;

	std::thread worker_thread;
	std::mutex mutex;
	bool cancel_flag = false;
	std::condition_variable cond_var;

	friend class AssetCompiler;
};
