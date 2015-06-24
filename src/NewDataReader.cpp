#include <string>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "datareader.h"
#include "Tanalyse_W.h"
#include "event.h"
#include "tomography.h"

using std::string;
using std::cout;
using std::endl;
using std::flush;

using boost::property_tree::ptree;

int main(int argc, char ** argv){
	if(argc<2){
		cout << "You must indicate a config file which contains the Run caracs" << endl;
		return 1;
	}
	enum process_type{
		data_run,
		ped_run,
		analysis
	};
	process_type operation = data_run;
	for(int i=2;i<argc;i++){
		if(argv[i] == string("ped")){
			operation = ped_run;
		}
		else if(argv[i] == string("data")){
			operation = data_run;
		}
		else if(argv[i] == string("analyse")){
			operation = analysis;
		}
	}
	string config_file = argv[1];
	ptree config_tree;
	read_json(config_file, config_tree);
	if(operation != analysis){
		DataReader blah(config_tree,true);
		blah.process();
		if(operation == ped_run) blah.compute_ped();
		blah.read_ped();
		blah.do_ped_sub();
		blah.do_common_noise_sub();
		if(operation == ped_run) blah.compute_RMSPed();
	}
	else{
		Tanalyse_W * analysisFile = new Tanalyse_W(config_tree.get<string>("Tree"),config_tree.get<int>("total_CM_N"),config_tree.get<int>("total_MG_N"));
		DataReader blah(config_tree,false);
		blah.read_ped();
		CosmicBench * bench = new CosmicBench(config_tree);
		int total_det = bench->get_CM_N() + bench->get_MG_N();
		long event_nb = 0;
		int Nevent = 0;
		double evttime = 0;
		while(!(blah.is_end())){
			if((event_nb%100) == 0) cout << "\r" << "event processed : " << event_nb << flush;
			blah.process_event();
			Nevent = blah.get_event_n();
			evttime = blah.get_evttime();
			blah.do_ped_CMN_sub_event();
			map<Tomography::det_type,vector<vector<vector<float> > > > current_data = blah.get_data();
			map<Tomography::det_type,vector<vector<vector<double> > > > current_data_d;
			for(map<Tomography::det_type,vector<vector<vector<float> > > >::iterator type_it = current_data.begin();type_it!=current_data.end();++type_it){
				current_data_d[type_it->first] = vector<vector<vector<double> > >((type_it->second).size());
				for(unsigned int i=0;i<(type_it->second).size();i++){
					current_data_d[type_it->first][i] = vector<vector<double> >((type_it->second)[i].size());
					for(unsigned int j=0;j<(type_it->second)[i].size();j++){
						current_data_d[type_it->first][i][j] = vector<double>((type_it->second)[i][j].size());
						for(unsigned int k=0;k<(type_it->second)[i][j].size();k++){
							current_data_d[type_it->first][i][j][k] = (type_it->second)[i][j][k];
						}
					}
				}
			}
			vector<CM_Event> cm_events;
			vector<MG_Event> mg_events;
			for(int i=0;i<total_det;i++){
				Detector * det = bench->get_detector(i);
				if(det->get_type() == Tomography::MG){
					MG_Detector * current_det = dynamic_cast<MG_Detector*>(det);
					mg_events.push_back(MG_Event(*current_det,current_data_d[Tomography::MG][current_det->get_mg_n_in_tree()],false,event_nb));
					(mg_events.back()).MultiCluster();
				}
				else if(det->get_type() == Tomography::CM){
					CM_Detector * current_det = dynamic_cast<CM_Detector*>(det);
					cm_events.push_back(CM_Event(*current_det,current_data_d[Tomography::CM][current_det->get_cm_n_in_tree()],false,event_nb));
					(cm_events.back()).MultiCluster();
				}
			}
			analysisFile->fillTree(Nevent,evttime,mg_events,cm_events);
			event_nb++;
		}
		cout << "\r" << "event processed : " << event_nb << endl;
		analysisFile->Write();
		analysisFile->CloseFile();
		delete analysisFile;
		delete bench;
	}
	return 0;
}