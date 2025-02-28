// -----------------------------------------
// PrtLutReco.cpp
//
// Created on: 13.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------

#include "PrtLutReco.h"

using std::cout;
using std::endl;

TGraph fgg_gr;
void circleFcn(int &, double *, double &f, double *par, int) {
  int np = fgg_gr.GetN();
  f = 0;
  double *x = fgg_gr.GetX();
  double *y = fgg_gr.GetY();
  for (int i = 0; i < np; i++) {
    double u = x[i] + par[0];
    double v = y[i] + par[1];
    double dr = par[2] - TMath::Sqrt(u * u + v * v);
    f += dr * dr;
  }
}

void circleFcn2(int &, double *, double &f, double *par, int) {
  int np = fgg_gr.GetN();
  f = 0;
  double *x = fgg_gr.GetX();
  double *y = fgg_gr.GetY();
  for (int i = 0; i < np; i++) {
    double u = x[i] + par[0];
    double v = y[i] + par[1];
    double dr = par[2] - TMath::Sqrt(u * u + v * v);
    if (dr > 0.07)
      f += dr * dr;
    else
      f += fabs(dr);
  }
}

TH1F *fHist1 = new TH1F("time1", ";measured time [ns];entries [#]", 1000, 0, 50);
TH1F *fHist2 = new TH1F("time2", ";calculated time [ns];entries [#]", 1000, 0, 50);
TH1F *fHist6 = new TH1F("time6", ";measured time [ns];entries [#]", 1000, 0, 50);

TH1F *fCtr = new TH1F("ctr", ";#Delta#theta_{C} [mrad];entries [#]", 150, -15, 15);

TH1F *hBounce = new TH1F("bounce", ";number of bounces [#];photons per event [#]", 250, 0, 250);

TH2F *fDiff =
  new TH2F("diff", ";measured time [ns];t_{measured}-t_{calculated} [ns]", 300, 0, 30, 150, -5, 5);
TH2F *fHist3 =
  new TH2F("time3", ";calculated time [ns];measured time [ns]", 500, 0, 80, 500, 0, 40);
TH2F *fHist4 = new TH2F("time4", ";#theta_{c}sin(#varphi_{c});#theta_{c}cos(#varphi_{c})", 400, -1,
                        1, 400, -1, 1);
TH2F *fHist5 = new TH2F("time5", ";#theta_{c}sin(#varphi_{c});#theta_{c}cos(#varphi_{c})", 400, -1,
                        1, 400, -1, 1);

TH2F *hLnMap = new TH2F("hLnMap", ";GR     ln L(p) - ln L(#pi);TI     ln L(p) - ln L(#pi); ", 120,
                        -60, 60, 120, -60, 60);

TH2F *hChrom = new TH2F("chrom", ";t_{measured}-t_{calculated} [ns];#Delta#theta_{C} [mrad]", 100,
                        -1.0, 1.0, 100, -30, 30);
TH2F *hChromL =
  new TH2F("chroml", ";(t_{measured}-t_{calculated})/t_{measured};#Delta#theta_{C} [mrad]", 100,
           -0.1, 0.1, 100, -30, 30);

const int nphi = 80, ntheta = 40;
TH2F *hLutCorrD = new TH2F("hLutCorrD", ";#theta_{l}sin(#varphi_{l});#theta_{l}cos(#varphi_{l})",
                           200, -1, 1, 200, -1, 1);
TH2F *hLutCorrC = new TH2F("hLutCorrC", ";#theta_{l}sin(#varphi_{l});#theta_{l}cos(#varphi_{l})",
                           100, -1, 1, 100, -1, 1);


// -----   Default constructor   -------------------------------------------
PrtLutReco::PrtLutReco(TString infile, TString lutfile, TString pdffile, int verbose) {
  fVerbose = verbose;
  fPdfPath = pdffile;
  fChain = new TChain("data");
  fChain->Add(infile);
  fEvent = new PrtEvent();
  frun = PrtManager::Instance()->getRun();
  
  ft = PrtTools(frun);  
  fmaxch = ft.maxdircch();
  fnpmt = frun->getNpmt();
  fnpix = frun->getNpix();
  fMethod = frun->getRunType();
  fStudyId = frun->getStudy();
  fRadiator = frun->getRadiator();
  int fileId = frun->getId();
  fPk = 0;
  if (frun->getPid() == 10002) fPk = 3; //  kaon

  fChain->SetBranchAddress("PrtEvent", &fEvent);

  TString sn = "cherenkov angle;#theta_{C} [rad];entries [#]";
  fHist = new TH1F("cherenkov_angle_hist", sn, 150, 0.6, 1);      // 150
  fHistPi = new TH1F("cherenkov_angle_hist_Pi", sn, 150, 0.6, 1); // 150
  fHisti = new TH1F("cherenkov_angle_histi", sn, 150, 0.6, 1);    // 150
  fFit = new TF1("fgaus", "[0]*exp(-0.5*((x-[1])/[2])*(x-[1])/[2]) +x*x*[3]+x*[4]+[5]", 0.35, 0.9);
  fFit->SetNpx(300);

  TString spath = "data/sim";
  if (infile.Contains("beam_")) {
    TString opath(infile);
    opath.Remove(opath.Last('/'));
    if (infile.Contains("C.root")) {
      spath = opath + Form("/%dr/%d", fStudyId, fileId);
    } else {
      spath = opath + Form("/%ds/%d", fStudyId, fileId);
    }
  }
  ft.set_path(spath);

  std::cout << "--- save path  " << spath << std::endl;

  for (int i = 0; i < fnpmt; i++) {
    fHistMcp[i] = new TH1F(Form("fHistMcp_%d", i),
                           Form("fHistMcp_%d;#theta_{C} [rad];entries [#]", i), 80, -0.05, 0.05);
    fHistTime[i] = new TH1F(Form("fHistTime_%d", i),
                            Form("fHistTime_%d;t_{measured}-t_{calculated} [ns]", i), 150, -5, 5);
  }
  for (int i = 0; i < 2; i++) {
    fHistTimeRefl[i] =
      new TH1F(Form("fHistTimeRefl_%d", i),
               Form("fHistTimeRefl_%d;t_{measured}-t_{calculated} [ns]", i), 150, -5, 5);
  }

  for (int i = 0; i < fmaxch; i++) {
    fHistCh[i] = new TH1F(Form("fHistCh_%d", i), Form("fHistCh_%d;#theta_{C} [rad];entries [#]", i),
                          80, -0.05, 0.05); // 150
  }

  // read lut
  if (lutfile == "") lutfile = ft.get_lutpath();
  if (!gSystem->AccessPathName(lutfile)) {
    std::cout << "--- reading  " << lutfile << std::endl;
    fFile = new TFile(lutfile);
    fTree = (TTree *)fFile->Get("prtlut");
    fLut = new TClonesArray("PrtLutNode");
    fTree->SetBranchAddress("LUT", &fLut);
    fTree->GetEntry(0);
    for (int i = 0; i < fmaxch; i++) {
      fLutNode[i] = (PrtLutNode *)fLut->At(i);
    }
  } else {
    std::cout << "--- lut file not found  " << fCorrPath << std::endl;
  }

  fTimeImaging = (fMethod == 4) ? true : false;

  // read pdf
  if (fPdfPath == "") {
    fPdfPath = infile;
    fPdfPath.ReplaceAll(".root", ".pdf1.root");
  }
  if (fMethod == 2) {
    if (!gSystem->AccessPathName(fPdfPath)) {
      std::cout << "--- reading  " << fPdfPath << std::endl;
      TFile pdfFile(fPdfPath);
      double sigma = 400; // PrtManager::Instance()->GetTest1();// 400;//250; // ps
      int binfactor = (int)(sigma / 50. + 0.1);
      for (int i = 0; i < fmaxch; i++) {
        auto hpdf2 = (TH1F *)pdfFile.Get(Form("hs_%d", i));
        auto hpdf4 = (TH1F *)pdfFile.Get(Form("hf_%d", i));
        if (sigma > 0) hpdf2->Rebin(binfactor);
        if (sigma > 0) hpdf4->Rebin(binfactor);
        // hpdf2->Smooth();
        // hpdf4->Smooth();
        fPdf2[i] = new TGraph(hpdf2);
        fPdf4[i] = new TGraph(hpdf4);

        fPdf2[i]->SetBit(TGraph::kIsSortedX);
        fPdf4[i]->SetBit(TGraph::kIsSortedX);

        fTimeImaging = true;
      }
    } else
      fTimeImaging = false;
  }

  // read corrections
  fCorrPath = PrtManager::Instance()->getOutName(); // infile;
  fCorrPath.ReplaceAll(".root", ".cor.root");
  fCorrPath.ReplaceAll("reco_", Form("reco_%d_", fileId));
  if (!gSystem->AccessPathName(fCorrPath)) {
    std::cout << "--- reading  " << fCorrPath << std::endl;
    int pmt, level;
    double cor_angle, cor_time, cor_time_0, cor_time_1;
    TChain ch("corr");
    ch.Add(fCorrPath);
    ch.SetBranchAddress("pmt", &pmt);
    ch.SetBranchAddress("level", &level);
    ch.SetBranchAddress("cor_angle", &cor_angle);
    ch.SetBranchAddress("cor_time", &cor_time);
    ch.SetBranchAddress("cor_time_0", &cor_time_0);
    ch.SetBranchAddress("cor_time_1", &cor_time_1);
    ch.SetBranchAddress("spr_pi", &fCorrSpr);

    for (int i = 0; i < ch.GetEntries(); i++) {
      ch.GetEvent(i);
      if (level == 2)  fCor_angle[pmt] = (fabs(cor_angle) < 0.012) ? cor_angle : cor_angle / fabs(cor_angle) * 0.012;
      fCor_time[pmt] = cor_time;
      fCor_time_refl[0] = cor_time_0 * 0.9;
      fCor_time_refl[1] = cor_time_1 * 0.9;
      std::cout << "L" << level << "   pmt " << pmt << "  " << fCor_angle[pmt] << " dt "
                << fCor_time[pmt] << "  spr " << fCorrSpr << " refl " << fCor_time_refl[0] << " "
                << fCor_time_refl[1] << std::endl;
    }
    fCor_level = level;
  } else {
    fCorrSpr = 0.01;
    fCor_level = 0;
    std::cout << "--- corr file not found  " << fCorrPath << std::endl;
  }

  for (int i = 0; i < fmaxch; i++) {
    fTime2[i] = new TH1F(Form("hs_%d", i), "pdf;LE time [ns]; entries [#]", 1000, 0, 50);
    fTime4[i] = new TH1F(Form("hf_%d", i), "pdf;LE time [ns]; entries [#]", 1000, 0, 50);
  }
  
  int range = 70;
  if (infile.Contains("415_2")) range = 280;
  if (infile.Contains("415_4")) range = 180;
  if (infile.Contains("436_4")) range = 180;

  int nrange = 100;
  if(fnpix>65){
    nrange = 200;
  }

  for (int i = 0; i < 5; i++) {
    hTof[i] = new TH1F("tof_" + ft.name(i), ";TOF [ns];entries [#]", 500, 32, 36);
    hTofc[i] = new TH1F("tofc_" + ft.name(i), ";TOF [ns];entries [#]", 500, 32, 36);
    hNph[i] = new TH1F("nph_" + ft.name(i), ";detected photons [#];entries [#]", nrange, 0, nrange);

    hLnDiffGr[i] = new TH1F("hLnGr_"+ ft.name(i), ";ln L(p) - ln L(#pi);entries [#]", 120, -range, range);
    hLnDiffTi[i] = new TH1F("hLnTi_"+ ft.name(i), ";ln L(p) - ln L(#pi);entries [#]", 120, -range, range);
  }

  TString snt = ";t_{measured}-t_{calculated} [ns];entries [#]";
  fHist0 = new TH1F("timediff", snt, 500, -10, 10);
  fHist0d = new TH1F("timediffd", snt, 500, -10, 10);
  fHist0r = new TH1F("timediffr", snt, 500, -10, 10);
  fHist0s = new TH1F("timediffs", snt, 500, -10, 10);
  fHist0i = new TH1F("timediffi", snt, 500, -10, 10);
  
  std::cout << "--- initialization done " << std::endl;
}

