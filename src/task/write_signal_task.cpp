#define write_signal_task_cpp

#include "task/write_signal_task.h"

#include "Tsignal_W.h"

Write_Signal_Task::Write_Signal_Task(Tsignal_W * writer_, Tomography::signal_type type_): IO_Task(){
	writer = writer_;
	type = type_;
}
Write_Signal_Task::Write_Signal_Task(Tsignal_W * writer_, Tomography::signal_type type_, Task * next_task_): IO_Task(next_task_){
	writer = writer_;
	type = type_;
}
Write_Signal_Task::~Write_Signal_Task(){

}
bool Write_Signal_Task::do_task(){
	if(type == Tomography::raw){
		struct raw_data current_data = Tomography::get_instance()->get_next_raw_data();
		if(current_data.Nevent<0) return false;
		pthread_mutex_lock(&IO_mutex);
		writer->fillTree_raw(current_data.Nevent, current_data.evttime, current_data.strip_data);
		pthread_mutex_unlock(&IO_mutex);
		return true;
	}
	else{
		struct ped_data current_data;
		if(type == Tomography::ped) current_data = Tomography::get_instance()->get_next_ped_data();
		else if(type == Tomography::corr) current_data = Tomography::get_instance()->get_next_corr_data();
		else return false;
		if(current_data.Nevent<0) return false;
		pthread_mutex_lock(&IO_mutex);
		if(type == Tomography::ped) writer->fillTree_ped(current_data.strip_data);
		else if(type == Tomography::corr) writer->fillTree_corr(current_data.strip_data);
		pthread_mutex_unlock(&IO_mutex);
		return true;
	}
}
bool Write_Signal_Task::can_exec(){
	bool is_empty;
	if(type == Tomography::raw) is_empty = Tomography::get_instance()->is_raw_data_empty();
	else if(type == Tomography::ped) is_empty = Tomography::get_instance()->is_ped_data_empty();
	else if(type == Tomography::corr) is_empty = Tomography::get_instance()->is_corr_data_empty();
	else is_empty = true;
	return ((writer!=NULL) && !is_empty);
}
void Write_Signal_Task::update_task_list(){
	add_task(next_task);
}