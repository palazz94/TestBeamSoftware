/*!
  \file                BaselineAnalysis.cc
  \brief               Example user code for Analysis
  \author              Suvankar Roy Chowdhury, Rajarshi Bhattacharya
  \date                05/07/16
  Support :            mail to : suvankar.roy.chowdhury@cern.ch, rajarshi.bhattacharya@cern.ch
*/
#include "TROOT.h"
#include "TInterpreter.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TKey.h"
#include "TClass.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include <map>
#include <utility>
#include <vector>
#include <sstream>

#include "BaselineAnalysis.h"
using std::vector;
using std::map;
BaselineAnalysis::BaselineAnalysis() :
  BeamAnaBase::BeamAnaBase()
{
}
void BaselineAnalysis::bookHistograms() {
  BeamAnaBase::bookHistograms();
  for(auto& m : *modVec()){
    //std::cout << "Detidbottom=" << m.hdirbottom_ << std::endl;
    TString s = TString(m.hdirbottom_);
    hist_->bookTrackMatchHistograms(s);
    s = TString(m.hdirtop_);
    hist_->bookTrackMatchHistograms(s);
  }
}

void BaselineAnalysis::beginJob() {
  BeamAnaBase::beginJob();
  nEntries_ = analysisTree()->GetEntries();
  hist_ = outFile();
  setAddresses();
  bookHistograms();
  //analysisTree()->GetEntry(0);
  //getCbcConfig(condEv()->cwd, condEv()->window);
}

void BaselineAnalysis::eventLoop()
{
  Long64_t nbytes = 0, nb = 0;
  maxEvent_ = getMaxEvt();
  if(nEntries_ < maxEvent_ || maxEvent_ == 0) maxEvent_ = nEntries_;
  cout << "#Events=" << nEntries_ <<" -->  MAX EVENTS TO BE PROCESSED : "<< maxEvent_ <<endl;
  hist_->fillHist1D("EventInfo","nevents", nEntries_);

  long int den = 0;
  long int nClsb = 0, nClst = 0, numstub = 0;
  for (Long64_t jentry=0; jentry < maxEvent_; jentry++) {
    clearEvent();
    Long64_t ientry = analysisTree()->GetEntry(jentry);
    if (ientry < 0) break;
    if (jentry%1000 == 0) {
      cout << " Events processed. " << std::setw(8) << jentry
	   << endl;
    }
    if(jentry==0) {
      hist_->fillHist1D("EventInfo","hvSettings", event()->HVsettings);
      hist_->fillHist1D("EventInfo","dutAngle", event()->DUTangle);
      //hist_->fillHist1D("EventInfo","vcth", event()->vcth);
      //hist_->fillHist1D("EventInfo","offset", cbcOffset1());
      //hist_->fillHist1D("EventInfo","offset", cbcOffset2());
      //hist_->fillHist1D("EventInfo","window", event()->cwd);
      //hist_->fillHist1D("EventInfo","tilt", static_cast<unsigned long int>(condEv()->tilt));
      for(auto& cm: event()->conddatamap){
        uint8_t uid = (cm.first >> 24) & MASK_BITS_8;
        uint8_t i2cReg  = (cm.first >> 16)  & MASK_BITS_8;
        uint8_t i2cPage = (cm.first >> 12)  & MASK_BITS_4;
        uint8_t roId    = (cm.first >> 8)   & MASK_BITS_4;
        uint8_t feId    = (cm.first)        & MASK_BITS_8;
        std::cout << "uid=" << (int)uid
                  << ":i2cReg="<< (int)i2cReg
                  << ":i2cPage="<< (int)i2cPage
                  << ":roId="<< (int)roId
                  << ":feId="<< (int)feId
                  << ":value=" << (int)cm.second
                  << std::endl;
        //if(uid == 1)
        //  std::cout << "Value for uid=1>>>" << (int)cm.second << std::endl;
      }

    }
    hist_->fillHist1D("EventInfo","condData", static_cast<unsigned int>(event()->condData));
    hist_->fillHist1D("EventInfo","tdcPhase", static_cast<unsigned int>(event()->tdcPhase));
    //set the hits/cluster/stubs from the sensors in user accessable module
    setDetChannelVectors();
    //fill common histograms for dut hits, clusters
    fillCommonHistograms();
    //Fill track match histograms
    float resultBothPlanesConstraintShift[] = {-18.8072, -114101., 2400.02, -0.980538, 18.5594};
    //Select events with one track
    if ( event()->tracks.size() !=1 ) continue;
    auto& tk = event()->tracks[0];
    //Select tracks with chi2 > 5
    if (tk.chi2()>5.) continue;
    den++;
    auto& m = modVec()->at(0);

    std::string dnamebottom = m.hdirbottom_ + "/TrackMatch";
    std::string dnametop    = m.hdirtop_ + "/TrackMatch";
    //extrapolate the track
    //xTkAtDUT is a pair of the extrapolated position of the tk in the two planes of the module
    //xTkAtDUT.first will give the position of the track on  bottom sensor
    //xTkAtDUT.second will give the position of the track on top sensor

    std::pair<double, double> xTkAtDUT = Utility::extrapolateTrackAtDUTwithAngles(tk, m.z, offsetbottom(), zDUTbottom(), sensordeltaZ(), dutangle(), shiftPlanes());

    hist_->fillHist1D(dnamebottom, "tkposx", xTkAtDUT.first);
    hist_->fillHist1D(dnametop,    "tkposx",    xTkAtDUT.second);
    
    bool mCls_bottom = false;
    bool mCls_top = false;
    bool mOfflinestub = false;   
    //Fill cluster residuals
    for(auto& c: m.bottomOfflineCls) {
      float cx = (c.center() - m.nstrips_/2.)*m.pitch_;
      hist_->fillHist1D(dnamebottom, "clusresidualX", cx - xTkAtDUT.first);//xTkAtDUT.first since bottom sensor
      if(std::abs(cx - xTkAtDUT.first) < 200.)    mCls_bottom = true;
    }

    for(auto& c: m.topOfflineCls) {
      float cx = (c.center() - m.nstrips_/2.)*m.pitch_;
      hist_->fillHist1D(dnamebottom, "clusresidualX", cx - xTkAtDUT.second);//xTkAtDUT.second since top sensor
      if(std::abs(cx - xTkAtDUT.first) < 200.)    mCls_top = true;
    }
    //stub residual
    for(auto& s: m.offlineStubs) {
      float sx = (s.positionX() - m.nstrips_/2.)*m.pitch_;
      hist_->fillHist1D(dnamebottom, "stubresidualX", sx - xTkAtDUT.first);//xTkAtDUT.first since bottom sensor
      if(std::abs(sx - xTkAtDUT.first) < 200.)    mOfflinestub = true;
    }
    
    //Increment counters if matching is found
    if(mCls_bottom)   nClsb++;
    if(mCls_top)      nClst++;
    if(mOfflinestub)  numstub++;
  }//event Loop end
  std::cout << "Den=" << den << "\t#ClustersBottom=" << nClsb << "\t#ClustersTop=" << nClst << "\tStub=" << numstub << std::endl;

}

void BaselineAnalysis::clearEvent() {
  BeamAnaBase::clearEvent();
}

void BaselineAnalysis::endJob() {
  BeamAnaBase::endJob();
  hist_->closeFile();
}

BaselineAnalysis::~BaselineAnalysis(){
  delete hist_;
}
