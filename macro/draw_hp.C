#if defined(__ACLIC__)
#include "../src/PrtTools.h"
#else
R__LOAD_LIBRARY(../build/libPrt.so)
#endif

void draw_hp(TString infile = "../build/hits.root") {

  PrtTools t(infile);
  double theta = t.run()->getTheta();
  int l = t.run()->getPmtLayout();

  while (t.next() && t.i() < 10000) {
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
  cdigi->SetName(Form("hp_%d_%d", (int)theta, l));
  t.add_canvas(cdigi);
  t.save_canvas("data/drawHP", 0);

  //  t.write_string("digi.csv", t.pix_digi("m,p,v\n"));  
}
