/*!
        \file                Histogrammer.cc
        \brief               Class provides user to book, fill histograms
        \author              Suvankar Roy Chowdhury
        \date                05/07/16
        Support :            mail to : suvankar.roy.chowdhury@cern.ch
*/
#include "Histogrammer.h"
#include <climits>
#include <cmath>
#include <cassert>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "TLorentzVector.h"
#include "TFile.h"

using std::cout;
using std::cerr;
using std::endl;
using std::ios;
using std::setw;

Histogrammer::Histogrammer(std::string& outFile) {
  fout_ = new TFile(TString(outFile),"RECREATE");
  isFileopen_ = true;
}

void Histogrammer::bookEventHistograms() {
  fout_->cd();
  fout_->mkdir("EventInfo");
  fout_->cd("EventInfo");
  new TH1I("nevents", "#Events", 10000001, -0.5, 10000000.5);
  new TH1I("dutAngle", "DUT Angle;DUTAngle;#Events", 1805, -90.25, 90.25);  
  new TH1I("alignAngle", "Alignment Angle;DUTAngle;#Events", 18050, -90.25, 90.25);  
  new TH1I("hvSettings", "High Voltage settings;HV;#Events", 1000,-0.5,999.5);
  new TH1I("vcth", "Vcth value;vcth;#Events", 200,-0.5,199.5);
  new TH1I("offset", ";offset;#Events", 200,-0.5,199.5);
  new TH1I("window", ";window;#Events", 200,-0.5,199.5);
  new TH1I("tilt", ";tilt;#Events", 200,-0.5,199.5);
  new TH1I("condData", ";condData;#Events", 20,-0.5,19.5);
  new TH1I("tdcPhase",";tdc;#Events",17,-0.5,16.5);
  new TH1I("isPeriodic",";isPeriodic;#Events",2,-0.5,1.5);
  new TH1I("isGoodFlag",";isGood;#Events",2,-0.5,1.5);
}

void Histogrammer::bookDUTHistograms(std::string det) {
  TString d(det);
  fout_->cd();
  fout_->mkdir(d);
  fout_->cd(d);
  new TH2I("hitmapfull",d + " hitmap;strip no.;#Events",1016,-0.5,1015.5,2,-0.5,1.5);
  bookDUTHistoForColumn(d,"C0");
  //bookDUTHistoForColumn(d,"C1");
}
void Histogrammer::bookDUTHistoForColumn(TString& d, TString c) {
  fout_->cd(d);    
  new TH1I("chsize" + c ,"dut0 channel occupancy per event" + c + ";#Channels;#Events",51,-0.5,50.5);
  new TH1I("hitmap" + c,"dut0 hitmap " + c + ";strip no.;#Events",1016,-0.5,1015.5);
  new TH1D("ncluster" + c,"#cluster dut0 " + c + ";#Clusters;#Events",51,-0.5,50.5);
  new TH1I("clusterWidth" + c,"dut0 clusterWidth " + c +";#ClusterWidth;#Events",128,-0.5,127.5);
  new TH1D("clusterPos" + c,"dut0 clusterPos " + c + ";Strip Number;#Events",1016,-0.5,1015.5);
  new TProfile("clusterWidthVsPosProf" + c,"dut0 clusterWidthVsPos " + c + ";Strip Number;Cluster Width",1016,-0.5,1015.5);
  new TH2D("clusterWidthVsPos2D" + c ,"dut0 clusterWidthVsPos " + c + ";Strip Number;Cluster Width",1016,-0.5,1015.5, 20,-0.5,19.5);
   new TH2D("nhitvsnclus" + c,"#Clusters vs #Hits;#Hits;#Clusters",50,-0.5,49.5, 50,-0.5,49.5);
   new TH2D("nhitvsHitClusPosDiff" + c,"Cluster-Hit MinPosDiff vs #Hits;#Hits;#Pos Diff",50,-0.5,49.5, 256,-0.5,255.5);
   new TH2D("propertyVsTDC2D" + c, "Hit Property vs TDC " + d + c + ";TDC;",17,-0.5,16.5, 10, 0.5, 10.5);
}

void Histogrammer::bookStubHistograms() {
  fout_->cd();
  fout_->mkdir("StubInfo");
  fout_->cd("StubInfo");

  new TH1I("cbcStubWord","Stub Bit from CBC",16,-0.5,15.5);
  new TH1I("recoStubWord","Stub Bit from offline CBC logic emulation",16,-0.5,15.5);
  new TH1I("nstubsFromCBCSword","Total number of stubs from CBC stub word",20,-.5,19.5);
  new TH1I("nstubsFromRecoSword","Total number of stubs from Reco Stub word",20,-.5,19.5); 
  new TH1I("nstubsFromReco","Total number of stubs from Reconstruction",20,-.5,19.5); 
  new TH1I("stubMatch","Matching between CBC Stub and Reco Stub",4,0.5,4.5);
  new TH1I("nstubsdiffSword","#StubsRecoStubword - #StubsfromStubWord",20,-0.5,19.5);
  new TH1I("nstubsdiff","#StubsReco - #StubsfromStubWord",20,-0.5,19.5);
  bookStubHistoForColumn("C0");
  //bookStubHistoForColumn("C1");
}