// -----   Destructor   ----------------------------------------------------
PrtLutReco::~PrtLutReco() {}

//-------------- Loop over tracks ------------------------------------------
void PrtLutReco::Run(int start, int end) {
  TVector3 dird, dir, momInBar, momInBar0, posInBar, cz;
  double mom, tangle, likelihood(0), boxPhi, weight, evtime, bartime, lenz, posz, dirz, luttheta,
    barHitTime, hitTime, angdiv, dtheta, dtphi, prtangle;
  int pid(0), distPid(0), likePid(0), pdgcode, evpointcount(0), total2(0), total4(0);
  int events[5] = {0};
  bool reflected = kFALSE;
  gStyle->SetOptFit(1);
  gStyle->SetOptStat(1);

  TVector3 fnX1 = TVector3(1, 0, 0);
  TVector3 fnY1 = TVector3(0, 1, 0);
  bool testTrRes = false;

  TString outFile = PrtManager::Instance()->getOutName();
  double theta(0), phi(0), cangle(0), spr(0), trr(0), nph(0), nph_err(0), cangle_pi(0), spr_pi(0),
    trr_pi(0), nph_pi(0), nph_pi_err(0), par1(0), par2(0), par3(0), par4(0), par5(0), par6(0),
    test1(0), test2(0), test3(0), sep_gr(0), sep_gr_err(0), sep_ti(0), sep_ti_err(0), beamx(0),
    beamz(0), nnratio_p(0), nnratio_pi(0), timeRes(0), ctr_sigma(0), ctr_err(0);
  double minChangle(0);
  double maxChangle(1);
  double criticalAngle = asin(1.00028 / 1.47125);      // n_quarzt = 1.47125; //(1.47125 <==> 390nm)


  double radiatorL = frun->getRadiatorL();
  double radiatorW = frun->getRadiatorW();
  double radiatorH = frun->getRadiatorH();

  // double radiatorL = (fRadiator == 2) ? 1224.9 : 1200; // plate : bar
  // double radiatorW = (fRadiator == 2) ? 174.8 : 34.9;  // plate : bar
  // double radiatorH = (fRadiator == 2) ? 1224.9 : 17.1; // plate : bar
  
  ft.set_palette(1);
  ft.create_maps();  
  ft.init_digi();  
  
  outFile.ReplaceAll("reco_", Form("reco_%d_", frun->getId()));
  TFile file(outFile, "recreate");
  TTree tree("reco", "PrtLutReco");
  tree.Branch("mom", &mom, "mom/D");
  tree.Branch("theta", &theta, "theta/D");
  tree.Branch("phi", &phi, "phi/D");
  tree.Branch("nph", &nph, "nph/D");
  tree.Branch("nph_err", &nph_err, "nph_err/D");
  tree.Branch("sep_gr", &sep_gr, "sep_gr/D");
  tree.Branch("sep_gr_err", &sep_gr_err, "sep_gr_err/D");
  tree.Branch("sep_ti", &sep_ti, "sep_ti/D");
  tree.Branch("sep_ti_err", &sep_ti_err, "sep_ti_err/D");
  tree.Branch("time_res", &timeRes, "timeRes/D");

  tree.Branch("cangle", &cangle, "cangle/D");
  tree.Branch("spr", &spr, "spr/D");
  tree.Branch("trr", &trr, "trr/D");

  tree.Branch("nph_pi", &nph_pi, "nph_pi/D");
  tree.Branch("nph_pi_err", &nph_pi_err, "nph_pi_err/D");
  tree.Branch("cangle_pi", &cangle_pi, "cangle_pi/D");
  tree.Branch("spr_pi", &spr_pi, "spr_pi/D");
  tree.Branch("trr_pi", &trr_pi, "trr_pi/D");

  tree.Branch("ctr_sigma", &ctr_sigma, "ctr_sigma/D");
  tree.Branch("ctr_err", &ctr_err, "ctr_err/D");

  tree.Branch("pid_tof", &pid, "pid/I");
  tree.Branch("pid_dist", &distPid, "distPid/I");
  tree.Branch("pid_lh", &likePid, "likePid/I");
  tree.Branch("likelihood", &likelihood, "likelihood/D");

  tree.Branch("test1", &test1, "test1/D");
  tree.Branch("test2", &test2, "test2/D");
  tree.Branch("test3", &test3, "test3/D");
  tree.Branch("nnratio_p", &nnratio_p, "nnratio_p/D");
  tree.Branch("nnratio_pi", &nnratio_pi, "nnratio_pi/D");
  tree.Branch("beamx", &beamx, "beamx/D");
  tree.Branch("beamz", &beamz, "beamz/D");
  tree.Branch("par5", &par5, "par5/D");
  tree.Branch("par6", &par6, "par6/D");

  test1 = frun->getTest1();
  test2 = frun->getTest2();
  test3 = frun->getTest3();
  par5 = frun->getPrismStepX();
  par6 = frun->getPrismStepY();
  timeRes = frun->getTimeSigma();
  fMethod = frun->getRunType();
  bool bsim = frun->getMc();

  if( fCor_level == 0) timeRes = 1.5;
  
  prtangle = frun->getTheta() + test1 * TMath::RadToDeg();
  phi = frun->getPhi() + test2 * TMath::RadToDeg();

  int nEvents = fChain->GetEntries();
  if (end == 0) end = nEvents;

  int pdfend = 50000;
  if (fMethod == 4) {
    if(bsim) start = 0;
    else start = pdfend;
    pdfend = nEvents;
    end = nEvents;
  }

  std::cout << "--- run started for [" << start << "," << end << "]" << std::endl;
  int nsHits(0), nsEvents(0), ninfit(1);

  if (start < 0) {
    ninfit = abs(start);
    start = 0;
  }  

  double speed = 196.5; // 197  mm/ns
  double sigma[] = {0, 0, 0.0075, 0, 0.0076}, noise(0.25), range(5 * sigma[2]);
  double cwindow = 0.05;

  if(fnpix>65){
    speed = 195.5;
    sigma[2] = fCorrSpr;
    sigma[3] = fCorrSpr;
    sigma[4] = fCorrSpr; 
  }
  
  for (int ievent = start;
       ievent < nEvents && (events[2] < end || events[fPk] < end) && ievent < pdfend; ievent++) {
    int nhhits(0);
    
    fChain->GetEntry(ievent);
    double angle1(0), angle2(0), sum1(0), sum2(0), sumti(0), sumti2(0), sumti4(0);

    if (ievent % 1000 == 0)
      std::cout << "Event # " << ievent << " has " << fEvent->getHits().size() << " hits "
                << events[2] << " " << events[fPk] << std::endl;

    if (bsim) posz = 0.5 * radiatorL - fEvent->getPosition().Z() + gRandom->Uniform(-5, 5);
    else posz = fEvent->getPosition().Z()+40;


    double etime = fEvent->getTime();

    if(20 * etime/(3600.) > test3) continue;
    momInBar = fEvent->getMomentum();
    beamx = fEvent->getPosition().X();
    beamz = fEvent->getPosition().Z();
    if (bsim) beamz = 0.5 * radiatorL - beamz;

    // std::cout<<"momInBar.Theta() "<<momInBar.Theta()*TMath::RadToDeg()<<std::endl;
    // if (fabs(momInBar.Theta()-TMath::Pi()) > 0.1) continue;
    
    momInBar.RotateY(TMath::TwoPi() - prtangle * TMath::DegToRad());

    mom = momInBar.Mag() * 0.001;
    pid = ft.get_pid(fEvent->getPid());
    pid = 1;

    if (mom < 3) continue;
    if (events[pid] >= end) continue;
 
    // if (ievent - start == 0) {
      
    //   momInBar = TVector3(0, 0, 1);
    //   momInBar.RotateY(TMath::Pi() - prtangle * TMath::DegToRad());
    //   momInBar.RotateZ(phi * TMath::DegToRad());
    // }
 
    for (int i = 0; i < 5; i++) {
      fAngle[i] = acos(sqrt(mom * mom + ft.mass(i) * ft.mass(i)) / mom /
                       1.4725); // 1.4738 = 370 = 3.35 // 1.4725 = 380 = 3.26
      fFunc[i] = new TF1(Form("gaus_%d", i), "[0]*exp(-0.5*((x-[1])/[2])*(x-[1])/[2])", 0.7, 0.9);
      fFunc[i]->SetParameter(0, 1);
      fFunc[i]->SetParameter(1, fAngle[i]);
      fFunc[i]->SetParameter(2, sigma[i]);
    }

    //smear track
    // momInBar0 = momInBar;
    // double smearangle =  gRandom->Gaus(0,0.001);
    // momInBar.RotateX(smearangle);
    // momInBar.RotateY(smearangle);    
    // momInBar.Rotate(gRandom->Uniform(0,TMath::TwoPi()),momInBar0);

    double trackingResTheta = 0.001;
    momInBar.SetTheta(gRandom->Gaus(momInBar.Theta(), trackingResTheta));
    momInBar.SetPhi(gRandom->Gaus(momInBar.Phi(), trackingResTheta));
    
    if (fVerbose == 3) {
      cz = momInBar.Unit();
      cz = TVector3(-cz.X(), cz.Y(), cz.Z());
    }

    // if(fMethod==2 && pid==4) continue;
 
    double sm = 0;
    if (!bsim) {
      int gch, ndirc(0), t2(0), t3h(0), t3v(0), str1(0), stl1(0), str2(0), stl2(0);
      int hodo1(0), hodo2(0);

      for (auto hit : fEvent->getHits()) {
        gch = hit.getChannel();
        if (gch < fmaxch) ndirc++;

        if (gch == 513) t2++;
        if (gch == 514) t3h++;
        if (gch == 515) t3v++;

        if (fMethod == 3) {
           if (gch >= 1094 && gch <= 1101) hodo1++;
        } else {
	  if (gch >= 1089 && gch <= 1106) hodo1++;
        }

        // if(gch>=1095 && gch<=1095) hodo1++;
        if (gch == 1140) str1++;
        if (gch == 1142) stl1++;
        if (gch == 1144) str2++;
        if (gch == 1146) stl2++;

        // if(gch>=1115 && gch<=1120)
        hodo2++;
      }

      if (ndirc < 5) continue;
      if (!(hodo1 && hodo2)) continue;
      if (!(t3h && t3v)) continue;
      if (!t2) continue;
      if (!(str1 && stl1 && str2 && stl2)) continue;
      
      double tof = fEvent->getTof();
      double tofPi = fEvent->getTofPi();
      double tofP = fEvent->getTofP();
      double s1 = 0, s2 = 0, c = 3 * 0.18; // 3 sigma cut
      hTof[pid]->Fill(tof);

      // tof cuts
      if (fabs(mom - 7) < 0.1) {
        if (fMethod == 4) {
          if (fabs(0.5 * fabs(tofPi + tofP) - tof) < 0.25) continue;
	  if (fabs(0.5 * fabs(tofPi + tofP) - tof) > 0.85) continue;
        } else {
          if (fabs(0.5 * fabs(tofPi + tofP) - tof) < 0.65) continue;
	  if (fabs(0.5 * fabs(tofPi + tofP) - tof) > 0.85) continue;
        }
      }
      
      hTofc[pid]->Fill(tof);
    } else {
      if (fStudyId == 415 || fStudyId == 436) {
        if (fabs(mom - 3) < 0.1) sm = 0.35;
        if (fabs(mom - 4) < 0.1) sm = 0.3;
        if (fabs(mom - 5) < 0.1) sm = 0.2;
      }
    }
    
    // SearchClusters();

    double t0smear = gRandom->Gaus(0, 0.1 + sm); // event t0 smearing    
    // double temp_ti[fmaxch] = {0};

    for (auto hit : fEvent->getHits()) {
      
      hitTime = hit.getLeadTime();
      int pixid = hit.getPixel();
      int mcpid = hit.getPmt();
      int ch =  ft.map_pmtpix[mcpid][pixid]; // hit.getChannel();//
      int pathid = hit.getPathInPrizm();
      
      // TString spathid = Form("%d",pathid);
      // if(pathid != 142) continue;
      // if(!spathid.BeginsWith("1")) continue;
      // if(!spathid.Contains("142")) continue;
      // std::cout<<"pathid "<<pathid<<std::endl;
      
      if (bsim) hitTime += gRandom->Gaus(0, 0.20) + t0smear; // time resol. if not simulated
      
      //======================================== dynamic cuts
      {
        { // time cuts
          if (prtangle <= 75) {
            if (hitTime < 7 || hitTime > 45) continue;
            reflected = kTRUE;
          } else if (prtangle > 105) {
            if (hitTime < 3 || hitTime > 40) continue;
            reflected = kFALSE;
          } else {
            if (hitTime < 11)
              reflected = kFALSE;
            else
              reflected = kTRUE;
          }
        }
      }
      //==================================================
      
      if (fVerbose == 3) {
        // TVector3 cd = hit.getMomentum();
        // fHist5->Fill(cd.Theta()*TMath::Sin(cd.Phi()),cd.Theta()*TMath::Cos(cd.Phi()));
      }

      // TVector3 vv = hit.getMomentum();
      // vv.RotateY(prtangle*deg);
      // dirz = vv.Z();
      // if(dirz<0) reflected = kTRUE;
      // else reflected = kFALSE;

      // if(!reflected) continue;
      if (reflected) lenz = 2 * radiatorL - posz;
      else lenz = posz;
      
      // hitTime += fCor_time[mcpid];
      hitTime += fCor_time_refl[reflected];
      if (ft.is_bad_channel(ch)) continue;

      int nedge = 0;
      // if(cluster[mcpid][pixid]>4) continue;
      
      bool isGoodHit_gr(0), isGoodHit_ti(0);
      int bestbounce = 0;
      double besttangle = 0, besttdiff = 100;
      int size = fLutNode[ch]->Entries();

      if (fMethod == 4 && bsim) {
	hitTime += gRandom->Gaus(0, 0.2);
        isGoodHit_ti = true;
	size = 0;
      }else{
	nedge = GetEdge(mcpid, pixid);
      }

      for (int i = 0; i < size; i++) {

        weight = 12 * fLutNode[ch]->GetWeight(i);
        if (fnpix >= 64) dird = fLutNode[ch]->GetEntry(i);
        else dird = fLutNode[ch]->GetEntryCs(i, nedge);
        evtime = fLutNode[ch]->GetTime(i);	
        int lpathid = fLutNode[ch]->GetPathId(i);

        bool samepath(false);
        if (bsim && lpathid == pathid) samepath = true;
        // if(!samepath) continue;

        double lphi = dird.Phi();
        double ltheta = dird.Theta();
        if (lphi < 0) lphi = TMath::TwoPi() + lphi;
        if (ltheta > TMath::PiOver2()) ltheta = TMath::Pi() - ltheta;
        int iphi = nphi * (lphi) / TMath::TwoPi();
        int itheta = ntheta * (ltheta) / TMath::PiOver2();

        for (int u = 0; u < 4; u++) {
          if (u == 0) dir = dird;
          if (u == 1) dir.SetXYZ(-dird.X(), dird.Y(), dird.Z());
          if (u == 2)
            dir.SetXYZ(dird.X(), -dird.Y(),
                       dird.Z()); // no need when no divergence in vertical plane
          if (u == 3)
            dir.SetXYZ(-dird.X(), -dird.Y(),
                       dird.Z()); // no need when no divergence in vertical plane
          if (reflected) dir.SetXYZ(dir.X(), dir.Y(), -dir.Z());
          if (dir.Angle(fnX1) < criticalAngle || dir.Angle(fnY1) < criticalAngle) continue;

          luttheta = dir.Theta();
          if (luttheta > TMath::PiOver2()) luttheta = TMath::Pi() - luttheta;

          double len = lenz / cos(luttheta);
          bartime = fabs(len / speed);
          double luttime = bartime + evtime;
          double tdiff = hitTime - luttime;
          fHist0->Fill(tdiff);
          if (reflected) fHist0r->Fill(tdiff);
          else fHist0d->Fill(tdiff);
	  
          if (samepath) fHist0i->Fill(tdiff);
          tangle = momInBar.Angle(dir) + fCor_angle[mcpid];
 
          if (fabs(tdiff) < 1.5 && fCor_level > 0) {
            if (fabs(prtangle - 90) < 16) {
              if (reflected) tangle -= 0.0075 * tdiff; // chromatic correction
              if (!reflected) tangle -= 0.006 * tdiff; // chromatic correction
            } else if (fabs(prtangle - 60) < 15) {
              tangle -= 0.055 * tdiff / hitTime;
            } else {
	      tangle -= 0.035 * tdiff / hitTime;
            }
          }
 
          hChrom->Fill(tdiff, (tangle - fAngle[pid]) * 1000);
          hChromL->Fill(tdiff / hitTime, (tangle - fAngle[pid]) * 1000);

          // if (fMethod == 4) {
          //   if (fabs(tdiff) < 0.8 + luttime * 0.04) {
          //     if (pid == 2 && fabs(tangle - fAngle[2]) < 0.02) isGoodHit_ti = true;
          //     if (pid == fPk && fabs(tangle - fAngle[fPk]) < 0.02) isGoodHit_ti = true;
          //     isGoodHit_ti = true;
          //   }
          // } else {
          if (fabs(tdiff) < 0.8 + luttime * 0.04 &&
              (fabs(tangle - fAngle[1]) < 0.03 || fabs(tangle - fAngle[fPk]) < 0.03))
            isGoodHit_ti = true;
          // }

          if (fabs(tdiff) > timeRes + luttime * 0.04) continue;

          fHistTime[mcpid]->Fill(tdiff);
          fHistTimeRefl[reflected]->Fill(tdiff);
          fDiff->Fill(hitTime, tdiff);
          fHist3->Fill(fabs(luttime), hitTime);

          if (tangle > minChangle && tangle < maxChangle && tangle < 1.85) {
	    if (pid == 1) fHist->Fill(tangle, weight);

	    // fAngleDiff->Fill(tangle-fAngle[pid], weight);

            if (pid == 1) fHistMcp[mcpid]->Fill(tangle - fAngle[pid], weight);

            fHistCh[ch]->Fill(tangle - fAngle[pid], weight);

            if (fabs(tangle - fAngle[1]) < cwindow || fabs(tangle - fAngle[fPk]) < cwindow) {

              isGoodHit_gr = true;

              sum1 += weight * TMath::Log(fFunc[fPk]->Eval(tangle) + noise);
              sum2 += weight * TMath::Log(fFunc[2]->Eval(tangle) + noise);

              fHist0s->Fill(tdiff);
              double lenx = len * dir.X();
              double leny = len * dir.Y();
              int nx = round(lenx / radiatorH);
              int ny = round(leny / radiatorW);
              if (fabs(tdiff) < fabs(besttdiff)) {
                besttdiff = tdiff;
                bestbounce = nx + ny;
                besttangle = tangle;
              }

              fHist2->Fill(luttime);
              hLutCorrD->Fill(ltheta * TMath::Sin(lphi), ltheta * TMath::Cos(lphi));
            }
            if (fVerbose == 3) {
              TVector3 rdir = TVector3(-dir.X(), dir.Y(), dir.Z());
              rdir.RotateUz(cz);
              double llphi = rdir.Phi();
              double tt = rdir.Theta();
              fHist4->Fill(tt * TMath::Sin(llphi), tt * TMath::Cos(llphi));

              // for cherenckov circle fit
              fgg_gr.SetPoint(fgg_i, tt * TMath::Sin(llphi), tt * TMath::Cos(llphi));
              fgg_i++;
            }
          }
        }
      }
      
      if (fTimeImaging && isGoodHit_ti) {
	
        if (fMethod == 2) {
          double t = hitTime;
          // if(fabs(besttdiff) < 0.3) t -= besttdiff;
          double noiseti = 1e-5;
          double aminti4 = fPdf4[ch]->Eval(t);
          double aminti2 = fPdf2[ch]->Eval(t);

          sumti4 += TMath::Log((aminti4 + noiseti));
          sumti2 += TMath::Log((aminti2 + noiseti));

          if (0) {
            TString x = (aminti4 > aminti2) ? " <====== PROTON" : "";
            std::cout << Form("f %1.6f s %1.6f mcp %d pix %d   pid %d", aminti4, aminti2, mcpid,
                              pixid, pid)
                      << "  " << x << std::endl;

            ft.add_canvas("ctemp", 800, 400);
            // ft.normalize(fPdf4[ch],fPdf2[ch]);
            fPdf4[ch]->SetLineColor(2);
            fPdf2[ch]->SetLineColor(4);
            fPdf4[ch]->Draw("APL");
            fPdf4[ch]->SetTitle(Form("mcp=%d  pix=%d", mcpid, pixid));
            fPdf4[ch]->GetXaxis()->SetTitle("LE time [ns]");
            fPdf4[ch]->GetYaxis()->SetTitle("PDF value");
            fPdf4[ch]->GetXaxis()->SetRangeUser(0, 40);
            fPdf2[ch]->Draw("PL same");
            gPad->Update();
            TLine *gLine = new TLine(0, 0, 0, 1000);
            gLine->SetLineWidth(2);
            gLine->SetX1(t);
            gLine->SetX2(t);
            gLine->SetY1(gPad->GetUymin());
            gLine->SetY2(gPad->GetUymax());
            gLine->Draw();
            gPad->Update();
            gPad->WaitPrimitive();
          }
        }

        if (fMethod == 4) { // create pdf
          double t = hitTime;
	  double w = 1;
          // temp_ti[ch] = t;
          // if(fabs(besttdiff) < 0.3) t -= besttdiff;
          if (pid == fPk) {
            total4++;
            fTime4[ch]->Fill(t,w);
          }
          if (pid == 2) {
            total2++;
            fTime2[ch]->Fill(t,w);
          }
        }
      }

      if (isGoodHit_gr) {
        hBounce->Fill(bestbounce);
        fHist1->Fill(hitTime);
        nhhits++;
        nsHits++;
        if (pid == 1) ft.fill_digi(mcpid,pixid);
      }
    }
    
    double sum_nph = 0;
    if (mom < 2.5) { // photon yield likelihood
      TF1 *f_pi = new TF1("gaus", "gaus", 0, 150);
      f_pi->SetParameters(1, 50, 8.7);
      TF1 *f_p = new TF1("gaus", "gaus", 0, 150);
      f_p->SetParameters(1, 30, 7.1);

      double lh_nph_p = f_p->Eval(nhhits);
      double lh_nph_pi = f_pi->Eval(nhhits);
      sum_nph = lh_nph_p - lh_nph_pi;
    }

    if (fMethod == 2 && fTimeImaging) { // time imaging
      sumti = 1.5 * (sumti4 - sumti2) + 30 * sum_nph;
      if (sumti != 0) hLnDiffTi[pid]->Fill(sumti);      
    }

    double sumgr = sum1 - sum2 + 30 * sum_nph;
    if (sumgr != 0) {
      hNph[pid]->Fill(nhhits);
      hLnDiffGr[pid]->Fill(1*sumgr);
      likelihood = sumgr;
      events[pid]++;
    }
    
    // if(fMethod == 4){
    //   for(int ch=0; ch<fmaxch; ch++){
    // 	if(temp_ti[ch]>0){
    // 	  if(pid == fPk && sumgr > 10){
    // 	    total4++;
    // 	    fTime4[ch]->Fill(temp_ti[ch]);
    // 	  }
    // 	  if(pid == 2 && sumgr < -10){
    // 	    total2++;
    // 	    fTime2[ch]->Fill(temp_ti[ch]);
    // 	  }
    // 	}
    //   }
    // }

    if (pid == fPk && sumti != 0) hLnMap->Fill(sumgr, sumti);

    // if(fVerbose==1){
    //   ft.add_canvas("ff",800,400);
    //   fFunc[fPk]->Draw();
    //   fFunc[2]->SetLineColor(4);
    //   fFunc[2]->Draw("same");

    //   ft.wait_primitive("ff");
    //   ft.del_canvas("ff");
    //   //ft.save_canvas(1,0);
    //   //ft.del_canvas(Form("lh_%d",fgg_ind));
    // }

    if (fVerbose > 0 && fMethod == 3 && nsEvents % ninfit == 0) {
      if (nsHits > 5) {
        FindPeak(cangle, spr, cangle_pi, spr_pi, prtangle, pid);

	fCtr->Fill(1000*(cangle-fAngle[1]));
	distPid = FindPdg(mom, cangle);
        nph = nsHits / (double)ninfit;
        spr = spr * 1000;
        trr = spr / sqrt(nph);
        theta = frun->getTheta();
      }
      ResetHists();
      nsHits = 0;
    }
  }

  if (fMethod == 4) { // create pdf
    std::cout << "saving pdfs into " << fPdfPath << std::endl;

    TFile efile(fPdfPath, "RECREATE");
    for (int i = 0; i < fmaxch; i++) {
      fTime2[i]->Scale(1 / (Double_t)total2);
      fTime4[i]->Scale(1 / (Double_t)total4);
      fTime2[i]->Write();
      fTime4[i]->Write();
    }
    efile.Write();
    efile.Close();
    std::cout << "total2 " << total2 << " total4 " << total4 << std::endl;
  }

  if (fMethod == 2) {
    TF1 *ff;
    gROOT->SetBatch(1);
    if (hNph[fPk]->GetEntries() > 20) {
      auto r = hNph[fPk]->Fit("gaus", "SQ", "", 0, 200);
      nph = r->Parameter(1);
      nph_err = r->ParError(1);
    }
    if (hNph[1]->GetEntries() > 20) {
      auto r = hNph[1]->Fit("gaus", "SQ", "", 0, 200);
      nph_pi = r->Parameter(1);
      nph_pi_err = r->ParError(1);
    }

    // nph = ft.fit(hNph[fPk],40,10,50,1).X();
    gROOT->SetBatch(0);
    FindPeak(cangle, spr, cangle_pi, spr_pi, prtangle);
    // nph = nsHits/(double)nsEvents;
    spr = spr * 1000;
    trr = spr / sqrt(nph);
    spr_pi = spr_pi * 1000;
    trr_pi = spr_pi / sqrt(nph_pi);
    theta = frun->getTheta();

    double m1, m2, s1, s2, dm1, dm2, ds1, ds2;
    if (hLnDiffGr[fPk]->GetEntries() > 50) {
      hLnDiffGr[fPk]->Fit("gaus", "Q");
      ff = hLnDiffGr[fPk]->GetFunction("gaus");
      if (ff) {
        ff->SetLineColor(kBlack);
        m1 = ff->GetParameter(1);
        s1 = ff->GetParameter(2);
        dm1 = ff->GetParError(1);
        ds1 = ff->GetParError(2);
      }
    }

    if (hLnDiffGr[2]->GetEntries() > 50) {
      hLnDiffGr[2]->Fit("gaus", "Q");
      ff = hLnDiffGr[2]->GetFunction("gaus");
      if (ff) {
        ff->SetLineColor(kBlack);
        m2 = ff->GetParameter(1);
        s2 = ff->GetParameter(2);
        dm2 = ff->GetParError(1);
        ds2 = ff->GetParError(2);
      }
    }    

    sep_gr = (fabs(m1 - m2)) / (0.5 * (s1 + s2));

    double e1, e2, e3, e4;
    e1 = 2 / (s1 + s2) * dm1;
    e2 = -2 / (s1 + s2) * dm2;
    e3 = -2 * (m1 - m2) / ((s1 + s2) * (s1 + s2)) * ds1;
    e4 = -2 * (m1 - m2) / ((s1 + s2) * (s1 + s2)) * ds2;
    sep_gr_err = sqrt(e1 * e1 + e2 * e2 + e3 * e3 + e4 * e4);
    
    if (fTimeImaging) {
      if (hLnDiffTi[fPk]->Integral() > 10) {
        hLnDiffTi[fPk]->Fit("gaus", "Q");
        ff = hLnDiffTi[fPk]->GetFunction("gaus");
        if (ff) {
          m1 = ff->GetParameter(1);
          s1 = ff->GetParameter(2);
          dm1 = ff->GetParError(1);
          ds1 = ff->GetParError(2);
          ff->SetLineColor(kBlack);
        }
      }

      if (hLnDiffTi[2]->Integral() > 10) {
        hLnDiffTi[2]->Fit("gaus", "Q");
        ff = hLnDiffTi[2]->GetFunction("gaus");
        if (ff) {
          m2 = ff->GetParameter(1);
          s2 = ff->GetParameter(2);
          dm2 = ff->GetParError(1);
          ds2 = ff->GetParError(2);
          ff->SetLineColor(kBlack);
        }
      }

      sep_ti = (fabs(m1 - m2)) / (0.5 * (s1 + s2));

      e1 = 2 / (s1 + s2) * dm1;
      e2 = -2 / (s1 + s2) * dm2;
      e3 = -2 * (m1 - m2) / ((s1 + s2) * (s1 + s2)) * ds1;
      e4 = -2 * (m1 - m2) / ((s1 + s2) * (s1 + s2)) * ds2;
      sep_ti_err = sqrt(e1 * e1 + e2 * e2 + e3 * e3 + e4 * e4);
    }
    
    nnratio_pi = hNph[1]->GetEntries() / (double)end;
    nnratio_p = hNph[fPk]->GetEntries() / (double)end;

    if (fVerbose) {
      std::cout << "nnratio_pi " << nnratio_pi << " " << end << "  " << hNph[1]->GetEntries()
                << std::endl;
      std::cout << Form("p  SPR = %2.2F N = %2.2f +/- %2.2f", spr, nph, nph_err) << std::endl;
      std::cout << Form("mu SPR = %2.2F N = %2.2f +/- %2.2f", spr_pi, nph_pi, nph_pi_err)
                << std::endl;
      std::cout << "sep gr " << sep_gr << " +/- " << sep_gr_err << std::endl;
      std::cout << "sep ti " << sep_ti << " +/- " << sep_ti_err << std::endl;

      TGaxis::SetMaxDigits(3);
      double a = prtangle;
      TString nid = ""; // Form("_%2.0f",a);

      // plots
      { // cherenkov angle
        ft.add_canvas("tangle" + nid, 800, 400);

        // fHist->SetTitle(Form("theta %3.1f", a));
        fHist->SetTitle("");
        fHist->SetMinimum(0);
        // fHist->Scale(1/fHist->GetMaximum());

        ft.normalize(fHist, fHistPi);
        fHist->SetLineColor(ft.color(fPk));
        fHist->SetMarkerStyle(20);
        fHist->SetMarkerSize(0.7);
        fHist->SetMarkerColor(ft.color(fPk) + 1);
        fHist->GetXaxis()->SetRangeUser(0.77, 0.9);
        auto f = fHist->GetFunction("fgaus");
        if (f) {
          f->SetLineColor(kBlack);
        }
	fHist->Draw("h");
        fHistPi->SetLineColor(ft.color(1));
        fHistPi->SetMarkerStyle(20);
        fHistPi->SetMarkerSize(0.7);
        fHistPi->SetMarkerColor(ft.color(1) + 1);
        f = fHistPi->GetFunction("fgaus");
        if (f) {
          f->SetLineColor(kBlack);
        }
        // fHistPi->Draw("h same");
        // fFunc[4]->Draw("same");
        // fFunc[2]->Draw("same");
        fHisti->SetLineColor(kRed + 2);
        if (fHisti->GetEntries() > 5) fHisti->Draw("same");

        TLegend *l = new TLegend(0.6, 0.68, 0.91, 0.82);
        l->SetFillColor(0);
        l->SetFillStyle(0);
        l->SetBorderSize(0);
        l->SetFillStyle(0);
        l->AddEntry(fHist, "#mu, e", "lp");
        l->Draw();

        TLegend *leg = new TLegend(0.55, 0.52, 0.8, 0.65);
        leg->SetFillColor(0);
        leg->SetFillStyle(0);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->AddEntry((TObject *)0, Form("#sigma_{#mu,e} = %2.2f mrad", spr), "");
        leg->Draw();

        drawTheoryLines();
      }

      { // time
        ft.add_canvas("time", 800, 400);
        ft.normalize(fHist1, fHist2);
        fHist1->SetTitle(Form("theta %3.1f", a));
        fHist2->SetLineColor(2);
        fHist1->Draw();
        fHist2->Draw("same");

        // fHist1->Scale(1/4000.);
        // fHist1->Rebin(10);
        // fHist1->Draw("p hist");

        ft.add_canvas("diff_time", 800, 400);
        fDiff->Draw("colz");

        ft.add_canvas("time_diff" + nid, 800, 400);
        fHist0->SetTitle(Form("theta %3.1f", a));
        fHist0->SetLineColor(kBlack);
        fHist0->Draw();
        fHist0d->SetLineColor(kGreen + 1);
        fHist0d->Draw("same");
        fHist0r->SetLineColor(kBlue + 1);
        fHist0r->Draw("same");
        fHist0s->SetLineColor(kOrange + 6);
        fHist0s->Draw("same");
        fHist0i->SetLineColor(kRed + 1);
        if (fHist0i->GetEntries() > 5) fHist0i->Draw("same");

        // ft.add_canvas("cm"+nid,800,400);
        // fHist3->SetTitle(Form("theta %3.1f", a));
        // fHist3->Draw("colz");
      }

      { // tof
      //   ft.add_canvas("tof", 800, 400);
      //   for (int i : {2, fPk}) {
      //     hTof[i]->SetLineColor(ft.color(i));
      //     hTofc[i]->SetLineColor(ft.color(i));
      //     hTofc[i]->SetFillColor(ft.color(i));
      //     hTofc[i]->SetFillStyle(3005);
      //     hTof[i]->Draw((i == 2) ? "" : "same");
      //     hTofc[i]->Draw("same");
      //   }
      }

      { // likelihood
        //   ft.add_canvas("lhood_gr", 800, 400);
        //   ft.normalize(hLnDiffGr[fPk], hLnDiffGr[2]);
        //   hLnDiffGr[fPk]->SetMarkerStyle(20);
        //   hLnDiffGr[fPk]->SetMarkerSize(0.85);
        //   hLnDiffGr[fPk]->SetLineColor(kRed);
        //   hLnDiffGr[fPk]->SetMarkerColor(kRed + 1);
        //   hLnDiffGr[fPk]->SetName(Form("s_%2.2f", sep_gr));
        //   hLnDiffGr[fPk]->Draw("E");
        //   hLnDiffGr[2]->SetMarkerStyle(20);
        //   hLnDiffGr[2]->SetMarkerSize(0.85);
        //   hLnDiffGr[2]->SetLineColor(kBlue);
        //   hLnDiffGr[2]->SetMarkerColor(kBlue + 1);
        //   hLnDiffGr[2]->Draw("E same");

        //   TLegend *leg = new TLegend(0.7, 0.7, 0.9, 0.9);
        //   leg->SetFillColor(0);
        //   leg->SetFillStyle(0);
        //   leg->SetBorderSize(0);
        //   leg->SetFillStyle(0);
        //   leg->AddEntry(hLnDiffGr[2], ft.name(2), "lp");
        //   leg->AddEntry(hLnDiffGr[fPk], ft.name(fPk), "lp");
        //   leg->Draw();

        //   if (fTimeImaging) {
        //     ft.add_canvas("lhood_ti", 800, 400);
        //     ft.normalize(hLnDiffTi[fPk], hLnDiffTi[2]);
        //     hLnDiffTi[fPk]->SetLineColor(kRed + 1);
        //     hLnDiffTi[fPk]->SetMarkerStyle(20);
        //     hLnDiffTi[fPk]->SetMarkerSize(0.85);
        //     hLnDiffTi[fPk]->SetMarkerColor(kRed + 1);
        //     hLnDiffTi[fPk]->SetName(Form("s_%2.2f", sep_ti));
        //     hLnDiffTi[fPk]->Draw("E");
        //     hLnDiffTi[2]->SetLineColor(kBlue + 1);
        //     hLnDiffTi[2]->SetMarkerStyle(20);
        //     hLnDiffTi[2]->SetMarkerSize(0.85);
        //     hLnDiffTi[2]->SetMarkerColor(kBlue + 1);
        //     hLnDiffTi[2]->Draw("E same");
        //     leg->Draw();
        //   }

        //   // ft.add_canvas("lhood_map",800,800);
        //   // hLnMap->SetStats(0);
        //   // hLnMap->Draw("colz");
      }

      { // bounce
        //   ft.add_canvas("bounce", 800, 400);
        //   hBounce->SetTitle(Form("theta %3.1f , TOF PID = %d", a, pid));
        //   hBounce->Scale(1 / 4000.);
        //   hBounce->Draw("hist");
      }

      { // lut corrections
        // ft.add_canvas("lutcorrd"+nid,600,600);
        // hLutCorrD->SetStats(0);
        // hLutCorrD->Draw("colz");
      }

      { // chromatic corrections
        ft.add_canvas("chrom" + nid, 800, 400);
        hChrom->SetStats(0);
        hChrom->Draw("colz");
        ft.add_canvas("chroml" + nid, 800, 400);
        hChromL->SetStats(0);
        hChromL->Draw("colz");
      }

      { // nph
        ft.add_canvas("nph" + nid, 800, 400);

        TLegend *leg = new TLegend(0.7, 0.7, 0.9, 0.9);
        leg->SetFillColor(0);
        leg->SetFillStyle(0);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);

        for (int i : {1, fPk}) {
          hNph[i]->SetLineColor(ft.color(i));
          auto f = hNph[i]->GetFunction("gaus");
          if (f) {
            f->SetLineWidth(2);
            f->SetLineColor(kRed + 1);
          }
          hNph[i]->Draw((i == 1) ? "" : "same");
          leg->AddEntry(hNph[i], ft.name(i), "lp");
        }
        leg->Draw();
      }

      { // hp
        auto cdigi = ft.draw_digi(0, 0);
        cdigi->SetName("hp" + nid);
        ft.add_canvas(cdigi);
      }
      
      if (fVerbose == 3) {
        TCanvas *cCher = new TCanvas("cCher", "cCher", 0, 0, 800, 400);
        cCher->Divide(2, 1);
        cCher->cd(1);

        fHist4->SetStats(0);
        fHist4->SetTitle(Form("Calculated from LUT, #theta = %3.1f#circ", a));
        fHist4->Draw("colz");
        double x0(0), y0(0);
        FitRing(x0, y0, cangle);
        TVector3 corr(x0, y0, 1 - TMath::Sqrt(x0 * x0 + y0 * y0));
        std::cout << "Tcorr " << corr.Theta() * 1000 << "  Pcorr " << corr.Phi() << std::endl;

        TLegend *leg = new TLegend(0.5, 0.7, 0.85, 0.87);
        leg->SetFillStyle(0);
        leg->SetBorderSize(0);
        leg->AddEntry((TObject *)0, Form("Entries %0.0f", fHist4->GetEntries()), "");
        leg->AddEntry((TObject *)0, Form("#Delta#theta_{c} %f [mrad]", corr.Theta() * 1000), "");
        leg->AddEntry((TObject *)0, Form("#Delta#varphi_{c} %f [mrad]", corr.Phi()), "");
        leg->Draw();

        TArc *arc = new TArc(x0, y0, cangle);
        arc->SetLineColor(kRed);
        arc->SetLineWidth(1);
        arc->SetFillStyle(0);
        arc->Draw();
        fgg_i = 0;
        fgg_gr.Set(0);

        cCher->cd(2);
        gStyle->SetOptStat(1110);
        fHist5->SetTitle(Form("True from MC, #theta = %3.1f#circ", a));
        fHist5->Draw("colz");

        ft.add_canvas(cCher);
      }
      gROOT->SetBatch(0);
    }
  }

  if (fMethod == 3) {

    { // Cherenkov track resolution
      ft.add_canvas("ctr", 800, 400);      
      auto r = fCtr->Fit("gaus", "SQ", "", -5, 5);
      ctr_sigma = r->Parameter(2);
      ctr_err = r->ParError(2);

      std::cout << "ctr_sigma " << ctr_sigma << " +/- " << ctr_err << std::endl;

      fCtr->Draw();

      ft.add_canvas("nph", 800, 400);      
      hNph[1]->Draw();
    }
  }

  tree.Fill();
  file.Write();

  ft.set_style();
  if (fVerbose > 1) ft.wait_primitive("time", "");
  if (fVerbose) {
    ft.save_canvas(2, 0, true);
    ResetHists();
  }
}

