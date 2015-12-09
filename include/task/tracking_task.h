#ifndef tracking_task_h
#define tracking_task_h

#include "MT_tomography.h"

class CosmicBench;

class Tracking_Abs_Task: public Task{
	public:
		Tracking_Abs_Task(const CosmicBench * const detectors_);
		Tracking_Abs_Task(const CosmicBench * const detectors_, Task * next_task_);
		~Tracking_Abs_Task();
		bool do_task();
		bool can_exec();
		void update_task_list();
	protected:
		const CosmicBench * detectors;
};
class Tracking_Dev_Task: public Task{
	public:
		Tracking_Dev_Task(const CosmicBench * const detectors_);
		Tracking_Dev_Task(const CosmicBench * const detectors_, Task * next_task_);
		~Tracking_Dev_Task();
		bool do_task();
		bool can_exec();
		void update_task_list();
	protected:
		const CosmicBench * detectors;
};

#endif