void Histogrammer::bookStubHistoForColumn(TString c) {
  fout_->cd("StubInfo");
  new TH1I("nstubReco" + c,"Number of stubs for " + c + " from offline reconstruction;#stubs;Events",20,-0.5,19.5);
}

void Histogrammer::bookCorrelationHistograms() {
  fout_->mkdir("Correlation");
  fout_->cd("Correlation");
  bookCorrelationHistoForColumn("C0");
  //bookCorrelationHistoForColumn("C1");
}
    
void Histogrammer::bookCorrelationHistoForColumn(TString c) {
  new TH1D("cor_hit" + c,"Sensor Hit Correlation " + c,4,0.5,4.5);
  new TH1I("nclusterdiff" + c,"Difference in #clusters between dut0 and dut1() for " + c + ";#cluster_{det0} - #cluster_  {det1_};Events",20,-0.5,19.5);
  //new TH1D("nhitdiff" + c,"#Hits det0 - #Hits det1 " + c,100,-.5,99.5);
}


void Histogrammer::bookTrackMatchHistograms() {
  fout_->mkdir("TrackMatch");
  fout_->cd("TrackMatch");
  new TH1D("nTrackParams","#Tracks Telescope;#tracks;#events",30,-0.5,29.5);
  new TH1D("nTrackParamsNodupl","#Tracks Telescope after duplicate removal;#tracks;#events",30,-0.5,29.5);
  new TH1D("hposxTkDUT0","Xpos of extrapolated track at DUT0 plane after alignment(#fid trk=1)",100,-20.,20.);
  new TH1D("hposxTkDUT1","Xpos of extrapolated track at DUT0 plane after alignment(#fid trk=1)",100,-20.,20.);
  new TH1D("hminposClsDUT0","Xpos of the cluster hit at DUT0 plane with min dist from track(#fid trk=1)",100,-20.,20.);
  new TH1D("hminposClsDUT1","Xpos of the cluster hit at DUT1 plane with min dist from track(#fid trk=1)",100,-20.,20.);
  new TH1D("hminposStub","Xpos of the stub hit with min dist from trk(#fid trk=1)",100,-20.,20.);
  new TH2D("minclsTrkPoscorrD0","Cluster xTrk Pos Correlation;trk;cluster",255,-0.5,254.5, 255,-0.5,254.5);
  new TH2D("minclsTrkPoscorrD1","Cluster xTrk Pos Correlation;trk;cluster",255,-0.5,254.5, 255,-0.5,254.5);
  TH2D* h2d = dynamic_cast<TH2D*>(Utility::getHist2D("minclsTrkPoscorrD0"));
  h2d->SetOption("colz");
  h2d = dynamic_cast<TH2D*>(Utility::getHist2D("minclsTrkPoscorrD1"));
  h2d->SetOption("colz");


  //new TH1D("residualDUT0multitrkfidNodupl","ClusterResidual at DUT0 plane(fiducial)(#trk>1, no duplicate tracks)",400,-20.,20.);
  //new TH1D("residualDUT1multitrkfidNodupl","ClusterResidual at DUT1 plane(fiducial)(#trk>1, no duplicate tracks)",400,-20.,20.);
  new TH1D("minresidualDUT0_1trkfid","ClusterResidual at DUT0 plane(fiducial)(#trk=1)",1000,-10.,10.);
  new TH1D("minresidualDUT1_1trkfid","ClusterResidual at DUT1 plane(fiducial)(#trk=1)",1000,-10.,10.);
  new TH1D("clswidthDUT0_1trkfid","ClusterWidth(cluster matched to track) at DUT0 plane(fiducial)(#trk=1)",50,-0.5,49.5);
  new TH1D("clswidthDUT1_1trkfid","ClusterWidth(cluster matched to track) at DUT1 plane(fiducial)(#trk=1)",50,-0.5,49.5);

  new TH2D("minstubTrkPoscorrD1_all","Closest-Stub xTrk Pos Correlation;trk;stub",255,-0.5,254.5, 255,-0.5,254.5);
  new TH2D("minstubTrkPoscorrD1_matched","Closest-Stub xTrk Pos Correlation(matched);trk;stub",255,-0.5,254.5, 255,-0.5,254.5);
  new TH2D("minstubTrkPoscorrD1_CBC","Closest-Stub xTrk Pos Correlation(matched);trk;stub",255,-0.5,254.5, 255,-0.5,254.5);
  h2d = dynamic_cast<TH2D*>(Utility::getHist2D("minstubTrkPoscorrD1_all"));
  h2d->SetOption("colz");
  h2d = dynamic_cast<TH2D*>(Utility::getHist2D("minstubTrkPoscorrD1_matched"));
  h2d->SetOption("colz");

  new TH1D("sminresidualC0_1trkfid","Stub Residual at DUT1 plane(fiducial)(#trk=1)",1000,-10.,10.);

  new TH1D("stubPos_all","Stub Pos;stub pos (strip)",1016,-0.5,1015.5);
  new TH1D("stubPos_matched","Stub Pos;stub pos (strip)",1016,-0.5,1015.5);

  new TH1D("trackPos_all","Stub Pos;stub pos (strip)",1016,-0.5,1015.5);
  new TH1D("trackPos_matched","Stub Pos;stub pos (strip)",1016,-0.5,1015.5);

  new TH1I("trkcluseff","",9,-0.5,11.5);

  TH1I* h = dynamic_cast<TH1I*>(Utility::getHist1D("trkcluseff"));
  h->GetXaxis()->SetBinLabel(1,"xtkNodupl(=1)");
  h->GetXaxis()->SetBinLabel(2,"xtkFidD0");
  h->GetXaxis()->SetBinLabel(3,"xtkFidD1");
  h->GetXaxis()->SetBinLabel(4,"xtkFidD0 && xtkFidD1");
  h->GetXaxis()->SetBinLabel(5,"xtkClsMatchD0");
  h->GetXaxis()->SetBinLabel(6,"xtkClsMatchD1");
  h->GetXaxis()->SetBinLabel(7,"xtkClsMatchD0_&&_D1");
  h->GetXaxis()->SetBinLabel(8,"no-match_D0&&_D1");
  h->GetXaxis()->SetBinLabel(9,"xtkStubMatchC0");
  h->GetXaxis()->SetBinLabel(10,"xtkCBCStubMatchC0");
  h->GetXaxis()->SetBinLabel(11,"xtkCBCStubWrongC0");
  h->GetXaxis()->SetBinLabel(12,"xtkCBCStub>1");

  new TH1I("trkCBCeff","",7,-0.5,6.5);
  TH1I* hc = dynamic_cast<TH1I*>(Utility::getHist1D("trkCBCeff"));
  hc->GetXaxis()->SetBinLabel(1,"xtkFidChip0");
  hc->GetXaxis()->SetBinLabel(2,"StubMatchChip0");
  hc->GetXaxis()->SetBinLabel(3,"xtkFidChip1");
  hc->GetXaxis()->SetBinLabel(4,"StubMatchChip1");
  hc->GetXaxis()->SetBinLabel(5,"xtkFidChip0 && StubChip1");
  hc->GetXaxis()->SetBinLabel(6,"xtkFidChip1 && StubChip0");
  hc->GetXaxis()->SetBinLabel(7,"CBCStub>1");


  new TH1I("effVtdc_num",";TDC;#Events",17,-0.5,16.5);
  new TH1I("effCBCVtdc_num",";TDC;#Events",17,-0.5,16.5);
  new TH1I("effChip0Vtdc_num",";TDC;#Events",17,-0.5,16.5);
  new TH1I("effChip1Vtdc_num",";TDC;#Events",17,-0.5,16.5);
  new TH1I("effRECOVtdc_num",";TDC;#Events",17,-0.5,16.5);
  new TH1I("effVtdc_den",";TDC;#Events",17,-0.5,16.5);
  new TH1I("effChip0Vtdc_den",";TDC;#Events",17,-0.5,16.5);
  new TH1I("effChip1Vtdc_den",";TDC;#Events",17,-0.5,16.5);

  new TH1F("deltaXPos_trkfei4", "Difference in matched Track impact and Hit X Position", 40000, -20.0, 20.0);
  new TH1F("deltaYPos_trkfei4", "Difference in matched Track Impact and Hit Y Position", 40000, -20.0, 20.0);


  new TH1I("trkFidStability","",300,0,300);
  new TH1I("clsD0Stability","",300,0,300);
  new TH1I("clsD1Stability","",300,0,300);
  new TH1I("clsBothStability","",300,0,300);
  new TH1I("stubStability","",300,0,300);
  new TH1I("cbcStability","",300,0,300);

  fout_->mkdir("TrackMatch/CBCcheck");
  fout_->cd("TrackMatch/CBCcheck");
  new TH1I("EventN","EventN;Event Number",300000,0,300000);
  new TH1D("NClusterDUT0","NClusterDUT0;#Clusters;#Events",51,-0.5,50.5);
  new TH1D("NClusterDUT1","NClusterDUT1;#Clusters;#Events",51,-0.5,50.5);
  new TH1D("ClusterPosDUT0","ClusterPosDUT0;Strip Number;#Events",1016,-0.5,1015.5);
  new TH1D("ClusterPosDUT1","ClusterPosDUT1;Strip Number;#Events",1016,-0.5,1015.5);
  new TH1D("RECOStubPosDUT1","RECOStubPos;Strip Number;#Events",1016,-0.5,1015.5);
  new TH1D("RECOStubResDUT1","RECOStubRes;Strip Number;#Events",5000,-8,8);
  new TH1D("ClusterDiff","ClusterDiff;Strip Number;#Events",20,0,20);
  new TH2D("ClusDifvsRes","ClusDifvsRes;Strip Number;Res. (mm)",40,-20,20,500,-4,4);

  fout_->mkdir("TrackMatch/RECOcheck");
  fout_->cd("TrackMatch/RECOcheck");
  new TH1I("EventN","EventN;Event Number",300000,0,300000);
  new TH1D("NClusterDUT0","NClusterDUT0;#Clusters;#Events",51,-0.5,50.5);
  new TH1D("NClusterDUT1","NClusterDUT1;#Clusters;#Events",51,-0.5,50.5);
  new TH1D("ClusterPosDUT0","ClusterPosDUT0;Strip Number;#Events",1016,-0.5,1015.5);
  new TH1D("ClusterPosDUT1","ClusterPosDUT1;Strip Number;#Events",1016,-0.5,1015.5);
  new TH1D("RECOStubPosDUT1","RECOStubPos;Strip Number;#Events",1016,-0.5,1015.5);
  new TH1D("RECOStubResDUT1","RECOStubRes;Strip Number;#Events",5000,-8,8);
  new TH1D("ClusterDiff","ClusterDiff;Strip Number;#Events",20,0,20);
  new TH2D("ClusDifvsRes","ClusDifvsRes;Strip Number;Res. (mm)",40,-20,20,500,-4,4);

  fout_->mkdir("TrackMatch/Bothcheck");
  fout_->cd("TrackMatch/Bothcheck");
  new TH1I("EventN","EventN;Event Number",300000,0,300000);
  new TH1D("NClusterDUT0","NClusterDUT0;#Clusters;#Events",51,-0.5,50.5);
  new TH1D("NClusterDUT1","NClusterDUT1;#Clusters;#Events",51,-0.5,50.5);
  new TH1D("ClusterPosDUT0","ClusterPosDUT0;Strip Number;#Events",1016,-0.5,1015.5);
  new TH1D("ClusterPosDUT1","ClusterPosDUT1;Strip Number;#Events",1016,-0.5,1015.5);
  new TH1D("RECOStubPosDUT1","RECOStubPos;Strip Number;#Events",1016,-0.5,1015.5);
  new TH1D("RECOStubResDUT1","RECOStubRes;Strip Number;#Events",5000,-8,8);
  new TH1D("ClusterDiff","ClusterDiff;Strip Number;#Events",20,0,20);
  new TH2D("ClusDifvsRes","ClusDifvsRes;Strip Number;Res. (mm)",40,-20,20,500,-4,4);

  fout_->mkdir("TrackMatch/CBCOnlycheck");
  fout_->cd("TrackMatch/CBCOnlycheck");
  new TH1I("EventN","EventN;Event Number",300000,0,300000);
  new TH1D("NClusterDUT0","NClusterDUT0;#Clusters;#Events",51,-0.5,50.5);
  new TH1D("NClusterDUT1","NClusterDUT1;#Clusters;#Events",51,-0.5,50.5);
  new TH1D("ClusterPosDUT0","ClusterPosDUT0;Strip Number;#Events",1016,-0.5,1015.5);
  new TH1D("ClusterPosDUT1","ClusterPosDUT1;Strip Number;#Events",1016,-0.5,1015.5);
  new TH1D("RECOStubPosDUT1","RECOStubPos;Strip Number;#Events",1016,-0.5,1015.5);
  new TH1D("RECOStubResDUT1","RECOStubRes;Strip Number;#Events",5000,-8,8);
  new TH1D("ClusterDiff","ClusterDiff;Strip Number;#Events",20,0,20);
  new TH2D("ClusDifvsRes","ClusDifvsRes;Strip Number;Res. (mm)",40,-20,20,500,-4,4);

  new TProfile("nfidtrk_1k","#Events with 1 Fiducial Tracks;Event Number (#times 1000);Entries for 1000 events",10000,0.5,10000.5);
  new TProfile("nmatchedStub_1k","#Events with Stub Matched;Event Number (#times 1000);Entries for 1000 events",10000,-0.5,10000.5);


}

