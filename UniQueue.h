#pragma once

#include <shared_mutex>
#include <queue>

template<typename OBJ>

class CUniQueue
{
public:
	//CUniQueue();
	//~CUniQueue();

	using Container = std::queue<OBJ>;
	CUniQueue() :is_run_(true) {};
	virtual ~CUniQueue() {};

	// 컨테이너가 비어 있는지 확인합니다.
	// 데이터 수정이 없기 때문에 shared lock모드를 사용합니다.							
	bool Empty()
	{
		std::shared_lock<std::shared_mutex> sl(sm_);
		return con_.empty();
	}

	// OBJ를 컨테이너에서 추출합니다. 							
	// 데이터가 없다면 condition_variable을 통해서 대기 합니다.							
	bool Pop(OBJ& obj) 
	{
		std::unique_lock<std::shared_mutex> ul(sm_);
		cv_.wait(ul, [this] {return (!con_.empty() || !is_run_); });

		if (!is_run_) { return false; }

		if (!con_.empty()) {
			obj = con_.front();
			con_.pop();
			return true;
		}
		return false;
	}

	// 데이터를 컨테이너에 넣습니다. 							
	// condition_variable 를 통해서 통지합니다.							
	void Push(const OBJ& obj) 
	{
		std::unique_lock<std::shared_mutex> sl(sm_);
		con_.push(obj);
		cv_.notify_one();
	}

	// 모든 대기 스레드에 통지를 전달합니다.							
	void NotifyAll()
	{
		cv_.notify_all();
	}

	// 컨테이너의 크기를 확인합니다. 							
	// 데이터 수정이 없기 때문에 shared lock모드를 사용합니다.							
	size_t Size()
	{
		std::shared_lock<std::shared_mutex> sl(sm_);
		return con_.size();
	}

	bool IsRun() 
	{ 
		return is_run_; 
	}
	void SetStop() 
	{
		std::unique_lock<std::shared_mutex> ul(sm_);
		is_run_ = false;
		NotifyAll();
	}

private:
	Container						con_;
	std::condition_variable_any		cv_;
	std::shared_mutex				sm_;
	bool							is_run_;
};

