#ifndef write_signal_task_h
#define write_signal_task_h

#include "MT_tomography.h"
#include "tomography.h"

class Tsignal_W;

class Write_Signal_Task: public IO_Task{
	public:
		Write_Signal_Task(Tsignal_W * writer_, Tomography::signal_type type_);
		Write_Signal_Task(Tsignal_W * writer_, Tomography::signal_type type_, Task * next_task_);
		~Write_Signal_Task();
		bool do_task();
		bool can_exec();
		void update_task_list();
	protected:
		Tomography::signal_type type;
		Tsignal_W * writer;
};

#endif