void Histogrammer::bookTelescopeAnalysisHistograms() {
  fout_->cd();
  fout_->mkdir("TelescopeAnalysis");
  fout_->cd("TelescopeAnalysis");
  new TH1D("nTrack","#Tracks Telescope;#tracks;#events",30,-0.5,29.5);
  new TH1D("nTrack_noduplicate","#Tracks Telescope(after duplicate removal);#tracks;#events",30,-0.5,29.5);
  new TH1D("nTrack_fiducial","#Tracks Telescope(fiducial);#tracks;#events",30,-0.5,29.5);
  new TH1I("nhitsFei4","#Hits in FeI4;#hitsFEI4;#Events",30,-0.5,29.5);

  new TH1F("TkXPos", "Track Impact XPos", 96, -12.0, 12.0);
  new TH1F("TkYPos", "Trach Impact YPos", 240, -12.0, 12.0);
  new TH1F("HtColumn", "Hit Column", 85, -0.5, 84.5);
  new TH1F("HtRow", "Hit Row", 340, -0.5, 339.5);
  new TH1F("HtXPos", "Hit XPos", 96, -12.0, 12.0);
  new TH1F("HtYPos", "Hit YPos", 240, -12.0, 12.0);
  new TH1F("deltaXPos", "Difference in Track impact and Hit X Position", 40000, -20.0, 20.0);
  new TH1F("deltaYPos", "Difference in Track Impact and Hit Y Position", 40000, -20.0, 20.0);
  new TH1F("deltaXPos_fit", "Difference in Track impact and Hit X Position", 40000, -20.0, 20.0);
  new TH1F("deltaYPos_fit", "Difference in Track Impact and Hit Y Position", 40000, -20.0, 20.0);

  new TH2F("tkXPosVsHtXPos", "tkXPosVsHtXPos;Xpos of FeI4-Hit(mm);Xpos of Track Impact(mm)", 96, -12.0, 12.0, 96, -12.0, 12.0);
  new TH2F("tkYPosVsHtYPos", "tkYPosVsHtYPos;Ypos of FeI4-Hit(mm);Ypos of Track Impact(mm)", 240, -12.0, 12.0, 240, -12.0, 12.0);
  TH2F* h = dynamic_cast<TH2F*>(Utility::getHist2D("tkXPosVsHtXPos"));
  h->SetOption("colz");
  h = dynamic_cast<TH2F*>(Utility::getHist2D("tkYPosVsHtYPos"));
  h->SetOption("colz");
  new TH1F("deltaXPos_trkfei4", "Difference in Track impact and Hit X Position after alignment", 40000, -20.0, 20.0);
  new TH1F("deltaYPos_trkfei4", "Difference in Track Impact and Hit Y Position after alignment", 40000, -20.0, 20.0);
  new TH1F("deltaXPos_trkfei4M", "Difference in matched Track impact and Hit X Position", 40000, -20.0, 20.0);
  new TH1F("deltaYPos_trkfei4M", "Difference in matched Track Impact and Hit Y Position", 40000, -20.0, 20.0);



}

