#define dataline_cpp
#include "dataline.h"
#include "mygblink.h"
#include <arpa/inet.h>

DataLine::DataLine(){
	data = 0;
}
DataLine::~DataLine(){

}
void DataLine::ntohs_(){
	data = ntohs(data);
}
bool DataLine::is_final_trailer() const{
	return GET_TYPE(data)==7;
}
bool DataLine::is_first_line() const{
	return GET_TYPE(data)==3;
}
bool DataLine::is_data() const{
	return GET_TYPE(data)==0;
}
bool DataLine::is_channel_ID() const{
	return GET_TYPE(data)==1;
}
int DataLine::get_dream_ID() const{
	return GET_DREAMID(data);
}
int DataLine::get_channel_ID() const{
	return GET_CHANNELID(data);
}
float DataLine::get_data() const{
	return GET_DATA(data);
}