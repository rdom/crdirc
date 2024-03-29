#include "../src/PrtTools.h"

void draw_tracker(TString infile = "../build/hits.root") {

  PrtTools t(infile);

  TH2F *hTracker1 = new TH2F("hTracker", "hTracker", 200, -100, 100, 200, -100, 100);

  TH1F *hDir = new TH1F("hDir", ";;", 200, 20, 20);
  TH1F *hMomentum = new TH1F("hMomentum", ";momentum [GeV/c];entries [#]", 200, 0, 10);

  TVector3 z = TVector3(0,0,-1);
  
  while (t.next()) {
    // get position vectors
    TVector3 t1 = t.event()->getT1Position();
    TVector3 t2 = t.event()->getT2Position();
    TVector3 t3 = t.event()->getT3Position();

    TVector3 dir = t.event()->getMomentum();
    // hDir->Fill(dir.Angle(z) * TMath::RadToDeg());
    hDir->Fill(dir.Theta() * TMath::RadToDeg());
    hMomentum->Fill(dir.Mag());

    // fill 2D histogram
    hTracker1->Fill(t1.X(), t1.Y());
  }

  t.add_canvas("dir");
  hDir->Draw();

  t.add_canvas("momentum");
  hMomentum->Draw();  
  
  t.add_canvas("t1");
  hTracker1->Draw("colz");
    
  t.save_canvas("data/draw_tracker", 0);
}