void Histogrammer::bookTrackFitHistograms(float zMin, float zStep, int zNsteps){
  fout_->cd();
  fout_->mkdir("TrackFit");
  fout_->cd("TrackFit");

  new TH1I("d0_1tk1Hit_diffX","X_{TkAtDUT}-X_{DUT}, d0",10000, -10, 10);
  new TH1I("d1_1tk1Hit_diffX","X_{TkAtDUT}-X_{DUT}, d1",10000, -10, 10);

  new TH1I("d0_1tk1Hit_diffX_bis","X_{TkAtDUT}-X_{DUT}, d0",10000, -10, 10);
  new TH1I("d1_1tk1Hit_diffX_bis","X_{TkAtDUT}-X_{DUT}, d1",10000, -10, 10);

  new TH1I("d0_1tk1Hit_diffX_ter","X_{TkAtDUT}-X_{DUT}, d0",10000, -10, 10);
  new TH1I("d1_1tk1Hit_diffX_ter","X_{TkAtDUT}-X_{DUT}, d1",10000, -10, 10);

  for (int iz=0; iz<zNsteps; iz++){
    new TH1I(Form("d0_1tk1Hit_diffX_iz%i", iz),"X_{TkAtDUT}-X_{DUT}, d0",10000, -10, 10);
    new TH1I(Form("d1_1tk1Hit_diffX_iz%i", iz),"X_{TkAtDUT}-X_{DUT}, d0",10000, -10, 10);
  }

  float zMax = zMin + ((float)zNsteps) * zStep;
  float shift = zStep/2.;

  new TH1F("d0_offsetVsZ", "x_{DUT} offset vs injected z_{DUT}, d0", zNsteps, zMin-shift, zMax-shift);
  new TH1F("d1_offsetVsZ", "x_{DUT} offset vs injected z_{DUT},d1", zNsteps, zMin-shift, zMax-shift);

  new TH1F("d0_chi2VsZ","chi2 vs injected z_{DUT}, d0", zNsteps, zMin-shift, zMax-shift);
  new TH1F("d1_chi2VsZ","chi2 vs injected z_{DUT}, d1", zNsteps, zMin-shift, zMax-shift);

  new TH1I("d0_1tk1Hit_diffX_aligned","X_{TkAtDUT}-X_{DUT}, d0",10000, -10, 10);
  new TH1I("d1_1tk1Hit_diffX_aligned","X_{TkAtDUT}-X_{DUT}, d1",10000, -10, 10);
  new TH1I("d0_1tk1ClusterBothPlanes_diffX_aligned","X_{TkAtDUT}-X_{Cls,DUT}, d0",10000, -10, 10);
  new TH1I("d1_1tk1ClusterBothPlanes_diffX_aligned","X_{TkAtDUT}-X_{Cls,DUT}, d1",10000, -10, 10);
  new TH1I("d0_1tk1ClusterBothPlanesConstraint_diffX_aligned","X_{TkAtDUT}-X_{Cls,DUT}, d0",10000, -10, 10);
  new TH1I("d1_1tk1ClusterBothPlanesConstraint_diffX_aligned","X_{TkAtDUT}-X_{Cls,DUT}, d1",10000, -10, 10);

  new TH1F("bothPlanes_chi2VsTheta","chi2 vs injected #theta", 41, -20.-0.5, 21.-0.5);
  new TH1F("bothPlanesConstraint_chi2VsTheta","chi2 vs injected #theta", 41, -20.-0.5, 21.-0.5);
  new TH1F("bothPlanesConstraint_chi2VsDeltaZ","chi2 vs injected #deltaZ", 41, 0.-0.125, 10.25-0.125);

}

