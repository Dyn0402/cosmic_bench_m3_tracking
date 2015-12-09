#ifndef ped_task_h
#define ped_task_h

#include "MT_tomography.h"
#include "tomography.h"

#include <vector>
#include <map>

using std::vector;
using std::map;

class Ped_Task: public Task{
	public:
		Ped_Task(map<Tomography::det_type,vector<vector<float> > > ped_);
		Ped_Task(map<Tomography::det_type,vector<vector<float> > > ped_, Task * next_task_);
		~Ped_Task();
		bool do_task();
		bool can_exec();
		void update_task_list();
	protected:
		map<Tomography::det_type,vector<vector<float> > > ped;
};
class Corr_Task: public Task{
	public:
		Corr_Task();
		Corr_Task(Task * next_task_);
		~Corr_Task();
		bool do_task();
		bool can_exec();
		void update_task_list();
};
class Ped_Corr_Task: public Task{
	public:
		Ped_Corr_Task(map<Tomography::det_type,vector<vector<float> > > ped_);
		Ped_Corr_Task(map<Tomography::det_type,vector<vector<float> > > ped_, Task * next_task_);
		~Ped_Corr_Task();
		bool do_task();
		bool can_exec();
		void update_task_list();
	protected:
		map<Tomography::det_type,vector<vector<float> > > ped;
};

#endif