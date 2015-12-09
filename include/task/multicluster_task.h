#ifndef multicluster_task_h
#define multicluster_task_h

#include "MT_tomography.h"

class CosmicBench;

class Multicluster_Task: public Task{
	public:
		Multicluster_Task(const CosmicBench * const detectors_);
		Multicluster_Task(const CosmicBench * const detectors_, Task * next_task_);
		~Multicluster_Task();
		bool do_task();
		bool can_exec();
		void update_task_list();
	protected:
		const CosmicBench * detectors;
};

#endif