void Histogrammer::bookFeI4Histograms()
{
  fout_->mkdir("FeI4");
  fout_->cd("FeI4");
  new TH1D("x0_Residual_Evnt",";Event number; Number of Mismatched Clusters",(300e3/5) , -0.5, 300e3 -0.5);
  new TH1D("lv1","Number of level1 accept;Level1 Accept; [a.u.]; Count [a.u]",(100/1.0),-0.5,99.5);
  new TH1D("lv1_M","Number of level1 accept;Level1 Accept; [a.u.]; Count [a.u]",(100/1.0),-0.5,99.5);
  
  new TH1I("nHits","Number of FeI4 hits per event;Pixels in Cluster; [Hits]; Count [a.u]",100,-0.5,99.5);
  new TH1I("nClusters","Number of FeI4 custers per event; [Clusters];Count [a.u]", 100 ,-0.5,99.5);
  new TH1I("hTotFei4","Total ToT FeI4 Cluster; [Clusters];Count [a.u]", 100 ,-0.5,99.5);
  new TH1D("hQFeI4","Charge Deposited in FeI4 per Event; Q [kElectrons]; Number of Events",(1000/1.5) , -0.5,  1000-0.5);
  new TH1I("nPixels","Number of Pixels per cluster in Fei4 plane;Pixels in Cluster; Count[a.u]",100,-0.5,99.5);
  
  new TH1I("nTracks","Number of Tracks per event; [Tracks]; Count [a.u]",100,-0.5,99.5);
  new TH1I("nTracks_event","Number of Tracks within acceptance window per event; [Tracks]; Count [a.u]",100,-0.5,99.5);
  new TH1I("nTracks_cluster","Number of Tracks within acceptance window per cluster in Fei4 plane; [Tracks]; Count [a.u]",100,-0.5,99.5);
  
  new TH1I("nTracks_1Cluster","Number of Tracks for events with 1 cluster in Fei4 plane; [Tracks]; Count [a.u]",100,-0.5,99.5);
  new TH1I("nTracks_2Cluster","Number of Tracks for events with 2 clusters in Fei4 plane; [Tracks]; Count [a.u]",100,-0.5,99.5);
  new TH1I("nTracks_3Cluster","Number of Tracks for events with 3 clusters in Fei4 plane; [Tracks]; Count [a.u]",100,-0.5,99.5);
  new TH1I("nTracks_4Cluster","Number of Tracks for events with 4 clusters in Fei4 plane; [Tracks]; Count [a.u]",100,-0.5,99.5);
  new TH1I("nTracks_5Cluster","Number of Tracks for events with 5 clusters in Fei4 plane; [Tracks]; Count [a.u]",100,-0.5,99.5);
  new TH1I("nTracks_6Cluster","Number of Tracks for events with 6 clusters in Fei4 plane; [Tracks]; Count [a.u]",100,-0.5,99.5);
  
  new TH1D("tk_dYdZ","dydz of tracks matched to FeI4 clusters;dydz; [a.u.]; Count [a.u]",(20e-4/1e-5),-10e-4,10e-4);
  new TH1D("tk_dXdZ","dxdz of tracks matched to FeI4 clusters;dxdz; [a.u.]; Count [a.u]",(20e-4/1e-5),-10e-4,10e-4);
  new TH2D("x0_Correlation","; x_{FeI4,Cluster} [mm]; x_{DUT0,Cluster}",36/(50e-3/1), -18.,18. , (36/(0.09/1)), -18.0, 18.0);
  new TH2D("x1_Correlation","; x_{FeI4,Cluster} [mm]; x_{DUT1,Cluster}",36/(50e-3/1), -18.,18. , (36/(0.09/1)), -18.0, 18.0);
  new TH2D("nClusters0_Correlation","; n_{FeI4,Cluster} [mm]; n_{DUT0,Cluster}",100,-0.5,99.5 , 100,-0.5,99.5);
  new TH2D("nClusters1_Correlation","; n_{FeI4,Cluster} [mm]; n_{DUT1,Cluster}",100,-0.5,99.5 , 100,-0.5,99.5);
  new TH2F("HitMap","FEI4 Hit Map; x_{FeI4} [mm]; y_{FeI4} [mm]" , (int)(40.0/250e-3) , -20.0, 20.0,  (int)(40.0/50e-3) , -20.0, 20.0);
  
  new TH2F("HitMap_Raw","FEI4 Hit Map; x_{FeI4} [mm]; y_{FeI4} [mm]" , (int)(40.0/250e-3) , -20.0, 20.0,  (int)(40.0/50e-3) , -20.0, 20.0);
  
  
  double Min_FeI4 = -10.0;
  double Max_FeI4 =  10.0; 
  double Range_FeI4 = (Max_FeI4-Min_FeI4);
  int nResX_binSize = (int)(Range_FeI4/(25e-3));
  int nResY_binSize = (int)(Range_FeI4/(25e-3));
  new TH2D("Residuals", "Difference in Track Impact and FeI4 Cluster; x_{FeI4} - x_{Track} [mm]; y_{FeI4} - y_{Track} [mm]", nResX_binSize, Min_FeI4 , Max_FeI4,  nResY_binSize,  Min_FeI4 , Max_FeI4);
  
  new TH1D("x0_Residual","; x_{FeI4,Cluster} - x_{DUT0,Cluster} [mm]; Counts [a.u]", 36/(50e-3/5), -18.0, 18.0);  
  new TH1D("x1_Residual","; x_{FeI4,Cluster} - x_{DUT1,Cluster} [mm]; Counts [a.u]", 36/(50e-3/5), -18.0, 18.0);  
} 

