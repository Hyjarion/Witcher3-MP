#pragma once

namespace Networking
{
	template<typename T>
	class tsqueue
	{
	public:
		tsqueue() = default;
		tsqueue(const tsqueue<T>&) = delete;
		virtual ~tsqueue() { clear(); }

	public:
		const T& front()
		{
			std::scoped_lock lock(muxQueue);
			return deQueue.front();
		}

		const T& back()
		{
			std::scoped_lock lock(muxQueue);
			return deQueue.back();
		}

		void push_back(const T& item)
		{
			std::scoped_lock lock(muxQueue);
			deQueue.emplace_back(std::move(item));

			std::unique_lock<std::mutex> ul(muxBlocking);
			cvBlocking.notify_one();
		}

		void push_front(const T& item)
		{
			std::scoped_lock lock(muxQueue);
			deQueue.emplace_front(std::move(item));

			std::unique_lock<std::mutex> ul(muxBlocking);
			cvBlocking.notify_one();
		}

		bool empty()
		{
			std::scoped_lock lock(muxQueue);
			return deQueue.empty();
		}

		size_t count()
		{
			std::scoped_lock lock(muxQueue);
			return deQueue.size();
		}

		void clear()
		{
			std::scoped_lock lock(muxQueue);
			deQueue.clear();
		}

		T pop_front()
		{
			std::scoped_lock lock(muxQueue);
			auto t = std::move(deQueue.front());
			deQueue.pop_front();
			return t;
		}

		T pop_back()
		{
			std::scoped_lock lock(muxQueue);
			auto t = std::move(deQueue.back());
			deQueue.pop_back();
			return t;
		}

		void wait()
		{
			while (empty())
			{
				std::unique_lock<std::mutex> ul(muxBlocking);
				cvBlocking.wait(ul);
			}
		}

	protected:
		std::mutex muxQueue;
		std::deque<T> deQueue;
		std::condition_variable cvBlocking;
		std::mutex muxBlocking;
	};
}
