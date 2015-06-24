
// from STL
#include <vector>
#include <string>
#include <utility>
#include <cmath>
#include <map>
#include <sstream>

// from boost
#include <boost/tuple/tuple.hpp>
#ifdef __GNUG__
#define BOOST_NO_CXX11_SCOPED_ENUMS
#endif
#include <boost/filesystem.hpp>
#include <boost/assign/list_inserter.hpp> // for 'insert()'
using namespace boost::assign; // bring 'operator+=()' into scope

// from ROOT
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TString.h"
#include "TROOT.h"
#include "TH1.h"
#include "TMath.h"
#include "TLeaf.h"
#include "TStopwatch.h"
#include "TGraphErrors.h"

// from RooFit
#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooCategory.h"
#include "RooPlot.h"
#include "RooDataSet.h"
#include "RooKeysPdf.h"
#include "RooAbsReal.h"
#include "RooDataHist.h"
#include "RooBinning.h"

// from DooCore
#include "doocore/io/EasyTuple.h"
#include "doocore/io/MsgStream.h"
#include "doocore/lutils/lutils.h"

// from DooFit
#include <doofit/plotting/Plot/Plot.h>
#include <doofit/plotting/Plot/PlotConfig.h>

using namespace ROOT;
using namespace RooFit;

using namespace doocore::io;
using namespace doofit::plotting;

