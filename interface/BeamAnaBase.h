#ifndef BeamAnaBase_h
#define BeamAnaBase_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>
#include <TTree.h>
#include "TSystem.h"

#include <iostream>
#include <utility>
#include <vector>
#include <iomanip>
#include<stdint.h>
#include <map>
#include <string>
#include <fstream>


//#include "DataFormats.h"
#include "Histogrammer.h"
#include "Event.h"
#include "AlignmentParameters.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::map;
using std::cerr;


class BeamAnaBase {

  public :
    BeamAnaBase();
    virtual ~BeamAnaBase();
    
    bool setInputFile(const std::string& fname);
    bool branchFound(const string& b);
    void setAddresses();
    void setDetChannelVectors();

    TTree* analysisTree() const{ return analysisTree_; } 
    tbeam::Event* event() const { return event_; }

    //int stubWindow()  const { return sw_;}
    //int cbcClusterWidth()  const { return cwd_;}
    //int cbcOffset1() const {return offset1_;}
    //int cbcOffset2() const {return offset2_;}

    
    bool hasTelescope() const { return hasTelescope_; }
    double resDUT() const { return residualSigmaDUT_; }
    double resfei4x() const { return alPars_.residualSigmaFEI4x(); }
    double resfei4y() const { return alPars_.residualSigmaFEI4y(); }
    double offsetfei4x() const { return alPars_.offsetFEI4x(); }
    double offsetfei4y() const { return alPars_.offsetFEI4y(); }
    double nstrips() const { return nStrips_; }
    double dutpitch() const { return pitchDUT_; }
    double dutangle() const { return alPars_.theta(); }
    double sensordeltaZ() const { return alPars_.deltaZ(); }
    
    virtual void beginJob();
    virtual void endJob();
    virtual void eventLoop() = 0; 
    virtual void bookHistograms();
    virtual void clearEvent();
    virtual bool readJob(const std::string jfile);
    
    void getCbcConfig(uint32_t cwdWord, uint32_t windowWord);
    //void getExtrapolatedTracks(std::vector<tbeam::Track>& fidTkColl);
    void readChannelMaskData(const std::string cmaskF);
    void setTelMatching(const bool mtel);
    void setChannelMasking(const std::string cFile);
    bool doTelMatching() const { return doTelMatching_;}
    bool doChannelMasking() const { return doChannelMasking_;}
    
    //std::map<std::string,std::vector<int> >* getMaskedChannelMap() const {return dut_maskedChannels_;}
    void readAlignmentConstant(const std::string& aFname);
    tbeam::alignmentPars aLparameteres() const { return alPars_; }
    
    //bool isTrkfiducial(const double xtrk0Pos, const double xtrk1Pos, const double ytrk0Pos, const double ytrk1Pos);
    Histogrammer* outFile() { return hout_; }
    
    void fillCommonHistograms();
    std::map<std::string,std::string> jobCardmap() const { return jobCardmap_;}
    std::string inFile() { return iFilename_; }    
    unsigned long int getMaxEvt() { return maxEvent_; }
    
    /*
      bool CheckFiducial( double xTrk , int pStripMin , int pStripMax ,  const std::string det) 
      {
      int iStrip = xTrk/dutpitch() + nstrips()/2.0; 
      bool isFiducial = true; 
      if( iStrip > nstrips() || iStrip < pStripMin || iStrip > pStripMax ) isFiducial = false; 
      //if( iStrip > nstrips() ) isFiducial = false; 
      if( doChannelMasking_ ) isFiducial =  std::find(dut_maskedChannels_->at(det).begin(), dut_maskedChannels_->at(det).end(), iStrip) ==    dut_maskedChannels_->at(det).end();
      return isFiducial;
      }*/
    
 private :
    std::string iFilename_;
    std::string outFilename_;
    std::string chmaskFilename_;
    TFile* fin_;
    TTree *analysisTree_; 
    tbeam::Event* event_;
    
    bool hasTelescope_;
    bool doTelMatching_;
    bool doChannelMasking_;
    unsigned long int maxEvent_=0;
    
    tbeam::alignmentPars  alPars_;
    std::map<std::string,std::string> jobCardmap_;
    std::map<std::string,std::vector<int> >* dut_maskedChannels_;
    
    Histogrammer* hout_;
    double residualSigmaDUT_;
    double residualSigmaFEI4x_;
    double residualSigmaFEI4y_;
    double offsetFEI4x_;
    double offsetFEI4y_;

    int nStrips_;
    double pitchDUT_;
};
#endif
