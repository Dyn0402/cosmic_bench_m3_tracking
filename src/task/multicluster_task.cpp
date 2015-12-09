#define multicluster_task_cpp

#include "task/multicluster_task.h"

#include "detector.h"
#include "event.h"

Multicluster_Task::Multicluster_Task(const CosmicBench * const detectors_): Task(){
	detectors = detectors_;
}
Multicluster_Task::Multicluster_Task(const CosmicBench * const detectors_, Task * next_task_): Task(next_task_){
	detectors = detectors_;
}
Multicluster_Task::~Multicluster_Task(){

}
bool Multicluster_Task::do_task(){
	struct ped_data current_data = Tomography::get_instance()->get_next_corr_data();
	if(current_data.Nevent < 0) return false;
	struct event_data demux_data;
	demux_data.Nevent = current_data.Nevent;
	demux_data.evttime = current_data.evttime;
	for(int i=0;i<detectors->get_det_N_tot();i++){
		Detector * det = detectors->get_detector(i);
		demux_data.det_data[det->get_type()].push_back(det->build_event(current_data.strip_data[det->get_type()][det->get_n_in_tree()],demux_data.Nevent,demux_data.evttime));
		(demux_data.det_data[det->get_type()].back())->MultiCluster();
	}
	Tomography::get_instance()->push_next_event_data(demux_data);
	return true;
}
bool Multicluster_Task::can_exec(){
	return (!(Tomography::get_instance()->is_corr_data_empty()));
}
void Multicluster_Task::update_task_list(){
	add_task(next_task);
}