int main(int argc, char* argv[]) {
  PlotConfig pconfig("plot_config");
  pconfig.InitializeOptions(argc, argv);
  
  if (argc<6) {
    sinfo << "Usage: " << argv[0] << " mc_tuple_file tree_name data_tuple_file tree_name runname(directory)" << endmsg;
    return 1;
  } else {
    doocore::lutils::setStyle();
  TCanvas c("c","c",800,600);
  
  std::string prefix(argv[5]);
  if(prefix == "")
    prefix = ".";
  std::string path_name = prefix + "/VarDistributionPlots";

  doocore::lutils::printPlotOpenStack(&c, "HighSeparation",   path_name);
  doocore::lutils::printPlotOpenStack(&c, "MediumSeparation", path_name);
  doocore::lutils::printPlotOpenStack(&c, "LowSeparation",    path_name);
  doocore::lutils::printPlotOpenStack(&c, "NoSeparation",     path_name);
  
  std::string cutstring = "B0_FitPVConst_chi2_flat<400 && B0_FitPVConst_status_flat==0";

  std::vector<std::string> dims = {"B0_TAU",
                                   "B0_TAUERR",
                                   "B0_TAUCHI2",
                                   "B0_FD_OWNPV",
                                   "B0_FDCHI2_OWNPV",
                                   "B0_MM",
                                   "B0_M",
                                   "B0_MMERR",
                                   "B0_PT",
                                   "B0_ENDVERTEX_CHI2",
                                   "J_psi_1S_TAU",
                                   "J_psi_1S_TAUERR",
                                   "J_psi_1S_TAUCHI2",
                                   "J_psi_1S_FD_ORIVX",
                                   "J_psi_1S_FDCHI2_ORIVX",
                                   "J_psi_1S_MM",
                                   "J_psi_1S_M",
                                   "J_psi_1S_MMERR",
                                   "J_psi_1S_PT",
                                   "J_psi_1S_ENDVERTEX_CHI2",
                                   "J_psi_1S_MINIP",
                                   "J_psi_1S_MINIPCHI2",
                                   "B0_FitPVConst_status_flat",
                                   "B0_FitPVConst_chi2_flat",
                                   "B0_FitPVConst_M_flat",
                                   "B0_FitPVConst_tau_flat",
                                   "B0_FitPVConst_chi2_flat",
                                   "B0_FitPVConst_KS0_P0_IP_flat",
                                   "B0_FitPVConst_KS0_P1_IP_flat",
                                   "B0_FitPVConst_KS0_P0_IPCHI2_flat",
                                   "B0_FitPVConst_KS0_P1_IPCHI2_flat",
                                   "B0_FitPVConst_J_psi_1S_P0_IP_flat",
                                   "B0_FitPVConst_J_psi_1S_P1_IP_flat",
                                   "B0_FitPVConst_J_psi_1S_P0_IPCHI2_flat",
                                   "B0_FitPVConst_J_psi_1S_P1_IPCHI2_flat",
                                   "B0_FitPVConst_M_flat",
                                   "B0_FitDaughtersPVConst_tau_flat",
                                   "B0_FitDaughtersPVConst_chi2_flat",
                                   "B0_IPCHI2_OWNPV",
                                   "eplus_PIDe",
                                   "eminus_PIDe",
                                   "eplus_ProbNNe",
                                   "eminus_ProbNNe",
                                   "eplus_PT",
                                   "eminus_PT",
                                   "eplus_MINIP",
                                   "eplus_MINIPCHI2",
                                   "eminus_MINIP",
                                   "eminus_MINIPCHI2",
                                   "eplus_TRACK_CHI2",
                                   "eplus_TRACK_CHI2NDOF",
                                   "eplus_TRACK_GhostProb",
                                   "eminus_TRACK_CHI2",
                                   "eminus_TRACK_CHI2NDOF",
                                   "eminus_TRACK_GhostProb",
                                   "KS0_TAU",
                                   "KS0_TAUERR",
                                   "KS0_TAUCHI2",
                                   "KS0_FD_ORIVX",
                                   "KS0_FDCHI2_ORIVX",
                                   "KS0_MM",
                                   "KS0_M",
                                   "KS0_PT",
                                   "KS0_ENDVERTEX_CHI2",
                                   "piplus_PIDK",
                                   "piminus_PIDK",
                                   "piplus_ProbNNpi",
                                   "piminus_ProbNNpi",
                                   "piplus_PT",
                                   "piminus_PT",
                                   "piplus_MINIP",
                                   "piplus_MINIPCHI2",
                                   "piminus_MINIP",
                                   "piminus_MINIPCHI2",
                                   "piplus_TRACK_CHI2",
                                   "piplus_TRACK_CHI2NDOF",
                                   "piplus_TRACK_GhostProb",
                                   "piminus_TRACK_CHI2",
                                   "piminus_TRACK_CHI2NDOF",
                                   "piminus_TRACK_GhostProb",
                                   "B0_Hlt2Topo2BodyBBDTDecision_TOS",
                                   "B0_Hlt2TopoE2BodyBBDTDecision_TOS",
                                   "B0_Hlt2Topo3BodyBBDTDecision_TOS",
                                   "B0_Hlt2TopoE3BodyBBDTDecision_TOS",
                                   "B0_Hlt2Topo4BodyBBDTDecision_TOS",
                                   "B0_Hlt2TopoE4BodyBBDTDecision_TOS"};

  RooArgSet args;

  for(auto dim_name : dims){
    RooRealVar* dim = new RooRealVar(dim_name.c_str(), dim_name.c_str(), 0);
    args.addOwned(*dim);
  }

  sinfo << "Importing Tuple 1" << endmsg;
  EasyTuple etuple_sig(argv[1], argv[2], args);
  sinfo << "Importing Tuple 2" << endmsg;
  EasyTuple etuple_bkg(argv[3], argv[4], args);

  sinfo << "Converting Tuple 1" << endmsg;
  RooDataSet *data_sig = &etuple_sig.ConvertToDataSet(Cut(cutstring.c_str()));
  sinfo << "Converting Tuple 2" << endmsg;
  RooDataSet *data_bkg = &etuple_bkg.ConvertToDataSet(Cut(cutstring.c_str()));

  for(auto dim_name : dims){
    sinfo << "Processing Observable: " << dim_name << endmsg;
    RooRealVar dim(dim_name.c_str(), dim_name.c_str(), 0);
          
    data_sig->Print();
    data_bkg->Print();
             
    std::pair<double, double> minmax_sig = doocore::lutils::MedianLimitsForTuple(*data_sig, dim.GetName());
    std::pair<double, double> minmax_bkg = doocore::lutils::MedianLimitsForTuple(*data_bkg, dim.GetName());
        
    double min, max;
    if (minmax_sig.first  < minmax_bkg.first) min = minmax_sig.first; else min = minmax_bkg.first;
    if (minmax_sig.second > minmax_bkg.second) max = minmax_sig.second; else max = minmax_bkg.second;
        
    dim.setMin(min);
    dim.setMax(max);
        
    TH1* data_sig_th1 = data_sig->createHistogram("data_sig_th1", dim);
    TH1* data_bkg_th1 = data_bkg->createHistogram("data_bkg_th1", dim);
        
    data_sig_th1->Scale(1.0/data_sig_th1->Integral());
    data_sig_th1->SetFillColor(kBlue);
    data_sig_th1->SetFillStyle(3004);
    data_bkg_th1->Scale(1.0/data_bkg_th1->Integral());
    data_bkg_th1->SetFillColor(kRed);
    data_bkg_th1->SetFillStyle(3003);
        
    if (data_bkg_th1->GetMaximum() > data_sig_th1->GetMaximum()) {
      data_sig_th1->SetMaximum(data_bkg_th1->GetMaximum()*1.15);
    } else {
      data_sig_th1->SetMaximum(data_sig_th1->GetMaximum()*1.15);
    }
        
    data_sig_th1->Draw("HIST");
    data_bkg_th1->Draw("HIST SAME");
        
    double separation = 0.0;
    for (int i=0; i<data_sig_th1->GetNbinsX(); ++i) {
      separation += (data_sig_th1->GetBinContent(i)>0&&data_bkg_th1->GetBinContent(i)>0) ? TMath::Power(data_sig_th1->GetBinContent(i)-data_bkg_th1->GetBinContent(i),2)/(data_sig_th1->GetBinContent(i)+data_bkg_th1->GetBinContent(i)) : 0.0;
    }
    separation /= 2.0;
    
    TString label_s = Form("p_{#chi^{2}} = %.3f  p_{KS} = %.3f  s = %.3f", data_sig_th1->Chi2Test(data_bkg_th1, "WW"), data_sig_th1->KolmogorovTest(data_bkg_th1), separation);
    TLatex  label(0.18, 0.87, label_s);
    label.SetNDC();
    label.Draw();

    std::string path_name_specific = "";
    std::string plot_name = dim_name;

    if (separation > 0.2) {
      path_name_specific = prefix + path_name + "/HighSeparation/";
      plot_name = "HighSeparation";
    } else if (separation <= 0.2 && separation > 0.1) {
      path_name_specific = prefix + path_name + "/MediumSeparation/";
      plot_name = "MediumSeparation";
    } else if (separation <= 0.1 && separation > 0.01) {
      path_name_specific = prefix + path_name + "/LowSeparation/";
      plot_name = "LowSeparation";
    } else if (separation <= 0.01) {
      path_name_specific = prefix + path_name + "/NoSeparation/";
      plot_name = "NoSeparation";
    }

    doocore::lutils::printPlot(&c, dim_name, path_name_specific.c_str(), true);
    doocore::lutils::printPlot(&c, plot_name, path_name.c_str(), true);
    
    delete data_sig_th1;
    delete data_bkg_th1;
  }
  
  doocore::lutils::printPlotCloseStack(&c, "HighSeparation", path_name);
  doocore::lutils::printPlotCloseStack(&c, "MediumSeparation", path_name);
  doocore::lutils::printPlotCloseStack(&c, "LowSeparation", path_name);
  doocore::lutils::printPlotCloseStack(&c, "NoSeparation", path_name);
  }
}
