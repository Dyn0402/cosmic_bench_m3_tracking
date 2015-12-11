#ifndef tomography_MT_h
#define tomography_MT_h

#include "tomography.h"

#include <pthread.h>

#include <queue>

using std::queue;

class Task{
	public:
		Task();
		Task(Task * next_task_);
		virtual ~Task();
		virtual bool do_task() = 0;
		virtual bool can_exec() = 0;
		virtual void update_task_list() = 0;
		static Task * get_next_task();
		static void add_task(Task * new_task);
		static unsigned int task_left();
	protected:
		static queue<Task*> task_queue;
		static pthread_mutex_t queue_mutex;
		static bool is_init;
		Task * next_task;
};

class IO_Task: public Task{
	public:
		IO_Task();
		IO_Task(Task * next_task_);
		~IO_Task();
		virtual bool do_task() = 0;
		virtual bool can_exec() = 0;
		virtual void update_task_list() = 0;
	protected:
		pthread_mutex_t IO_mutex;

};

class Input_Task{
	public:
		Input_Task();
		Input_Task(Task * next_task_);
		~Input_Task();
		virtual bool do_task() = 0;
		virtual bool can_exec() = 0;
		void update_task_list();
	protected:
		pthread_mutex_t IO_mutex;
		Task * next_task;
};

class Thread{
	public:
		Thread();
		virtual ~Thread();
		int start();
		int stop();
		pthread_t getThreadId() const;
		virtual bool is_working() const = 0;
	private:
		virtual void * run() = 0;
		static void * runThread(void * arg);
		virtual void pre_stop();
		pthread_attr_t attr;
		pthread_t id;
		int running;
		int detached;
};

class Worker_Thread: public Thread{
	public:
		Worker_Thread();
		~Worker_Thread();
		bool is_working() const;
	protected:
		void * run();
		void pre_stop();
		bool working;
		Task * current_task;
};

class Reader_Thread: public Thread{
	public:
		Reader_Thread(Input_Task * current_task_);
		~Reader_Thread();
		bool is_working() const;
	protected:
		void * run();
		void pre_stop();
		bool working;
		Input_Task * current_task;
};


#endif