int g_num = 0;
bool PrtLutReco::FindPeak(double &cangle, double &spr, double &cangle_pi, double &spr_pi, double a,
                          int tofpdg) {
  cangle = 0;
  spr = 0;
  gROOT->SetBatch(1);

  if (fHist->GetEntries() > 20 || fHistPi->GetEntries() > 20) {
    cangle = fHist->GetXaxis()->GetBinCenter(fHist->GetMaximumBin());
    if (cangle > 0.84) cangle = 0.82;
    fFit->SetParameters(100, cangle, 0.008);
    fFit->SetParNames("p0", "#theta_{c}", "#sigma_{c}", "p3", "p4");
    fFit->SetParLimits(0, 10, 1E6);
    fFit->SetParLimits(1, cangle - 0.03, cangle + 0.03);
    fFit->SetParLimits(2, 0.005, 0.012);

    if (fMethod == 3) {
      fFit->SetParameters(10, 0.5 * (fAngle[fPk] + fAngle[1]), 0.006);
      fFit->SetParLimits(2, 0.003, 0.007);
      fFit->SetParLimits(1, fAngle[fPk] - 0.01, fAngle[1] + 0.01);
      gROOT->SetBatch(1);
      fHist->Fit("fgaus", "LQ", "", 0.6, 1);
      cangle = fFit->GetParameter(1);
      spr = fFit->GetParameter(2);
      gROOT->SetBatch(!fVerbose);

      if (fVerbose > 2) {
        gROOT->SetBatch(0);
        TString nid = Form("casphi_%2.3f_mrad", PrtManager::Instance()->getRun()->getTest1());
        ft.add_canvas(nid, 800, 400);
        fHist->SetTitle(Form("%d", tofpdg));
        if (tofpdg == 2212) fHist->SetLineColor(kRed);
        else fHist->SetLineColor(kBlue);
        fHist->Draw("h");
        drawTheoryLines();
        ft.wait_primitive(nid);
        // ft.save_canvas(1,0);
      }
    }

    if (fMethod == 2) {
      gROOT->SetBatch(1);

      double range = 0.07;
      fHist->Fit("fgaus", "Q", "", cangle - range, cangle + range);
      // fHist->Fit("fgaus","Q","",0.76,0.9);
      if (fVerbose > 1) gROOT->SetBatch(0);
      cangle = fFit->GetParameter(1);
      spr = fFit->GetParameter(2);

      cangle = fHistPi->GetXaxis()->GetBinCenter(fHistPi->GetMaximumBin());
      fFit->SetParLimits(1, cangle - 0.03, cangle + 0.03);
      fHistPi->Fit("fgaus", "Q", "", cangle - range, cangle + range);
      // fHistPi->Fit("fgaus","Q","",0.76,0.9);
      cangle_pi = fFit->GetParameter(1);
      spr_pi = fFit->GetParameter(2);

      // if(fCreateCorr){ // pre ch corrections
      // 	std::cout<<"Writing "<<fCorrPath<<std::endl;

      // 	TFile fc(fCorrPath,"recreate");
      // 	TTree *tc = new TTree("corr","corr");
      // 	int ch, pmt;
      // 	double corr;
      // 	tc->Branch("pmt",&pmt,"pmt/I");
      // 	tc->Branch("ch",&ch,"ch/I");
      // 	tc->Branch("corr",&corr,"corr/D");

      // 	fFit->SetParameters(100,0,0.007);
      // 	fFit->SetParLimits(1,-0.02,0.02);// mean
      // 	fFit->SetParLimits(2,0.004,0.009); // width
      // 	fFit->FixParameter(3,0);
      // 	fFit->FixParameter(4,0);
      // 	for(ch=0; ch<fmaxch; ch++){
      // 	  double integral =
      // fHistCh[ch]->Integral(fHistCh[ch]->GetXaxis()->FindBin(-0.02),fHistCh[ch]->GetXaxis()->FindBin(0.02));
      // 	  if(integral<100) continue;
      // 	  fHistCh[ch]->Fit("fgaus","NQ","",-0.03,0.03);
      // 	  corr = -fFit->GetParameter(1);
      // 	  if(fabs(corr) > 0.005) continue;
      // 	  tc->Fill();
      // 	  std::cout<<"if(ch == "<< ch<<") tangle += "<<corr<<";" <<std::endl;
      // 	  if(fVerbose>2){
      // 	    ft.add_canvas(Form("r_tangle_%d",ch),800,400);
      // 	    fHistCh[ch]->Fit("fgaus","MQ","",-0.03,0.03);
      // 	    fHistCh[ch]->Draw();
      // 	    drawTheoryLines();
      // 	  }
      // 	}

      // 	tc->Write();
      // 	fc.Write();
      // 	fc.Close();

      // 	fFit->ReleaseParameter(1);
      // 	fFit->ReleaseParameter(2);
      // 	fFit->ReleaseParameter(3);
      // 	fFit->ReleaseParameter(4);
      // }

      if (fCor_level < 2) { // corrections
        std::cout << "--- writing " << fCorrPath << std::endl;

        TFile fc(fCorrPath, "recreate");
        TTree *tc = new TTree("corr", "corr");
        int pmt, level = 0;
        double cor_angle, cor_time, cor_time_0 = 0, cor_time_1 = 0;
        tc->Branch("pmt", &pmt, "pmt/I");
	tc->Branch("level", &level, "level/I");
        tc->Branch("cor_angle", &cor_angle, "cor_angle/D");
        tc->Branch("cor_time", &cor_time, "cor_time/D");
        tc->Branch("cor_time_0", &cor_time_0, "cor_time_0/D");
        tc->Branch("cor_time_1", &cor_time_1, "cor_time_1/D");
        tc->Branch("spr_pi", &spr_pi, "spr_pi/D");

        fFit->SetParameters(100, 0, 0.005);
        fFit->SetParLimits(1, -0.011, 0.011); // mean
        fFit->SetParLimits(2, 0.004, 0.006);  // width
	
        for (pmt = 0; pmt < fnpmt; pmt++) {

          if (fCor_level == 1) {
            if (fHistMcp[pmt]->GetEntries() < 10000) continue;
            fHistMcp[pmt]->Fit("fgaus", "NQ", "", -0.05, 0.05);
            cor_angle = -fFit->GetParameter(1);
	    level = 2;
          }

          if (fCor_level == 0) {
            auto ff = fHistTime[pmt]->Fit("gaus", "NQS", "", -1, 1);
            cor_time = -ff->Parameter(1);
            cor_time_0 = -ft.fit(fHistTimeRefl[0], 0.5, 100, 2, 1, 0, "NQ").X();
            cor_time_1 = -ft.fit(fHistTimeRefl[1], 0.5, 100, 2, 1, 0, "NQ").X();
	    level = 1;
          } else {
            cor_time = fCor_time[pmt];
            cor_time_0 = fCor_time_refl[0];
            cor_time_1 = fCor_time_refl[1];
          }

          tc->Fill();
          std::cout << "pmt " << pmt << " cor_angle " << cor_angle << " cor_time " << cor_time
                    << " r " << cor_time_0 << " " << cor_time_1 << std::endl;
          if (fVerbose > 2) {
            if (fCor_level == 1) {
              ft.add_canvas(Form("r_tangle_%d", pmt), 800, 400);
              fHistMcp[pmt]->Fit("fgaus", "Q", "", -0.05, 0.05);
              fHistMcp[pmt]->Draw();
            } else {
              ft.add_canvas(Form("r_dtime_%d", pmt), 800, 400);
              fHistTime[pmt]->Fit("gaus", "Q", "", -1, 1);
              fHistTime[pmt]->Draw();

              ft.add_canvas(Form("r_dtime_refl_%d", pmt), 800, 400);
              ft.fit(fHistTimeRefl[0], 0.5, 100, 2, 1, 0, "Q");
              ft.fit(fHistTimeRefl[1], 0.5, 100, 2, 1, 0, "Q");
              ft.normalize(fHistTimeRefl, 2);
              fHistTimeRefl[0]->Draw();
              fHistTimeRefl[1]->SetLineColor(kRed);
              fHistTimeRefl[1]->Draw("same");
            }
          }
        }

        tc->Write();
        fc.Write();
        fc.Close();

        fFit->ReleaseParameter(1);
        fFit->ReleaseParameter(2);
        if (fVerbose > 2) gPad = 0;
      }
    }
  }

  return (cangle > 0 && cangle < 1);
}

