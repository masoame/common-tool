#pragma once
#include<type_traits>
#include<memory>
#include<functional>
#include<queue>
#include<mutex>
#include<condition_variable>

#include<future>
#include<assert.h>
namespace common
{
    /*
    * Functor用于静态封装一个函数，使其可以作为模板参数。
	* _F: 被封装的函数
    */
    template <auto _F>
    using Functor = std::integral_constant<std::remove_reference_t<decltype(_F)>, _F>;

    /*
    * AutoPtr用于封装智能指针，使其可以自动管理内存，
	* 方便对于一些c库自动析构进行适配
	* 支持两级指针因为有些库封装析构的指针是二级指针。（即释放对二级指向对象的指针对对应的一级指针进行置空）
	* 重载 operator& 返回一个指针_Type**，用于获取原始指针的地址。
	* _T: 被封装的智能指针类型
    */
    template<class _T, class _FreeFunc>
    struct AutoPtr
    {
        using _Type = std::remove_reference_t<_T>;
        static_assert(std::is_invocable_v< _FreeFunc::value_type, _Type**> || std::is_invocable_v<_FreeFunc::value_type, _Type*>);
        constexpr static bool isSecPtr = std::is_invocable_v<_FreeFunc::value_type, _Type**>;

        AutoPtr() noexcept {}
        AutoPtr(AutoPtr& Autoptr) = delete;
        explicit AutoPtr(AutoPtr&& Autoptr) noexcept : _ptr(Autoptr.release()) {}
        AutoPtr(_Type* ptr) noexcept : _ptr(ptr) {}

        void operator=(_Type* ptr) noexcept { _ptr.reset(ptr); }
        void operator=(AutoPtr&& Autoptr) noexcept { _ptr.reset(Autoptr.release()); }

        operator const _Type* () const noexcept { return _ptr.get(); }
        operator _Type*& () noexcept { return *reinterpret_cast<_Type**>(this); }
        operator bool() const noexcept { return _ptr.get() != nullptr; }

        _Type** operator&() { static_assert(sizeof(*this) == sizeof(void*)); return reinterpret_cast<_Type**>(this); }

        _Type* operator->() const noexcept { return _ptr.get(); }

        void reset(_Type* ptr = nullptr) noexcept { _ptr.reset(ptr); }

        auto release() noexcept { return _ptr.release(); }
        auto get() const noexcept { return _ptr.get(); }
    private:
        struct DeletePrimaryPtr { void operator()(void* ptr) { _FreeFunc()(static_cast<_Type*>(ptr)); } };
        struct DeleteSecPtr { void operator()(void* ptr) { _FreeFunc()(reinterpret_cast<_Type**>(&ptr)); } };
        using DeletePtr = std::conditional<isSecPtr, DeleteSecPtr, DeletePrimaryPtr>::type;
        std::unique_ptr<_Type, DeletePtr> _ptr;
    };
    /*
    * reverse_bit用于反转位域reverse_bit
    */
    template<class _T1>
    inline void reverse_bit(_T1& val, _T1 local) noexcept requires std::is_pod_v<_T1>
	{
        val = (val & local) ? (val & (~local)) : (val | local);
    }

	/*
	* circular_queue用于实现一个循环队列，支持线程安全的入队和出队操作。内部资源都是通过智能指针管理，支持自定义的删除函数。
	* _IsThreadSafe == true 时，队列是线程安全性最高的。
	* _IsThreadSafe == false 时，队列在一读一写的情况下是线程安全的，但是在多读多写的情况下，需要用户自行保证线程安全。
	* _buf_level 用于设置队列的大小，默认为4，最大为64。 实际大小为2的_buf_level次方。
	*/
	template<class _T, class _DeleteFunctionType = nullptr_t, bool _IsThreadSafe = false>
	class circular_queue 
	{
		using _Type = std::remove_reference<_T>::type;
		using _ElementPtrType = std::conditional_t<std::is_null_pointer_v<_DeleteFunctionType>, std::unique_ptr<_Type>, AutoPtr<_Type, _DeleteFunctionType>>;
		using _HasMutex = std::conditional_t<_IsThreadSafe, std::mutex, nullptr_t>;
		using _IsLock = std::conditional_t<_IsThreadSafe, std::unique_lock<std::mutex>, nullptr_t>;

	public:
		circular_queue(circular_queue& target) = delete;

		explicit circular_queue(unsigned char _buf_level = 4) :_mask(~((~0) << _buf_level))
		{
			assert(_buf_level >= 1);
			assert(_buf_level <= 64);
			_Arr.reset(new _ElementPtrType[_mask + 1]);
		}

