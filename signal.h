#ifndef signal_h
#define signal_h
#include "Tsignal.h"
#include "detector.h"

#include <TProfile.h>

#include <string>
#include <map>

using std::string;
using std::map;

class Signal: public Tsignal, public CosmicBench{
	public:
		Signal(string configFilePath);
		~Signal();
		void MultiCluster();
		void HoughTracking(int event_nb);
		map<int,TProfile*> SignalOverNoise();
	protected:
		string analyseTree;
		bool use_srf;
};
#endif