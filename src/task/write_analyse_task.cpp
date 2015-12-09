#define write_analyse_task_cpp

#include "task/write_analyse_task.h"

#include "Tanalyse_W.h"
#include "event.h"

Write_Analyse_Task::Write_Analyse_Task(Tanalyse_W * writer_): IO_Task(){
	writer = writer_;
}
Write_Analyse_Task::Write_Analyse_Task(Tanalyse_W * writer_, Task * next_task_): IO_Task(next_task_){
	writer = writer_;
}
Write_Analyse_Task::~Write_Analyse_Task(){

}
bool Write_Analyse_Task::do_task(){
	struct event_data current_data = Tomography::get_instance()->get_next_event_data();
	if(current_data.Nevent<0) return false;
	pthread_mutex_lock(&IO_mutex);
	writer->fillTree(current_data.Nevent, current_data.evttime, current_data.det_data);
	pthread_mutex_unlock(&IO_mutex);
	for(map<Tomography::det_type,vector<Event*> >::iterator type_it = current_data.det_data.begin();type_it!=current_data.det_data.end();++type_it){
		while((type_it->second).size()>0){
			delete (type_it->second).back();
			(type_it->second).pop_back();
		}
	}
	return true;
}
bool Write_Analyse_Task::can_exec(){
	return (writer!=NULL && (!(Tomography::get_instance()->is_event_data_empty())));
}
void Write_Analyse_Task::update_task_list(){
	add_task(next_task);
}