#pragma once
#include <functional>
#include <vector>
namespace Magia {

	using UpdateFunc = std::function<void(float)>;
	using IsOverFunc = std::function<bool(void)>;
	using StartFunc = std::function<void(void)>;
	using EndFunc = std::function<void(void)>;

	class Task
	{
	public:
		std::vector<UpdateFunc> update_func_list;
		std::vector<StartFunc> start_func_list;
		std::vector<EndFunc> end_func_list;
		IsOverFunc isover_func;
		void invoke();
		void end();
		Task() :update_func_list(), start_func_list(), end_func_list(), isover_func() {}
		~Task();
		void update(float dt);

	private:
		bool running = false;
	};

	void Task::invoke()
	{
		if (!running)
		{
			for (size_t i = 0; i < start_func_list.size(); i++)
			{
				start_func_list[i]();
			}
			running = true;
		}
	}

	void Task::update(float dt)
	{
		if (running)
		{
			if (isover_func())
			{
				end();
				return;
			}
			for (size_t i = 0; i < update_func_list.size(); i++)
			{
				update_func_list[i](dt);
			}
		}
	}

	void Task::end()
	{
		if (running)
		{
			for (size_t i = 0; i < end_func_list.size(); i++)
			{
				end_func_list[i]();
				running = false;
			}
		}
	}

	Task::~Task()
	{
	}
}