#ifndef read_elec_task_h
#define read_elec_task_h

#include "MT_tomography.h"

class DataReader;

class Read_Elec_Task: public Input_Task{
	public:
		Read_Elec_Task(DataReader * reader_);
		Read_Elec_Task(DataReader * reader_, Task * next_task_);
		~Read_Elec_Task();
		bool do_task();
		bool can_exec();
	protected:
		DataReader * reader;
};

#endif