void PrtLutReco::ResetHists() {
  fHist->Reset();
  fHisti->Reset();
  fHist0->Reset();
  fHist0i->Reset();
  fHist1->Reset();
  fHist2->Reset();
  fHist3->Reset();
  fHist4->Reset();
  ft.init_digi();
}

TF1 *lFit = new TF1("lgaus", "[0]*exp(-0.5*((x-[1])/[2])*(x-[1])/[2]) +x*[3]+[4]", 0.6, 0.9);
TF1 *lFitPi = new TF1("lgausPi", "[0]*exp(-0.5*((x-[1])/[2])*(x-[1])/[2]) +x*[3]+[4]", 0.6, 0.9);

double PrtLutReco::fillLnDiffPPi(double cangle, int pid, double mom) {
  if (fHist->GetEntries() > 20) {
    double angle1(0), angle2(0), sigma(0.006), range(0.015);

    // //fHist->Scale(1/fHist->GetMaximum());

    // double d1,d2, sum1(0),sum2(0);
    // int sbin = fHist->FindBin(fAngle[fPk]-range);
    // int ebin = fHist->FindBin(fAngle[fPk]+range);
    // // fHist->GetXaxis()->GetNbins()
    // for(int i=sbin; i< ebin; i++){
    //   if(fHist->GetBinContent(i) < 0.01 ) continue;
    //   d1 = fFunc[fPk]->Eval(fHist->GetBinCenter(i))- fHist->GetBinContent(i);
    //   d2 = fFunc[2]->Eval(fHist->GetBinCenter(i))- fHist->GetBinContent(i);

    //   std::cout<<"f1 "<< fFunc[fPk]->Eval(fHist->GetBinCenter(i)) << "   f2
    //   "<<fFunc[2]->Eval(fHist->GetBinCenter(i)) << "    v "<< fHist->GetBinContent(i)
    //   <<std::endl;

    //   // if(d1>0) sum1+=TMath::Log(d1);
    //   // if(d2>0) sum2+=TMath::Log(d2);
    //   sum1+=TMath::Log(fabs(d1));
    //   sum2+=TMath::Log(fabs(d2));

    // }

    lFit->SetRange(fAngle[fPk] - range, fAngle[1] + range);
    lFit->FixParameter(0, fHist->GetMaximum() - 0.5);
    lFit->FixParameter(1, fAngle[fPk]);
    lFit->FixParameter(2, 0.01);
    lFit->FixParameter(3, 0);
    lFit->FixParameter(4, 0.5);

    fHist->Fit("lgaus", "Q", "", fAngle[fPk] - range, fAngle[1] + range);
    double amin, amin2, edm, errdef;
    int nvpar, nparx;
    TVirtualFitter *fitter = TVirtualFitter::Fitter(fHist);
    fitter->GetStats(amin, edm, errdef, nvpar, nparx);

    // lFitPi->SetRange(fAnglePi-range,fAnglePi+range);
    // lFitPi->SetLineColor(4);
    // lFitPi->FixParameter(0,fFit->GetParameter(0));
    // lFitPi->FixParameter(1,fAnglePi);
    // lFitPi->FixParameter(2,sigma);
    // lFitPi->FixParameter(3,fFit->GetParameter(3));
    // lFitPi->FixParameter(4,fFit->GetParameter(4));

    lFitPi->SetRange(fAngle[fPk] - range, fAngle[1] + range);
    lFitPi->SetLineColor(4);
    lFitPi->FixParameter(0, fHist->GetMaximum() - 0.5);
    lFitPi->FixParameter(1, fAngle[1]);
    lFitPi->FixParameter(2, 0.01);
    lFitPi->FixParameter(3, 0);
    lFitPi->FixParameter(4, 0.5);

    fHist->Fit("lgausPi", "Q", "", fAngle[fPk] - range, fAngle[1] + range);
    fitter = TVirtualFitter::Fitter(fHist);
    fitter->GetStats(amin2, edm, errdef, nvpar, nparx);

    if (fVerbose)
      printf("tof pid %04d | %1.4f (%1.4f/%1.4f) likelihood is %1.2f/%1.2f \n", pid, cangle,
             fAngle[fPk], fAngle[1], amin, amin2);
    fgg_ind++;

    if (fVerbose == 1) {
      ft.add_canvas("ff", 800, 400);
      // ft.add_canvas(Form("lh_%d",fgg_ind),800,400);
      fHist->SetTitle(Form("%d", pid));
      fHist->Draw();
      lFit->SetLineColor(2);
      lFit->Draw("same");
      // fFunc[fPk]->Draw("same");
      // fFunc[2]->SetLineColor(4);
      // fFunc[2]->Draw("same");

      // if(fabs(amin-amin2)<5)
      ft.wait_primitive("ff");
      ft.del_canvas("ff");
      // ft.save_canvas(1,0);
      // ft.del_canvas(Form("lh_%d",fgg_ind));
    }

    return amin - amin2;
  }
  return 1000;
}

