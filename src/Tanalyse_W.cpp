#define Tanalyse_W_cxx
#include "Tanalyse_W.h"
#include <iostream>

using std::cout;
using std::endl;
/*
Tanalyse_W::Tanalyse_W(string saveFileName){
   saveFile = new TFile(saveFileName.c_str(),"RECREATE");
}
*/
Tanalyse_W::Tanalyse_W(string saveFileName, map<Tomography::det_type,unsigned short> det_N_)
{
   saveFile = new TFile(saveFileName.c_str(),"RECREATE");
   T = new TTree("T","event");
   T->SetMaxTreeSize(10000000000000LL);
   det_N = det_N_;
   Init();
}

Tanalyse_W::~Tanalyse_W()
{
   if(det_N[Tomography::CM]>0){
      delete CM_NClus;
      delete CM_Spark;
      delete CM_ClusAmpl;
      delete CM_ClusSize;
      delete CM_ClusPos;
      delete CM_ClusTOT;
      delete CM_ClusT;
      delete CM_ClusMaxStrip;
      delete CM_ClusMaxSample;
      delete CM_ClusMaxStripAmpl;
      delete CM_StripMaxAmpl;
   }
   if(det_N[Tomography::MG]>0){
      delete MG_NClus;
      delete MG_Spark;
      delete MG_ClusAmpl;
      delete MG_ClusSize;
      delete MG_ClusPos;
      delete MG_ClusTOT;
      delete MG_ClusT;
      delete MG_ClusMaxStrip;
      delete MG_ClusMaxSample;
      delete MG_ClusMaxStripAmpl;
      delete MG_StripMaxAmpl;
   }
   //delete T;
   delete saveFile;
}

