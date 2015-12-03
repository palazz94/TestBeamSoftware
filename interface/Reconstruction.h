#ifndef Reconstruction_h
#define Reconstruction_h

#include "tBeamBase.h"

#include "TString.h"
#include "Utility.h"
class TH1;

using Utility::Cluster;
class Reconstruction : public tBeamBase {
 public:
  Reconstruction(const string inFilename,const string outFilename,int stubWindow, bool publish);
  ~Reconstruction(){}
  void bookHistogram(TFile* fout);
  void publishPlots(TString dirName);
  void setStubWindow(int stubWindow) { stubWindow_ = stubWindow; }
  void beginJob();
  void Loop();
  void endJob();

 private:
  TString outFile;
  TFile* fout_;
  int stubWindow_;
  bool publishPng_;
  std::map<std::string,std::vector<Cluster> >  detClustermap_;
};
#endif