#include <deque>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>


template<typename T>
class Thread_Safe_Queue
{
private:
    std::mutex mutex_;
    //std::condition_variable condition_;
   
	bool end_;

public:
	std::deque<T> queue_;
	Thread_Safe_Queue():end_(0){}

	template<typename Func>
	void Sort(Func f)
	{
		 std::unique_lock<std::mutex> lock(mutex_);
		 std::sort(queue_.begin(), queue_.end(), f);
	}

    void push(T const& value)
	{
        {
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.push_front(value);
        }
      //  condition_.notify_one();
    }

    T pop() 
	{
        std::unique_lock<std::mutex> lock(mutex_);

		/*condition_.wait(lock, [=]{ 
			return !queue_.empty() || end_;
		});*/

		if(queue_.empty())
			return 0;

        T rc = queue_.back();

        queue_.pop_back();

		//condition_.notify_one();
        return rc;
    }

	int guessSize()
	{
		return queue_.size();
	}

	T erase(int idx)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		T val = queue_[idx];
		queue_.erase(queue_.begin() + idx);
		return val;
	}

	T at(int idx)
	{
	    std::unique_lock<std::mutex> lock(mutex_);
		if(idx < queue_.size())
			return queue_[idx];
		else
			return 0;
	}

	T atNoLock(int idx)
	{
		if(idx < queue_.size())
			return queue_[idx];
		else
			return 0;
	}

	bool in(T& val)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		return std::find(queue_.begin(), queue_.end(), val) != queue_.end();
	}

};

template<typename Key, typename Value, typename Hash>
class Thread_Safe_UnorderedMap
{
private:
    std::mutex mutex_;
    std::condition_variable condition_;

	std::unordered_map<Key, Value, Hash> map_;

public:

	void insert(Key k, Value v)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		map_.insert(std::make_pair(k,v));
	}

	void erase(Key k)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		map_.erase(k);
	}
	
	Value get(Key k)
	{
		std::unique_lock<std::mutex> lock(mutex_);

		std::unordered_map<Key, Value, Hash>::const_iterator got = map_.find(k);

		if(got == map_.end())
			return 0;
		else
		{
			return got->second;
		}

	}
};

