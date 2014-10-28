#include "detector.h"
#include "event.h"
#include "Tanalyse.h"
#include "Tsignal.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <utility>
#include "TRint.h"

using std::ostringstream;
using boost::property_tree::ptree;
using std::cout;
using std::endl;
using std::flush;
using std::vector;
using std::ifstream;
using std::string;
using std::map;
using std::pair;
using std::cin;

int main(int argc, char ** argv){
	if(argc<2){
		cout << "You must indicate a config file which contains the Cosmic Bench caracs" << endl;
		return 0;
	}
	
	int n = 100;
	char path[n];
	ostringstream config_file;
	config_file << getcwd(path,n) << "/" << argv[1];
	ptree config_tree;
	read_json(config_file.str(), config_tree);
	cout << "source file : " << config_tree.get<string>("signal_file") << endl;
	cout << "destination file : " << config_tree.get<string>("Tree") << endl;
	cout << "RMSPed file : " << config_tree.get<string>("RMSPed") << endl;
	TFile *fIn = new TFile((config_tree.get<string>("signal_file")).c_str(),"READ");
	TTree * treeIn = (TTree*)(fIn->Get("T"));
	int CM_N = 0;
	int MG_N = 0;
	int total_CM_N = config_tree.get<int>("total_CM_N");
	int total_MG_N = config_tree.get<int>("total_MG_N");
	map<int, MG_Detector> MG_detectors;
	//map<int, CM_Detector> CM_detectors;
	ifstream in;
	in.open((config_tree.get<string>("RMSPed")).c_str());
	int rms_strip, det;
	//double RMS[Nstrip_MG*total_MG_N];
	vector<vector<double> > RMS;
	vector<double> empty_vector(61,0);
	for(int i=0;i<total_MG_N;i++){
		RMS.push_back(empty_vector);
	}
	int nlines=0;
	while (1) { // read the text file
		int det_n = nlines/61;
		int strip_n = nlines%61;
		if(det_n>(total_MG_N-1)) break;
		double current_rms;
		in >> det >> rms_strip >> current_rms;
		RMS[det_n][strip_n] = current_rms;
		if (!in.good()) break;
		nlines++;
	}
	in.close();
	BOOST_FOREACH(const ptree::value_type& child, config_tree.get_child("CosmicBench.MultiGens")){
		
		MG_detectors.insert(pair<int,MG_Detector>(child.second.get<int>("mg_n"),MG_Detector(child.second.get<double>("z"),child.second.get<bool>("is_X"),child.second.get<bool>("is_up"),child.second.get<int>("mg_n"),child.second.get<bool>("is_ref"),child.second.get<double>("offset"),child.second.get<bool>("direction"),child.second.get<double>("angle"))));
		MG_detectors[child.second.get<int>("mg_n")].set_RMS(RMS[child.second.get<int>("mg_n")]);
		MG_detectors[child.second.get<int>("mg_n")].set_SRF(child.second.get<double>("SRF.offset"),child.second.get<double>("SRF.gauss"),child.second.get<double>("SRF.lorentz"),child.second.get<double>("SRF.ratio"));
		MG_N++;
	}
	if((total_CM_N!=CM_N) || (total_MG_N!=MG_N)){
		cout << "problem in detectors number" << endl;
		return 0;
	}
	if(total_CM_N>0){
		cout << "cosmulti not implemented" << endl;
		return 0;
	}
	Tsignal * signalTree = new Tsignal(treeIn,CM_N,MG_N);
	Tanalyse * analyseTree = new Tanalyse(config_tree.get<string>("Tree"),CM_N,MG_N);
	long nentries = signalTree->GetEntriesFast();
	for(int i=0;i<nentries;i++){
		signalTree->LoadTree(i);
		signalTree->GetEntry(i);
		MG_Event mg_events[MG_N];
		CM_Event cm_events[CM_N];
		for(int j=0;j<MG_N;j++){
			mg_events[j] = MG_Event(MG_detectors[j],signalTree->get_mg_ampl(j),config_tree.get<bool>("use_SRF"));
			mg_events[j].MultiCluster();
		}
		/*
		for(int j=0;j<CM_N;j++){
			cm_events[j] = CM_Event(CM_detectors[j],signalTree->get_cm_ampl(j),config_tree.get<bool>("use_SRF"));
			cm_events[j].MultiCluster();
		}
		*/
		analyseTree->fillTree(signalTree->Nevent,0,mg_events,cm_events);
		if(i%100 == 0) cout << "\r" << i << flush;
	}
	cout << "\r" << nentries << endl;
	analyseTree->Write();
	analyseTree->CloseFile();
	//delete signalTree; delete fIn; delete analyseTree;
	return 1;
}