double PrtLutReco::fillLnDiffPPi2(double cangle, int pid, double mom) {
  if (fHist->GetEntries() > 20) {
    double angle1(0), angle2(0), sigma(0.006), range(0.03);

    double d1, d2, sum1(0), sum2(0);
    int sbin = fHist->FindBin(fAngle[1] - range);
    int ebin = fHist->FindBin(fAngle[fPk] + range);
    for (int i = sbin; i < ebin; i++) {
      if (fHist->GetBinContent(i) < 1) continue;
      d1 = 10 * fabs(fHist->GetBinContent(i) * (fAngle[fPk] - fHist->GetBinCenter(i)));
      d2 = 10 * fabs(fHist->GetBinContent(i) * (fAngle[1] - fHist->GetBinCenter(i)));
      if (d1 > 0 && d2 > 0) {
        std::cout << "d1  " << d1 << "   d2    " << d2 << std::endl;
        sum1 += TMath::Log(d1);
        sum2 += TMath::Log(d2);
      }
    }

    if (fVerbose)
      printf("tof pid %04d | %1.4f (%1.4f/%1.4f) likelihood is %1.2f/%1.2f \n", pid, cangle,
             fAngle[fPk], fAngle[1], sum1, sum2);
    fgg_ind++;

    if (fVerbose == 1) {
      ft.add_canvas("ff", 800, 400);
      // ft.add_canvas(Form("lh_%d",fgg_ind),800,400);
      fHist->SetTitle(Form("%d", pid));
      fHist->Draw();
      lFit->SetLineColor(2);
      lFit->Draw("same");
      // gFp->Draw("same");
      // gFpi->SetLineColor(4);
      // gFpi->Draw("same");

      // if(fabs(amin-amin2)<5)
      ft.wait_primitive("ff");
      ft.del_canvas("ff");
      // ft.save_canvas(1,0);
      // ft.del_canvas(Form("lh_%d",fgg_ind));
    }

    return sum1 - sum2;
  }
  return 1000;
}

