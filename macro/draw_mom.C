#if defined(__ACLIC__)
#include "../src/PrtTools.h"
#else
R__LOAD_LIBRARY(../build/libPrt.so)
#endif

void draw_mom(TString infile = "../build/hits.root") {

  PrtTools t(infile);
  TVector3 mom;

  TH1F *hmom = new TH1F("mom",";momentum [GeV/c];entries [#]",100,0,10);
  TH1F *htime = new TH1F("t",";time [h];events [#]",100,0,30);
  TH1F *hy = new TH1F("t",";detected photons [#];entries [#]",100,0,100);

  
  while (t.next() && t.i() < 10E10) {

    bool good = t.event()->getT1Position().Z() < -10 && t.event()->getT2Position().Z() < -10;
    // &&
    //             t.event()->getT3Position().Z() < -10 ;

    if (!good) continue;

    mom = t.event()->getMomentum();
    hmom->Fill(mom.Mag()/1000.);

    if (mom.Mag() / 1000. > 2) htime->Fill(t.event()->getTime() / 3600.);
    
    if (mom.Mag() / 1000. > 2) hy->Fill(t.event()->getHits().size());
    
    for (auto hit : t.event()->getHits()) {
      int ch = hit.getChannel();
      int pmt = hit.getPmt();
      int pix = hit.getPixel();
      double time = hit.getLeadTime();

      //      if (t.pid() == 2)
	t.fill_digi(pmt, pix);
    }
  }

  auto cdigi = t.draw_digi();
  t.add_canvas(cdigi);


  t.add_canvas("mom");
  gPad->SetLogy();
  hmom->Draw();

  t.add_canvas("time");
  htime->Draw();
  
  t.add_canvas("yield");
  hy->Draw();

  
  t.save_canvas("data/draw_mom", 0);
 
}