void Tanalyse_W::Init()
{
   T->Branch("evn", &evn, "evn/I");
   T->Branch("evttime", &evttime, "evttime/D");
   //Tout->Branch("finetstp", &finetstp, "finetstp/I");

   if(det_N[Tomography::MG]>0){
      MG_NClus = new int[det_N[Tomography::MG]];
      MG_Spark = new int[det_N[Tomography::MG]];
      MG_ClusAmpl = new Double_t[det_N[Tomography::MG]][300];
      MG_ClusSize = new Double_t[det_N[Tomography::MG]][300];
      MG_ClusPos = new Double_t[det_N[Tomography::MG]][300];
      MG_ClusMaxStripAmpl = new Double_t[det_N[Tomography::MG]][300];
      MG_ClusMaxStrip = new Int_t[det_N[Tomography::MG]][300];
      MG_ClusMaxSample = new Double_t[det_N[Tomography::MG]][300];
      MG_ClusTOT = new Double_t[det_N[Tomography::MG]][300];
      MG_ClusT = new Double_t[det_N[Tomography::MG]][300];
      MG_StripMaxAmpl = new Double_t[det_N[Tomography::MG]][MG_Detector::Nchannel];

      int MG_MaxNClus = 300;

      char leefMGNClus[100];
      sprintf(leefMGNClus,"MG_NClus[%d]/I",det_N[Tomography::MG]);
      T->Branch("MG_NClus", MG_NClus, leefMGNClus);

      char leefMG_Spark[100];
      sprintf(leefMG_Spark,"MG_Spark[%d]/I",det_N[Tomography::MG]);
      T->Branch("MG_Spark", MG_Spark, leefMG_Spark);

      char leefMG_ClusAmpl[100];
      sprintf(leefMG_ClusAmpl,"MG_ClusAmpl[%d][%d]/D",det_N[Tomography::MG],MG_MaxNClus);
      T->Branch("MG_ClusAmpl", MG_ClusAmpl, leefMG_ClusAmpl);

      char leefMG_ClusSize[100];
      sprintf(leefMG_ClusSize,"MG_ClusSize[%d][%d]/D",det_N[Tomography::MG],MG_MaxNClus);
      T->Branch("MG_ClusSize", MG_ClusSize, leefMG_ClusSize);

      char leefMG_ClusPos[100];
      sprintf(leefMG_ClusPos,"MG_ClusPos[%d][%d]/D",det_N[Tomography::MG],MG_MaxNClus);
      T->Branch("MG_ClusPos", MG_ClusPos, leefMG_ClusPos);

      char leefMG_ClusMaxStripAmpl[100];
      sprintf(leefMG_ClusMaxStripAmpl,"MG_ClusMaxStripAmpl[%d][%d]/D",det_N[Tomography::MG],MG_MaxNClus);
      T->Branch("MG_ClusMaxStripAmpl", MG_ClusMaxStripAmpl, leefMG_ClusMaxStripAmpl);

      char leefMG_ClusMaxSample[100];
      sprintf(leefMG_ClusMaxSample,"MG_ClusMaxSample[%d][%d]/D",det_N[Tomography::MG],MG_MaxNClus);
      T->Branch("MG_ClusMaxSample", MG_ClusMaxSample, leefMG_ClusMaxSample);

      char leefMG_ClusTOT[100];
      sprintf(leefMG_ClusTOT,"MG_ClusTOT[%d][%d]/D",det_N[Tomography::MG],MG_MaxNClus);
      T->Branch("MG_ClusTOT", MG_ClusTOT, leefMG_ClusTOT);

      char leefMG_ClusT[100];
      sprintf(leefMG_ClusT,"MG_ClusT[%d][%d]/D",det_N[Tomography::MG],MG_MaxNClus);
      T->Branch("MG_ClusT", MG_ClusT, leefMG_ClusT);

      char leefMG_StripMaxAmpl[100];
      sprintf(leefMG_StripMaxAmpl,"MG_StripMaxAmpl[%d][%d]/D",det_N[Tomography::MG],61);
      T->Branch("MG_StripMaxAmpl", MG_StripMaxAmpl, leefMG_StripMaxAmpl);

      char leefMG_ClusMaxStrip[100];
      sprintf(leefMG_ClusMaxStrip,"MG_ClusMaxStri[%d][%d]/I",det_N[Tomography::MG],MG_MaxNClus);
      T->Branch("MG_ClusMaxStrip", MG_ClusMaxStrip, leefMG_ClusMaxStrip);
   }

   if(det_N[Tomography::CM]>0){
      CM_NClus = new int[det_N[Tomography::CM]];
      CM_Spark = new int[det_N[Tomography::CM]];
      CM_ClusAmpl = new Double_t[det_N[Tomography::CM]][600];
      CM_ClusSize = new Double_t[det_N[Tomography::CM]][600];
      CM_ClusPos = new Double_t[det_N[Tomography::CM]][600];
      CM_ClusMaxStripAmpl = new Double_t[det_N[Tomography::CM]][600];
      CM_ClusMaxSample = new Double_t[det_N[Tomography::CM]][600];
      CM_ClusTOT = new Double_t[det_N[Tomography::CM]][600];
      CM_ClusT = new Double_t[det_N[Tomography::CM]][600];
      CM_StripMaxAmpl = new Double_t[det_N[Tomography::CM]][CM_Detector::Nchannel/2];

      int CM_MaxNClus = 600;

      char leefCMNClus[100];
      sprintf(leefCMNClus,"CM_NClus[%d]/I",det_N[Tomography::CM]);
      T->Branch("CM_NClus", CM_NClus, leefCMNClus);

      char leefCM_Spark[100];
      sprintf(leefCM_Spark,"CM_Spark[%d]/I",det_N[Tomography::CM]);
      T->Branch("CM_Spark", CM_Spark, leefCM_Spark);

      char leefCM_ClusAmpl[100];
      sprintf(leefCM_ClusAmpl,"CM_ClusAmpl[%d][%d]/D",det_N[Tomography::CM],CM_MaxNClus);
      T->Branch("CM_ClusAmpl", CM_ClusAmpl, leefCM_ClusAmpl);

      char leefCM_ClusSize[100];
      sprintf(leefCM_ClusSize,"CM_ClusSize[%d][%d]/D",det_N[Tomography::CM],CM_MaxNClus);
      T->Branch("CM_ClusSize", CM_ClusSize, leefCM_ClusSize);

      char leefCM_ClusPos[100];
      sprintf(leefCM_ClusPos,"CM_ClusPos[%d][%d]/D",det_N[Tomography::CM],CM_MaxNClus);
      T->Branch("CM_ClusPos", CM_ClusPos, leefCM_ClusPos);

      char leefCM_ClusMaxStripAmpl[100];
      sprintf(leefCM_ClusMaxStripAmpl,"CM_ClusMaxStripAmpl[%d][%d]/D",det_N[Tomography::CM],CM_MaxNClus);
      T->Branch("CM_ClusMaxStripAmpl", CM_ClusMaxStripAmpl, leefCM_ClusMaxStripAmpl);

      char leefCM_ClusMaxStrip[100];
      sprintf(leefCM_ClusMaxStrip,"CM_ClusMaxStrip[%d][%d]/D",det_N[Tomography::CM],CM_MaxNClus);
      T->Branch("CM_ClusMaxStrip", CM_ClusMaxStrip, leefCM_ClusMaxStrip);

      char leefCM_ClusMaxSample[100];
      sprintf(leefCM_ClusMaxSample,"CM_ClusMaxSample[%d][%d]/D",det_N[Tomography::CM],CM_MaxNClus);
      T->Branch("CM_ClusMaxSample", CM_ClusMaxSample, leefCM_ClusMaxSample);

      char leefCM_ClusTOT[100];
      sprintf(leefCM_ClusTOT,"CM_ClusTOT[%d][%d]/D",det_N[Tomography::CM],CM_MaxNClus);
      T->Branch("CM_ClusTOT", CM_ClusTOT, leefCM_ClusTOT);

      char leefCM_ClusT[100];
      sprintf(leefCM_ClusT,"CM_ClusT[%d][%d]/D",det_N[Tomography::CM],CM_MaxNClus);
      T->Branch("CM_ClusT", CM_ClusT, leefCM_ClusT);

      char leefCM_StripMaxAmpl[100];
      sprintf(leefCM_StripMaxAmpl,"CM_StripMaxAmpl[%d][%d]/D",det_N[Tomography::CM],32);
      T->Branch("CM_StripMaxAmpl", CM_StripMaxAmpl, leefCM_StripMaxAmpl);
   }
}
TTree * Tanalyse_W::getTree() const{
   return T->CloneTree();
}
void Tanalyse_W::Write(){
   saveFile->cd();
   T->Write();
}
void Tanalyse_W::CloseFile(){
   saveFile->Close();
}
void Tanalyse_W::fillTree(int evn_, double evttime_, map<Tomography::det_type,vector<Event*> > events){
   evn = evn_;
   evttime = evttime_;

   for(map<Tomography::det_type,vector<Event*> >::iterator type_it=events.begin();type_it!=events.end();++type_it){
      if((type_it->second).size() != det_N[type_it->first]){
         cout << "problem in event number" << endl;
         return;
      }
      if(type_it->first == Tomography::MG){
         for(vector<Event*>::iterator event_it=(type_it->second).begin();event_it!=(type_it->second).end();++event_it){
            int i = (*event_it)->get_n_in_tree();
            MG_NClus[i] = (*event_it)->get_NClus();
            vector<Cluster*> current_clusters = (*event_it)->get_clusters();
            if(current_clusters.size()!=static_cast<unsigned int>(MG_NClus[i])){
               cout << "problem in cluster number" << endl;
               return;
            }
            for(int j=0;j<MG_NClus[i];j++){
               MG_ClusAmpl[i][j] = current_clusters[j]->get_ampl();
               MG_ClusT[i][j] = current_clusters[j]->get_t();
               MG_ClusTOT[i][j] = current_clusters[j]->get_TOT();
               MG_ClusPos[i][j] = current_clusters[j]->get_pos();
               MG_ClusSize[i][j] = current_clusters[j]->get_size();
               MG_ClusMaxSample[i][j] = current_clusters[j]->get_maxSample();
               MG_ClusMaxStripAmpl[i][j] = current_clusters[j]->get_maxStripAmpl();
               MG_ClusMaxStrip[i][j] = current_clusters[j]->get_maxStrip();
            }
            for(int j=MG_NClus[i];j<300;j++){
               MG_ClusAmpl[i][j] = 0;
               MG_ClusT[i][j] = 0;
               MG_ClusTOT[i][j] = 0;
               MG_ClusPos[i][j] = 0;
               MG_ClusSize[i][j] = -1;
               MG_ClusMaxSample[i][j] = 0;
               MG_ClusMaxStripAmpl[i][j] = 0;
               MG_ClusMaxStrip[i][j] = 0;
            }
         }
      }
      if(type_it->first == Tomography::CM){
         for(vector<Event*>::iterator event_it=(type_it->second).begin();event_it!=(type_it->second).end();++event_it){
            int i = (*event_it)->get_n_in_tree();
            CM_NClus[i] = (*event_it)->get_NClus();
            vector<Cluster*> current_clusters = (*event_it)->get_clusters();
            if(current_clusters.size()!=static_cast<unsigned int>(CM_NClus[i])){
               cout << "problem in cluster number" << endl;
               return;
            }
            for(int j=0;j<CM_NClus[i];j++){
               CM_ClusAmpl[i][j] = current_clusters[j]->get_ampl();
               CM_ClusT[i][j] = current_clusters[j]->get_t();
               CM_ClusTOT[i][j] = current_clusters[j]->get_TOT();
               CM_ClusPos[i][j] = current_clusters[j]->get_pos();
               CM_ClusSize[i][j] = current_clusters[j]->get_size();
               CM_ClusMaxSample[i][j] = current_clusters[j]->get_maxSample();
               CM_ClusMaxStripAmpl[i][j] = current_clusters[j]->get_maxStripAmpl();
               CM_ClusMaxStrip[i][j] = current_clusters[j]->get_maxStrip();
            }
            for(int j=CM_NClus[i];j<300;j++){
               CM_ClusAmpl[i][j] = 0;
               CM_ClusT[i][j] = 0;
               CM_ClusTOT[i][j] = 0;
               CM_ClusPos[i][j] = 0;
               CM_ClusSize[i][j] = -1;
               CM_ClusMaxSample[i][j] = 0;
               CM_ClusMaxStripAmpl[i][j] = 0;
               CM_ClusMaxStrip[i][j] = 0;
            }
         }
      }
   }
   T->Fill();
}