void PrtLutReco::FitRing(double &x0, double &y0, double &theta) {
  TGraph ff_gr;
  int ff_i(0);
  int np = fgg_gr.GetN();
  double *x = fgg_gr.GetX();
  double *y = fgg_gr.GetY();
  for (int i = 0; i < np; i++) {
    if (fabs(theta - TMath::Sqrt(x[i] * x[i] + y[i] * y[i])) < 0.05) {
      ff_gr.SetPoint(ff_i, x[i], y[i]);
      ff_i++;
    }
  }
  fgg_gr = ff_gr;

  // Fit a circle to the graph points
  TVirtualFitter::SetDefaultFitter("Minuit"); // default is Minuit
  TVirtualFitter *fitter = TVirtualFitter::Fitter(0, 3);
  fitter->SetPrecision(0.00000001);
  fitter->SetMaxIterations(1000);
  double p1 = -1;
  fitter->ExecuteCommand("SET PRINTOUT", &p1, 1);

  fitter->SetFCN(circleFcn);
  fitter->SetParameter(0, "x0", 0.03, 0.01, -0.05, 0.05);
  fitter->SetParameter(1, "y0", 0, 0.01, -0.05, 0.05);
  fitter->SetParameter(2, "R", theta, 0.01, theta - 0.05, theta + 0.05);

  // fitter->FixParameter(0);
  // fitter->FixParameter(1);
  fitter->FixParameter(2);
  double arglist[1] = {0};
  fitter->ExecuteCommand("MINIMIZE", arglist, 0);

  // fitter->SetFCN(circleFcn2);
  // fitter->ExecuteCommand("MINIMIZE", arglist, 0);

  x0 = fitter->GetParameter(0);
  y0 = fitter->GetParameter(1);
  theta = fitter->GetParameter(2);
}

