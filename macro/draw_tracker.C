#include "../src/PrtTools.h"

void draw_tracker(TString infile = "../build/hits.root") {

  PrtTools t(infile);

  TH2F *hTracker1 = new TH2F("hTracker", "hTracker", 200, -100, 100, 200, -100, 100);

  while (t.next()) {
    // get position vectors
    TVector3 t1 = t.event()->getT1Position();
    TVector3 t2 = t.event()->getT2Position();
    TVector3 t3 = t.event()->getT3Position();

    // fill 2D histogram
    hTracker1->Fill(t1.X(), t1.Y());
  }

  t.add_canvas("t1");
  hTracker1->Draw("colz");
  t.save_canvas("data/draw_tracker", 0);
}