		bool try_push(_ElementPtrType&& target) noexcept
		{
			_IsLock _lock(_push_mtx);
			if (full()) return false;
			_Arr[_rear++ & _mask] = std::move(target);
			return true;
		}

		template<class ...Args>
		bool try_emplace(Args&& ...target) noexcept
		{
			_IsLock _lock(_push_mtx);
			if (full()) return false;
			_Arr[_rear & _mask].reset(new _Type(std::forward<Args>(target)...));
			_rear++;
			return true;
		}

		bool try_push(_Type&& target) noexcept requires std::is_move_constructible_v<_Type>
		{
			_IsLock _lock(_push_mtx);
			if (full()) return false;
			_Arr[_rear & _mask].reset(new _Type(std::move(target)));
			_rear++;
			return true;
		}

		bool try_push(_Type* target) noexcept
		{
			_IsLock _lock(_push_mtx);
			if (full()) return false;
			_Arr[_rear & _mask].reset(target);
			_rear++;
			return true;
		}

		_ElementPtrType try_pop() noexcept
		{
			_IsLock _lock(_pop_mtx);
			if (empty()) return nullptr;
			return _ElementPtrType{ _Arr[_front++ & _mask].release() };
		}

		_Type* try_pop_raw() noexcept
		{
			_IsLock _lock(_pop_mtx);
			if (empty()) return nullptr;
			return _Arr[_front++ & _mask].release();
		}

		_Type* front() noexcept
		{
			if (empty()) return nullptr;
			return reinterpret_cast<_Type*>(_Arr[_front & _mask].get());
		}

		_Type* rear() noexcept
		{
			if (full()) return nullptr;
			return reinterpret_cast<_Type*>(_Arr[_rear & _mask].get());
		}

		bool full() const noexcept
		{
			if (((_rear + 1) & _mask) == (_front & _mask)) return true;
			return false;
		}
		bool empty() const noexcept
		{
			if ((_rear & _mask) == (_front & _mask)) return true;
			return false;
		}

		size_t size() const noexcept { return _rear - _front; }

		void clear() noexcept { _front.store(_rear); }
	private:
		const unsigned long long _mask;

		_HasMutex _pop_mtx{}, _push_mtx{};

		std::unique_ptr <_ElementPtrType[]> _Arr;
		std::atomic<size_t> _front = 0, _rear = 0;
	};

	/*
	* singletont_thread_pool用于实现一个单例线程池，支持线程安全的任务队列和线程管理。
	* 参考项目: https://github.com/progschj/ThreadPool
	* 未完成
	*/
	class ThreadPool {
	public:
		ThreadPool(size_t threads, size_t max_queue_size);
		~ThreadPool();

		template<class F, class... Args>
		auto enqueue(F&& f, Args&&... args)
			-> std::future<typename std::invoke_result<F,Args...>::type>;


	private:
		// need to keep track of threads so we can join them
		std::vector< std::thread > workers;

		circular_queue<std::function<void()>> _task_queue;
		// the task queue
		std::queue< std::function<void()> > tasks;

		// synchronization
		std::mutex queue_mutex;
		std::condition_variable condition;
		bool stop;
	};

	// the constructor just launches some amount of workers
	inline ThreadPool::ThreadPool(size_t threads,size_t max_queue_size)
		: stop(false)
	{
		for (size_t i = 0; i < threads; ++i)
			workers.emplace_back(
				[this]
				{
					for (;;)
					{
						std::function<void()> task;

						{
							std::unique_lock<std::mutex> lock(this->queue_mutex);
							this->condition.wait(lock,
								[this] { return this->stop || !this->tasks.empty(); });
							if (this->stop && this->tasks.empty())
								return;
							task = std::move(this->tasks.front());
							this->tasks.pop();
						}

						task();
					}
				}
			);
	}

	// add new work item to the pool
	template<class F, class... Args>
	auto ThreadPool::enqueue(F&& f, Args&&... args)
		-> std::future<typename std::invoke_result<F,Args...>::type>
	{
		using return_type = typename std::invoke_result<F,Args...>::type;

		auto task = std::make_shared< std::packaged_task<return_type()> >(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

		std::future<return_type> res = task->get_future();
		{
			std::unique_lock<std::mutex> lock(queue_mutex);

			// don't allow enqueueing after stopping the pool
			if (stop)
				throw std::runtime_error("enqueue on stopped ThreadPool");

			tasks.emplace([task]() { (*task)(); });
		}
		condition.notify_one();
		return res;
	}

	// the destructor joins all threads
	inline ThreadPool::~ThreadPool()
	{
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			stop = true;
		}
		condition.notify_all();
		for (std::thread& worker : workers)
			worker.join();
	}
}