void Histogrammer::bookCBCHistograms(std::string cbc)
{
  TString d(cbc);
  fout_->cd();
  fout_->mkdir(d);
  fout_->cd(d);
  double xMin = -1.0*0.09*150; 
  double xMax = 1.0*0.09*150;
  double xBin = 0.09/1.0;  
  double xOffset = 0.0;//xBin/2.0;
  new TH1D("fidTracks_num", ";# Fiducial Tracks Per Event; Number of Events" , 100 , 0.0-0.5, 100.0-0.5 );
  new TH1D("fidTracks_impact_mm", ";x_{Track} [mm]; Number of Events" , (xMax-xMin)/xBin, xMin , xMax );
  new TH1D("fidTracks_impact_strips", ";x_{Track} [mm]; Number of Events" , 255,-0.5,254.5);
  new TH1D("matchedCluster_mm", ";x_{Cluster} [mm]; Number of Events" , (xMax-xMin)/xBin, xMin , xMax );
  
  new TH1D("fidTracks_impact_mm_CentralRegion", ";x_{Track} [mm]; Number of Events" , (xMax-xMin)/(0.1*xBin) , xMin , xMax );
  new TH1D("matchedCluster_mm_CentralRegion", ";x_{Track} [mm]; Number of Events" , (xMax-xMin)/(0.1*xBin) , xMin , xMax );
  new TH2D("fidTracks_impact_mm_CentralRegion_2D", ";x_{Track} [mm]; Cluster Width [Strips]" , (xMax-xMin)/(0.1*xBin) , xMin , xMax , 5 , -0.5 , 5-0.5) ;
  new TH2D("matchedCluster_mm_CentralRegion_2D", ";x_{Track} [mm]; Cluster Width [Strips]" , (xMax-xMin)/(0.1*xBin) , xMin , xMax ,5  , -0.5 , 5-0.5 );
  
  new TH1D("matchedCluster_strips", ";x_{Cluster} [mm]; Number of Events" , 255,-0.5,254.5);
  new TH2D("matchedCluster_CW", ";x_{Cluster} [mm]; Cluster Width [Strips]" ,  255,-0.5,254.5 , 10 , -0.5 , 9.5);
  new TH2D("matchedCluster_CW_mm", ";x_{Track} [mm]; Cluster Width [Strips]" , (xMax-xMin)/xBin, xMin , xMax, 10 , -0.5 , 9.5);
  new TProfile("matchedClusterWidth_mm", ";x_{Track} [mm]; Average Cluster Width [Strips]" , (xMax-xMin)/xBin, xMin , xMax );
  
  new TH1D("matchedStub_mm", ";x_{Stub} [mm]; Number of Events" , (xMax-xMin)/xBin, xMin , xMax );
  new TH2D("matchedStub_mm_2D", ";x_{Track} [mm]; TDC Phase" , (xMax-xMin)/xBin, xMin , xMax , 20 , -0.5 , 19.5);
  new TH1D("candidateStubs_impact_mm", ";x_{Track} [mm]; Number of Events" , (xMax-xMin)/xBin, xMin , xMax );
  new TH2D("candidateStubs_impact_mm_2D", ";x_{Track} [mm]; TDC Phase" , (xMax-xMin)/xBin, xMin , xMax , 20 , -0.5 , 19.5);
  //new TH2D("candidateStubs_impact_mm_CW", ";x_{Track} [mm]; TDC Phase" , (xMax-xMin)/xBin, xMin , xMax , 20 , -0.5 , 19.5);
  
  new TH1D("nCandidateClusters", ";TDC Phase; Number of Events", 17 , -0.5 , 16.5);
  new TH1D("nClusters_ANA", ";TDC Phase; Number of Clusters", 17 , -0.5 , 16.5);
  new TH1D("nCandidateStubs", ";TDC Phase; Number of Events", 17 , -0.5 , 16.5);
  new TH1D("nStubs_CHIP", ";TDC Phase; Number of Events", 17 , -0.5 , 16.5);
  new TH1D("nStubs_RECO", ";TDC Phase; Number of Events", 17 , -0.5 , 16.5);
  new TH1D("nStubs_ANA", ";TDC Phase; Number of Events", 17 , -0.5 , 16.5);
  new TH1D("stubMatching", "; ; Number of Events" , 5 , -0.5 , 5.0 -0.5);
  new TH2D("stubMatching_2D", "; ; Number of Events" , 5 , -0.5 , 5.0 -0.5 ,(xMax-xMin)/xBin, xMin , xMax );
  new TH2D("stubMatching_CW", "; ; Number of Events" , 5 , -0.5 , 5.0 -0.5 , 20 , -0.5 , 19.5);
  new TH2D("stubMatching_TDC", "; ; Number of Events" , 5 , -0.5 , 5.0 -0.5 , 20 , -0.5 , 19.5);
}