int PrtLutReco::FindPdg(double mom, double cangle) {
  cangle = fHist->GetXaxis()->GetBinCenter(fHist->GetMaximumBin());

  int pdg[] = {211, 2212};
  double mass[] = {0.139570, 0.9382723};
  double tdiff, diff = 100;
  int minid = 0;
  for (int i = 0; i < 2; i++) {
    tdiff = fabs(
      cangle - acos(sqrt(mom * mom + mass[i] * mass[i]) / mom / 1.46907)); // 1.46907 - fused silica
    if (tdiff < diff) {
      diff = tdiff;
      minid = i;
    }
  }
  return pdg[minid];
}

int PrtLutReco::GetEdge(int mcpid, int pixid) {
  // int x(0),y(0), piid(pixid) , nedge(0); //new
  // for (auto hit : fEvent->getHits()) {
  //    int pid = hit.getPixel();
  //    int mid = hit.getMcp();
  // 	double tdif=fabs(hitTime-hit.getLeadTime());
  // 	if(mid!=mcpid || pid==piid || tdif>0.3) continue;
  // 	if(pid==piid-1 && piid%8!=0) y-=1;
  // 	if(pid==piid+1 && piid%8!=7) y+=1;

  // 	if(pid==piid+8 && piid<57) x-=1;
  // 	if(pid==piid-8 && piid>8)  x+=1;
  // }

  int x(0), y(0), piid(pixid), nedge(0); // old
  for (auto hit : fEvent->getHits()) {
    int pid = hit.getPixel();
    int mid = hit.getPmt();

    if (mid != mcpid || pid == piid) continue;
    if (pid == piid - 1 && piid % 8 != 1) x -= 1;
    if (pid == piid + 1 && piid % 8 != 0) x += 1;

    if (pid == piid + 8 && piid < 57) y += 1;
    if (pid == piid - 8 && piid > 8) y -= 1;
  }

  if (x == 0 && y == 0) nedge = 0;
  if (x == -1 && y == 0) nedge = 1;
  if (x == -1 && y == 1) nedge = 2;
  if (x == 0 && y == 1) nedge = 3;
  if (x == 1 && y == 1) nedge = 4;
  if (x == 1 && y == 0) nedge = 5;
  if (x == 1 && y == -1) nedge = 6;
  if (x == 0 && y == -1) nedge = 7;
  if (x == -1 && y == -1) nedge = 8;

  return nedge;
}

