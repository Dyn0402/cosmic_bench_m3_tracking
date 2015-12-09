#define tomography_MT_cpp

#include "MT_tomography.h"

#include "datareader.h"
#include "detector.h"
#include "event.h"
#include "Tsignal_R.h"
#include "Tanalyse_R.h"
#include "Tray.h"
#include "Tsignal_W.h"
#include "Tanalyse_W.h"

#include <vector>

using std::vector;

Task::Task(){
	next_task = NULL;
}
Task::Task(Task * next_task_){
	next_task = next_task_;
}
Task::~Task(){

}
Task * Task::get_next_task(){
	Task * priority_task;
	pthread_mutex_lock(&queue_mutex);
	if(task_queue.empty()) priority_task = NULL;
	else{
		priority_task = task_queue.front();
		task_queue.pop();
	}
	pthread_mutex_unlock(&queue_mutex);
	return priority_task;
}
bool Task::is_init = false;
pthread_mutex_t Task::queue_mutex;
queue<Task*> Task::task_queue;

void Task::add_task(Task * new_task){
	if(!is_init){
		pthread_mutex_init(&queue_mutex, NULL);
		is_init = true;
	}
	pthread_mutex_lock(&queue_mutex);
	task_queue.push(new_task);
	pthread_mutex_unlock(&queue_mutex);
}
unsigned int Task::task_left(){
	return task_queue.size();
}
IO_Task::IO_Task(): Task(){
	pthread_mutex_init(&IO_mutex, NULL);
}
IO_Task::IO_Task(Task * next_task_): Task(next_task_){
	pthread_mutex_init(&IO_mutex, NULL);
}
IO_Task::~IO_Task(){
	pthread_mutex_destroy(&IO_mutex);
}

Input_Task::Input_Task(): IO_Task(){

}
Input_Task::Input_Task(Task * next_task_): IO_Task(next_task_){

}
Input_Task::~Input_Task(){

}
void Input_Task::update_task_list(){
	add_task(next_task);
	add_task(this);
}

Thread::Thread(){
	id = 0;
	running = 0;
	detached = 0;
}
Thread::~Thread(){
	if(running ==1 && detached == 0) pthread_detach(id);
	if(running == 1) pthread_cancel(id);
}
void * Thread::runThread(void * arg){
	return ((Thread*) arg)->run();
}
int Thread::start(){
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	int result = pthread_create(&id,NULL,runThread,this);
	if(result == 0) running = 1;
	pthread_attr_destroy(&attr);
	return result;
}
void Thread::pre_stop(){

}
int Thread::stop(){
	pre_stop();
	if(running == 0){
		return -1;
	}
	void * status;
	int result = pthread_join(id,&status);
	if(result == 0) running = 0;
	return result;
}
pthread_t Thread::getThreadId() const{
	return id;
}

Worker_Thread::Worker_Thread(): Thread(){
	working = false;
	current_task = NULL;
}
Worker_Thread::~Worker_Thread(){
	working = false;
	current_task = NULL;
}
void * Worker_Thread::run(){
	working = true;
	unsigned int wait_time = 0;
	while(working){
		current_task = Task::get_next_task();
		if(current_task!=NULL){
			wait_time = 0;
			while(!(current_task->can_exec())) usleep(100);
			if(current_task->do_task()) current_task->update_task_list();
		}
		else{
			usleep(100);
			wait_time++;
			if(wait_time>50000) working = false;
		}
	}
	return 0;
}
void Worker_Thread::pre_stop(){
	working = false;
}
bool Worker_Thread::is_working() const{
	return working;
}