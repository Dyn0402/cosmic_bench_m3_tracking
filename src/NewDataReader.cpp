#include <string>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "tomography.h"

using std::string;
using std::cout;
using std::endl;

using boost::property_tree::ptree;

int main(int argc, char ** argv){
	if(argc<2){
		cout << "You must indicate a config file which contains the Run caracs" << endl;
		return 1;
	}

	string config_file = argv[1];
	ptree config_tree;
	read_json(config_file, config_tree);
	Tomography::process_elec_files(config_tree);
	return 0;
}