TH1* Histogrammer::GetHistoByName(const char* dir, const char* hname){
  fout_->cd(dir);
  return Utility::getHist1D(hname);
}

TH1* Histogrammer::GetHistoByName(const std::string& dir, const std::string& hname) {
  fout_->cd(dir.c_str());
  return Utility::getHist1D(hname);

}

TH2* Histogrammer::Get2DHistoByName(const char* dir, const char* hname){
  fout_->cd(dir);
  return Utility::getHist2D(hname);
}

TH2* Histogrammer::Get2DHistoByName(const std::string& dir, const std::string& hname) {
  fout_->cd(dir.c_str());
  return Utility::getHist2D(hname);
}

TProfile* Histogrammer::GetProfileByName(const std::string& dir, const std::string& hname) {
  fout_->cd(dir.c_str());
  return Utility::getHistProfile(hname);
}
TProfile* Histogrammer::GetProfileByName(const char* dir, const char* hname){
  fout_->cd(dir);
  return Utility::getHistProfile(hname);
}

void Histogrammer::FillAlignmentOffsetVsZ(const char* det, const char* histo, int iz, float z, float x, float x_err){

  fout_->cd("TrackFit");
  char histname[50];
  strcpy( histname, det );
  strcat( histname, histo );
  TH1* h = (TH1*) gDirectory->Get(histname);
  h->Fill(z, x);
  h->SetBinError(iz+1, x_err);
}

void Histogrammer::closeFile() { 
  fout_->cd();
  fout_->Write();
  fout_->Close();
  isFileopen_=false;  
}
void Histogrammer::fillClusterHistograms( const char* det, std::vector<tbeam::cluster>& cvec, 
                                          const char* col) {
  fout_->cd(det);
  TString c(col);
  Utility::fillHist1D( "ncluster" + c, cvec.size() );
  for( unsigned int i =0; i<cvec.size(); i++ ) {
    Utility::fillHist1D("clusterWidth" + c,cvec[i].size);
    Utility::fillHist1D("clusterPos" + c,cvec[i].x);
    Utility::fillHistProfile("clusterWidthVsPosProf" + c,cvec[i].x,cvec[i].size);
    Utility::fillHist2D("clusterWidthVsPos2D" + c,cvec[i].x,cvec[i].size);
  } 
}

Histogrammer::~Histogrammer() {
  if(isFileopen_)  {
    std::cout << "You forgot to close the output file!!!Closing it now" << std::endl;  
    closeFile();
   }
  delete fout_; 
}
