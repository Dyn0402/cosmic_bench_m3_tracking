#define write_rays_task_cpp

#include "task/write_rays_task.h"

#include "Tray.h"

Write_Rays_Task::Write_Rays_Task(Tray * writer_, double z_up_, double z_down_): IO_Task(){
	writer = writer_;
	z_up = z_up_;
	z_down = z_down_;
}
Write_Rays_Task::Write_Rays_Task(Tray * writer_, double z_up_, double z_down_, Task * next_task_): IO_Task(next_task_){
	writer = writer_;
	z_up = z_up_;
	z_down = z_down_;
}
Write_Rays_Task::~Write_Rays_Task(){

}
bool Write_Rays_Task::do_task(){
	struct ray_data current_data = Tomography::get_instance()->get_next_ray_data();
	if(current_data.CBevent == NULL) return false;
	pthread_mutex_lock(&IO_mutex);
	writer->fillTree((current_data.CBevent)->get_evn(), (current_data.CBevent)->get_evttime(), current_data.rays, z_up, z_down);
	pthread_mutex_unlock(&IO_mutex);
	delete current_data.CBevent;
	return true;
}
bool Write_Rays_Task::can_exec(){
	return (writer!=NULL && (!(Tomography::get_instance()->is_ray_data_empty())));
}
void Write_Rays_Task::update_task_list(){
	add_task(next_task);
}