int PrtLutReco::getneighbours(int m, int p) {
  for (int i = 0; i < 65; i++)
    if (p == lneighbours[i]) return -1;
  lneighbours[lsize] = p;
  lsize++;
  for (int t = 0; t < 65; t++) {
    if (mcpdata[m][t]) {
      for (int i = 0; i < 65; i++)
        if (t == lneighbours[i]) continue;
      if ((t == p - 1 && p % 8 != 0) || (t == p + 1 && p % 8 != 7) || (t == p + 8 && p < 57) ||
          (t == p - 8 && p > 8))
        getneighbours(m, t);
    }
  }
  return lsize;
}

void PrtLutReco::getclusters() {
  for (int m = 0; m < fnpmt; m++) {
    for (int p = 0; p < 65; p++) {
      if (mcpdata[m][p]) cluster[m][p] = getneighbours(m, p);
      lsize = 0;
      for (int i = 0; i < 65; i++) lneighbours[i] = 0;
    }
  }
}

void PrtLutReco::SearchClusters() {

  for (int j = 0; j < fnpmt; j++) {
    for (int i = 0; i < 65; i++) {
      mcpdata[j][i] = 0;
      cluster[j][i] = 0;
    }
  }
  for (auto hit : fEvent->getHits()) {
    mcpdata[hit.getPmt()][hit.getPixel()] = 1;
  }
  getclusters();
}

void PrtLutReco::drawTheoryLines() {
  gPad->Update();
  TLine *line = new TLine(0, 0, 0, 1000);
  line->SetX1(fAngle[fPk]);
  line->SetX2(fAngle[fPk]);
  line->SetY1(gPad->GetUymin());
  line->SetY2(gPad->GetUymax());
  line->SetLineColor(ft.color(fPk));
  line->Draw();

  TLine *line1 = new TLine(0, 0, 0, 1000);
  line1->SetX1(fAngle[1]);
  line1->SetX2(fAngle[1]);
  line1->SetY1(gPad->GetUymin());
  line1->SetY2(gPad->GetUymax());
  line1->SetLineColor(ft.color(1));
  line1->Draw();
}
