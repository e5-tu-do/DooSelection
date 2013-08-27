
// from STL
#include <vector>
#include <string>
#include <utility>
#include <cmath>
#include <map>
#include <sstream>

// from boost
#include <boost/tuple/tuple.hpp>
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

// dimension, dimension description, weight var sig, weight var bkg, cut, name for output, secondary observables, produce keys pdf, log plot
typedef boost::tuple<std::string, std::string, std::string, std::string, std::string, std::string, RooArgSet, bool> DimensionTuple;

// tuple file, tree in file, plot name
typedef boost::tuple<std::string, std::string, std::string> TreeTuple;

using namespace doocore::io;
using namespace doofit::plotting;

template <class Iter, class Incr>
Iter safe_advance(Iter& curr, const Iter& end, Incr n)
{
  Iter next = curr;
  size_t remaining(std::distance(next, end));
  if (remaining < n)
  {
    n = remaining;
  }
  std::advance(next, n);
  return next;
}

RooBinning GetQuantileBinning(RooDataSet* data, TString nameofquantileobservable, int nbins){
  std::vector<double> data_vector;
  for (unsigned int i = 0; i < data->numEntries(); i++) {
    data_vector.push_back(data->get(i)->getRealValue(nameofquantileobservable));
  }
  sort(data_vector.begin(), data_vector.end());
  std::vector<double> probability_vector;
  for (unsigned int i = 1; i < nbins; i++) {
    probability_vector.push_back(1.*i/nbins);
  }
  std::vector<double> xbins(nbins+1,0);
  RooRealVar* quantilerealvar = dynamic_cast<RooRealVar*>(data->get()->find(nameofquantileobservable));
  xbins.front() = quantilerealvar->getMin();
  xbins.back() = quantilerealvar->getMax();
  
  TMath::Quantiles(data_vector.size(), nbins-1, &data_vector[0], &xbins[1], &probability_vector[0]);
  double* xbinarray = new double[xbins.size()];
  for (int i = 0; i < xbins.size(); i++) {
    xbinarray[i] = xbins[i];
  }
  RooBinning Binning(nbins,xbinarray,"Binning_"+nameofquantileobservable);
  return Binning;
}

bool CompareVectorsByFirstEntry(const std::vector<double>& i, const std::vector<double>& j) {
  return i[0] < j[0];
}

template<typename DataType>
std::vector<std::vector<DataType> >* TransposeVector(const std::vector<std::vector<DataType> >& vector_in) {
  std::vector<std::vector<DataType> >* vector_out = new std::vector<std::vector<DataType> >(vector_in[0].size(), std::vector<DataType>(vector_in.size()));
  std::vector<std::vector<DataType> >& vector_out_ref = *vector_out;
  
  for (typename std::vector<DataType>::size_type i = 0; i < vector_in[0].size(); i++) {
    for (typename std::vector<DataType>::size_type j = 0; j < vector_in.size(); j++) {
      vector_out_ref[i][j] = vector_in[j][i];
    }
  }
  return vector_out;
}

std::vector<std::vector<double> > AverageSortedDataSet(const std::vector<std::vector<double> >& sorted_dataset, const RooBinning& binning, std::vector<std::vector<double> >* errors_ptr) {
  std::vector<std::vector<double> > averages;
  std::vector<std::vector<double> >& errors = *errors_ptr;
  int index_set = 0;
  int num_vars = sorted_dataset.size();
  double ref_val = 0.0;
  int num_events = sorted_dataset[0].size();
  
  for (int index_vars=0; index_vars<num_vars; ++index_vars) {
    averages.push_back(std::vector<double>(binning.numBins(),0.0));
    errors.push_back(std::vector<double>(binning.numBins(),0.0));
  }
  
  for (int index_binning=0; index_binning<binning.numBins(); ++index_binning) {
    int index_binning_start = index_set;
    int index_binning_end = index_set;
    
    while (index_set < num_events && sorted_dataset[0][index_set] <= binning.binHigh(index_binning)) {
      for (int index_vars=0; index_vars<num_vars; ++index_vars) {
        if (index_vars != 1) {
          averages[index_vars][index_binning] += sorted_dataset[1][index_set]*sorted_dataset[index_vars][index_set];
        } else {
          averages[index_vars][index_binning] += sorted_dataset[index_vars][index_set];
        }
      }
      ++index_set;
    }
    index_binning_end = index_set;

    for (int index_vars=0; index_vars<num_vars; ++index_vars) {
      if (index_vars != 1) {
        averages[index_vars][index_binning] /= averages[1][index_binning];
        
//        sdebug << "[" << index_vars << "]: " << averages.back()[index_vars] << " - " << TMath::Mean(sorted_dataset[index_vars].begin()+index_binning_start, sorted_dataset[index_vars].begin()+index_binning_end, sorted_dataset[1].begin()+index_binning_start)<< endmsg;
      }
    }
    
    for (int index_err = index_binning_start; index_err<index_binning_end; ++index_err) {
      for (int index_vars=0; index_vars<num_vars; ++index_vars) {
        if (index_vars != 1) {
//          sdebug << "[index_err] = " << index_err << " => " << sorted_dataset[index_err][1] << "*" << TMath::Power(sorted_dataset[index_err][index_vars],2) << "-" << TMath::Power(averages.back()[index_vars],2) << endmsg;
          errors[index_vars][index_binning] += sorted_dataset[1][index_err]*TMath::Power(sorted_dataset[index_vars][index_err]-averages[index_vars][index_binning],2);
        }
      }
    }
    for (int index_vars=0; index_vars<num_vars; ++index_vars) {
      if (index_vars != 1) {
        errors[index_vars][index_binning] /= TMath::Power(averages[1][index_binning],2);
        errors[index_vars][index_binning] = TMath::Sqrt(errors[index_vars][index_binning]);
      }
    }
    
    
//    sdebug << "Averages: " << averages.back() << endmsg;
  }
  
  return averages;
}

std::map<std::string, int> SortDataSetByVar(std::string sort_var, const RooDataSet& data, std::vector<std::vector<double> >** sorted_dataset_ptr) {
  RooRealVar& var_sort = *dynamic_cast<RooRealVar*>(data.get()->find(sort_var.c_str()));
  int num_vars = data.get()->getSize();
  int num_entries = data.numEntries();
  std::vector<std::vector<double> > sorted_dataset;
  std::map<std::string, int> variable_positions;
  const RooArgSet* args = data.get();
  int j;
  
  TIterator* it;
  RooRealVar* var;
  
  j = 0;
  it = args->createIterator();
  while ((var = dynamic_cast<RooRealVar*>(it->Next()))) {
    variable_positions[var->GetName()] = j+2;
    ++j;
  }
  
  for (int i=0; i<num_entries; ++i) {
    sorted_dataset.push_back(std::vector<double>(num_vars+2,0.0));
    
    args = data.get(i);
    
    sorted_dataset.back()[0] = var_sort.getVal();
    sorted_dataset.back()[1] = data.weight();
    
    j = 2;
    it = args->createIterator();
    while ((var = dynamic_cast<RooRealVar*>(it->Next()))) {
      sorted_dataset.back()[j] = var->getVal();
      
      ++j;
    }
  }
  
  std::sort(sorted_dataset.begin(), sorted_dataset.end(), CompareVectorsByFirstEntry);

//  for (std::vector<std::vector<double> >::const_iterator it = sorted_dataset.begin(), end = sorted_dataset.end();
//       it != end; ++it) {
//    sdebug << *it << endmsg;
//  }
  
  *sorted_dataset_ptr = TransposeVector(sorted_dataset);
    
  return variable_positions;
}

void PlotSweightedTuple(const std::vector<TreeTuple>& tuples, const std::vector<DimensionTuple>& dimensions, const PlotConfig& pconfig) {  
  for (std::vector<TreeTuple>::const_iterator it_tuples=tuples.begin(); it_tuples!=tuples.end(); ++it_tuples) {
    sinfo << "Plotting for tuple: " << (*it_tuples).get<0>() << ":" << (*it_tuples).get<1>() << " into prefix " << (*it_tuples).get<2>() << endmsg;
    
    doocore::lutils::setStyle();
    TCanvas c("c","c",800,600);
    
    std::string path_name;
    path_name = "VarDistributionPlots/" + (*it_tuples).get<2>();
    doocore::lutils::printPlotOpenStack(&c, "HighSeparation", path_name);
    doocore::lutils::printPlotOpenStack(&c, "MediumSeparation", path_name);
    doocore::lutils::printPlotOpenStack(&c, "LowSeparation", path_name);
    doocore::lutils::printPlotOpenStack(&c, "NoSeparation", path_name);
    
    int packet_size = 20;
    
    std::vector<std::vector<DimensionTuple> > dimensions_packeted;
    for (std::vector<DimensionTuple>::const_iterator it_dims=dimensions.begin(); it_dims!=dimensions.end(); it_dims=safe_advance(it_dims, dimensions.end(), packet_size)) {
      std::vector<DimensionTuple> dimensions_packet;
      
//      sdebug << "Plotting these variables as group: ";
      for (std::vector<DimensionTuple>::const_iterator it_int=it_dims, it_int_end=safe_advance(it_dims, dimensions.end(), packet_size); it_int!=it_int_end; ++it_int) {
        dimensions_packet.push_back(*it_int);
//        sdebug << (*it_int).get<0>() << " ";
      }
//      sdebug << endmsg;

      dimensions_packeted.push_back(dimensions_packet);
    }
    
    sdebug << endmsg << endmsg;
    
    TStopwatch sw;
    int num_packets_processed = 0;
    sw.Start();
    
    std::multimap<double, std::string> map_separation_to_variable;
    
    for (std::vector<std::vector<DimensionTuple> >::const_iterator it_packets=dimensions_packeted.begin(); it_packets!=dimensions_packeted.end(); ++it_packets) {
      const std::vector<DimensionTuple>& dimensions_packet = *it_packets;
      
      RooArgSet args;
      
      for (std::vector<DimensionTuple>::const_iterator it_dims=dimensions_packet.begin(); it_dims!=dimensions_packet.end(); ++it_dims) {
        const std::string& dim_name = (*it_dims).get<0>();
        const std::string& dim_desc = (*it_dims).get<1>();
        RooRealVar* dim = new RooRealVar(dim_name.c_str(), dim_desc.c_str(), 0);
        args.addOwned(*dim);
      }
      
      const DimensionTuple& dim_first = dimensions_packet.front();
      
      const std::string& sw_sig   = dim_first.get<2>();
      const std::string& sw_bkg   = dim_first.get<3>();
      
      RooArgSet args_additional = dim_first.get<6>();
      args.addClone(args_additional);

      EasyTuple etuple_sig((*it_tuples).get<0>(), (*it_tuples).get<1>(), args);
      RooDataSet *data_sig = &etuple_sig.ConvertToDataSet(/*Cut(cut.c_str()),*/ WeightVar(sw_sig.c_str()));
      
      EasyTuple etuple_bkg((*it_tuples).get<0>(), (*it_tuples).get<1>(), args);
      RooDataSet *data_bkg = &etuple_bkg.ConvertToDataSet(/*Cut(cut.c_str()),*/ WeightVar(sw_bkg.c_str()));
      
      data_sig->Print();
      data_bkg->Print();
      
      RooArgSet args_not_in_dataset_sig(args);
      args_not_in_dataset_sig.remove(*data_sig->get(), false, true);
      args_not_in_dataset_sig.remove(*args.find(sw_sig.c_str()), false, true);
      RooArgSet args_not_in_dataset_bkg(args);
      args_not_in_dataset_bkg.remove(*data_bkg->get(), false, true);
      args_not_in_dataset_bkg.remove(*args.find(sw_bkg.c_str()), false, true);
      
      data_sig->addColumns(args_not_in_dataset_sig);
      data_bkg->addColumns(args_not_in_dataset_bkg);
      
      TStopwatch sw_sort;
      sw_sort.Start();
      RooBinning binning = GetQuantileBinning(data_sig, "obsTime", 10);
      std::vector<std::vector<double> >* sorted_dataset_ptr = NULL;
      std::map<std::string, int> variable_positions = SortDataSetByVar("obsTime", *data_sig, &sorted_dataset_ptr);
      std::vector<std::vector<double> >& sorted_dataset = *sorted_dataset_ptr;
      
      std::vector<std::vector<double> > errors;
      std::vector<std::vector<double> > averages = AverageSortedDataSet(sorted_dataset, binning, &errors);
      sdebug << "Sorting took " << sw_sort << endmsg;
            
      for (std::vector<DimensionTuple>::const_iterator it_dims=dimensions_packet.begin(); it_dims!=dimensions_packet.end(); ++it_dims) {
        const std::string& dim_name = (*it_dims).get<0>();
        const std::string& dim_desc = (*it_dims).get<1>();
        const std::string& sw_sig   = (*it_dims).get<2>();
        const std::string& sw_bkg   = (*it_dims).get<3>();
        const std::string& cut      = (*it_dims).get<4>();
        const std::string& out_name = (*it_dims).get<5>();
        RooArgSet          args     = (*it_dims).get<6>();
        bool               log_plot = (*it_dims).get<7>();
        
        RooRealVar dim(dim_name.c_str(), dim_desc.c_str(), 0);
        
        
        RooDataSet *data_sig_single_var = NULL;
        RooDataSet *data_bkg_single_var = NULL;
        
        if (cut.length()>0) {
          data_sig_single_var = dynamic_cast<RooDataSet*>(data_sig->reduce(cut.c_str()));
          data_bkg_single_var = dynamic_cast<RooDataSet*>(data_bkg->reduce(cut.c_str()));
        } else {
          data_sig_single_var = data_sig;
          data_bkg_single_var = data_bkg;
        }
        
        sdebug << "Correlation for " << dim.GetName() << endmsg;
        for (int i=0; i<binning.numBins(); ++i) {
//          std::stringstream sstr;
//          
//          sstr << "obsTime>" << binning.binLow(i) << "&&obsTime<" << binning.binHigh(i);
//          RooDataSet* data_red = dynamic_cast<RooDataSet*>(data_sig->reduce(sstr.str().c_str()));
//          sdebug << "binned avg: " << sstr.str() << " => " << data_red->mean(dim) << " +/- " << data_red->sigma(dim) << ", <t> = " << averages[i][0] << ", <x> = " << averages[i][variable_positions[dim_name]] << endmsg;
          
          sdebug << "<t> = " << averages[0][i] << " +/- " << errors[0][i] << ", <x> = " << averages[variable_positions[dim_name]][i] << " +/- " << errors[variable_positions[dim_name]][i] << endmsg;
        }
        
        std::string path_name_specific_corr = "VarDistributionPlots/" + (*it_tuples).get<2>() + "/Correlation/";
//        TCanvas c1("c1", "c1", 800, 600);
        TGraphErrors tge(binning.numBins(), &averages[0][0], &averages[variable_positions[dim_name]][0], &errors[0][0], &errors[variable_positions[dim_name]][0]);
        tge.Draw("AP");
        doocore::lutils::printPlot(&c, out_name, path_name_specific_corr.c_str(), true);
        
        std::pair<double, double> minmax_sig = doocore::lutils::MedianLimitsForTuple(*data_sig_single_var, dim.GetName());
        std::pair<double, double> minmax_bkg = doocore::lutils::MedianLimitsForTuple(*data_bkg_single_var, dim.GetName());
        
        double min, max;
        if (minmax_sig.first  < minmax_bkg.first) min = minmax_sig.first; else min = minmax_bkg.first;
        if (minmax_sig.second > minmax_bkg.second) max = minmax_sig.second; else max = minmax_bkg.second;
        
        dim.setMin(min);
        dim.setMax(max);
        
        TH1* data_sig_th1 = data_sig_single_var->createHistogram("data_sig_th1", dim);
        TH1* data_bkg_th1 = data_bkg_single_var->createHistogram("data_bkg_th1", dim);
        
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
        //data_sig_th1->SetMinimum(0.0);
        
        data_sig_th1->Draw("HIST");
        data_bkg_th1->Draw("HIST SAME");
        
        double separation = 0.0;
        for (int i=0; i<data_sig_th1->GetNbinsX(); ++i) {
          separation += (data_sig_th1->GetBinContent(i)>0&&data_bkg_th1->GetBinContent(i)>0) ? TMath::Power(data_sig_th1->GetBinContent(i)-data_bkg_th1->GetBinContent(i),2)/(data_sig_th1->GetBinContent(i)+data_bkg_th1->GetBinContent(i)) : 0.0;
        }
        separation /= 2.0;
        
        if (separation >= 0.005) {
          insert(map_separation_to_variable)(separation, dim_name);
          
          TString label_s = Form("p_{#chi^{2}} = %.3f  p_{KS} = %.3f  s = %.3f", data_sig_th1->Chi2Test(data_bkg_th1, "WW"), data_sig_th1->KolmogorovTest(data_bkg_th1), separation);
          TLatex  label(0.18, 0.87, label_s);
          label.SetNDC();
          label.Draw();
          
          //      sdebug << data_sig_th1->KolmogorovTest(data_bkg_th1) << endmsg;
          //      sdebug << data_sig_th1->Chi2Test(data_bkg_th1, "WW") << endmsg;
          
          std::string path_name_specific;
          std::string plot_name;
          
          if (separation > 0.2) {
            path_name_specific = "VarDistributionPlots/" + (*it_tuples).get<2>() + "/HighSeparation/";
            plot_name = "HighSeparation";
          } else if (separation <= 0.2 && separation > 0.1) {
            path_name_specific = "VarDistributionPlots/" + (*it_tuples).get<2>() + "/MediumSeparation/";
            plot_name = "MediumSeparation";
          } else if (separation <= 0.1 && separation > 0.01) {
            path_name_specific = "VarDistributionPlots/" + (*it_tuples).get<2>() + "/LowSeparation/";
            plot_name = "LowSeparation";
          } else if (separation <= 0.01) {
            path_name_specific = "VarDistributionPlots/" + (*it_tuples).get<2>() + "/NoSeparation/";
            plot_name = "NoSeparation";
          }
          doocore::lutils::printPlot(&c, out_name, path_name_specific.c_str(), true);
          doocore::lutils::printPlot(&c, plot_name, path_name.c_str(), true);
        }
        //c.SaveAs(plot_name.c_str());
        
        delete data_sig_th1;
        delete data_bkg_th1;
        
        if (data_sig_single_var != data_sig) delete data_bkg_single_var;
        if (data_bkg_single_var != data_bkg) delete data_sig_single_var;
      }
      
      ++num_packets_processed;
      sinfo << "Time per variable " << sw.RealTime()/static_cast<double>(packet_size*num_packets_processed) << " s." << endmsg;
      sw.Start(false);
    }
    
    MsgStream sseplist(kTextGreen, path_name+"/SeparationPower.txt");
    
    sseplist << endmsg << "Printing variables sorted by separation power: " << endmsg;
    for (std::multimap<double, std::string>::const_iterator it = map_separation_to_variable.begin(), end = map_separation_to_variable.end();
         it != end; ++it) {
      sseplist << " v: " << it->second << " -> " << it->first << endmsg;
    }
    
    doocore::lutils::printPlotCloseStack(&c, "HighSeparation", path_name);
    doocore::lutils::printPlotCloseStack(&c, "MediumSeparation", path_name);
    doocore::lutils::printPlotCloseStack(&c, "LowSeparation", path_name);
    doocore::lutils::printPlotCloseStack(&c, "NoSeparation", path_name);
  }
}

int Bd2JpsiKSSweights(const PlotConfig& pconfig, int argc, char* argv[]) {
  std::vector<TreeTuple> tuples;
  
  //  tuples.push_back(TreeTuple("/fhgfs/groups/e5/lhcb/NTuples/Bs2JpsiKS/20121106_BDTPrestudies/Bd2JpsiKS_data_Stripping17_DVv29r3p1_FTr134334_v6_20120219_fkruse_combined_fkruse_tupleA_LLMassWin_sweighted.root", "Bd2JpsiKS", "Bd2JpsiKSLL_v1"));
  //  tuples.push_back(TreeTuple("/fhgfs/groups/e5/lhcb/NTuples/Bs2JpsiKS/20121106_BDTPrestudies/Bd2JpsiKS_data_Stripping17_DVv29r3p1_FTr134334_v6_20120219_fkruse_combined_fkruse_tupleA_DDMassWin_sweighted.root", "Bd2JpsiKS", "Bd2JpsiKSDD_v1"));
  
  //  tuples.push_back(TreeTuple("/fhgfs/groups/e5/lhcb/NTuples/Bs2JpsiKS/20121106_BDTPrestudies/Bd2JpsiKS_data_Stripping20_DVv33r1_FTfromDV_v3_20130107_fkruse_combined_tupleA_DDMassWin_sweighted_LeftSides_NoPresel.root", "Bd2JpsiKS", "Bd2JpsiKSDD_Stripping20_DVv33r1"));
  //  tuples.push_back(TreeTuple("/fhgfs/groups/e5/lhcb/NTuples/Bs2JpsiKS/20121106_BDTPrestudies/Bd2JpsiKS_data_Stripping20_DVv33r1_FTfromDV_v3_20130107_fkruse_combined_tupleA_LLMassWin_sweighted_LeftSides_NoPresel.root", "Bd2JpsiKS", "Bd2JpsiKSLL_Stripping20_DVv33r1"));
  
//  tuples.push_back(TreeTuple("/fhgfs/groups/e5/lhcb/NTuples/Bs2JpsiKS/20121106_BDTPrestudies/Bd2JpsiKS_data_Stripping20_DVv33r1_FTfromDV_v3_20130107_fkruse_combined_tupleA_LLMassWin_sweighted_AllRangeBsExcluded_WithPresel.root", "Bd2JpsiKS", "Bd2JpsiKSDD_Stripping20_DVv33r1_AllRangeBsExcluded_WithPresel"));
//  tuples.push_back(TreeTuple("/fhgfs/groups/e5/lhcb/NTuples/Bs2JpsiKS/20121106_BDTPrestudies/Bd2JpsiKS_data_Stripping20_DVv33r1_FTfromDV_v3_20130107_fkruse_combined_tupleA_DDMassWin_sweighted_AllRangeBsExcluded_WithPresel.root", "Bd2JpsiKS", "Bd2JpsiKSLL_Stripping20_DVv33r1_AllRangeBsExcluded_WithPresel"));
  
//  tuples.push_back(TreeTuple("/fhgfs/groups/e5/lhcb/NTuples/Bs2JpsiKS/20130617_ScytheRun/B2JpsiKSDetached_data_Stripping20r0_DVv33r4_FTfromDV_v1_20130531_rniet_combined_tupleA_preselected_triggered_ll_sweighted.root", "Bd2JpsiKS", "Bd2JpsiKSLL_Stripping20r0"));
//  tuples.push_back(TreeTuple("/fhgfs/groups/e5/lhcb/NTuples/Bs2JpsiKS/20130617_ScytheRun/B2JpsiKSDetached_data_Stripping20r0_DVv33r4_FTfromDV_v1_20130531_rniet_combined_tupleA_preselected_triggered_dd_sweighted.root", "Bd2JpsiKS", "Bd2JpsiKSDD_Stripping20r0"));
  tuples.push_back(TreeTuple(argv[1], argv[2], argv[3]));
//  tuples.push_back(TreeTuple("/fhgfs/groups/e5/lhcb/NTuples/Bs2JpsiKS/20130617_ScytheRun/B2JpsiKSDetached_data_Stripping20r0_DVv33r4_FTfromDV_v1_20130531_rniet_combined_tupleA_flattened_preselected_triggered_bestpv_dd_sweighted.root", "Bd2JpsiKS", "Bd2JpsiKSDDFlat_Stripping20r0"));
//  tuples.push_back(TreeTuple("/fhgfs/groups/e5/lhcb/NTuples/Bs2JpsiKS/20130617_ScytheRun/B2JpsiKSDetached_data_Stripping20r0_DVv33r4_FTfromDV_v1_20130531_rniet_combined_tupleA_flattened_preselected_triggered_bestpv_dd_sweighted.root", "Bd2JpsiKS", "SpeedupTest"));

//  tuples.push_back(TreeTuple("/fhgfs/groups/e5/lhcb/NTuples/Bs2JpsiKS/20130206_MCFits/B2JpsiKS_MC11a_Bs2JpsiKS_Stripping17b_DVv33r1_FTfromDV_v2_20130117_fkruse_combined_tupleA_DD_SweightsForCBShapes.root", "Bd2JpsiKS", "Bd2JpsiKSDD_MC11_CBShapesSeparated"));
//  tuples.push_back(TreeTuple("/fhgfs/groups/e5/lhcb/NTuples/Bs2JpsiKS/20130206_MCFits/B2JpsiKS_MC11a_Bs2JpsiKS_Stripping17b_DVv33r1_FTfromDV_v2_20130117_fkruse_combined_tupleA_LL_SweightsForCBShapes.root", "Bd2JpsiKS", "Bd2JpsiKSLL_MC11_CBShapesSeparated_5390_to_5500"));
//  tuples.push_back(TreeTuple("/fhgfs/groups/e5/lhcb/NTuples/Bs2JpsiKS/20130206_MCFits/B2JpsiKS_MC11a_Bs2JpsiKS_Stripping17b_DVv33r1_FTfromDV_v2_20130117_fkruse_combined_tupleA_DD_SweightsForCBShapes.root", "Bd2JpsiKS", "Bd2JpsiKSDD_MC11_CBShapesSeparated_5390_to_5500"));

  
  std::vector<DimensionTuple> dimensions;
  
  RooRealVar obsMass("obsMass", "obsMass", 5180, 5520);
  RooRealVar sw_sig(argv[4], argv[4], 0.0);
  RooRealVar sw_bkg(argv[5], argv[5], 0.0);
  
  RooRealVar obsTime("obsTime", "obsTime", 0.0, 20.0);
  
  RooArgSet secondary_observables(obsMass, sw_sig, sw_bkg, obsTime);
  
//  EasyTuple etuple_sig("/fhgfs/groups/e5/lhcb/NTuples/Bs2JpsiKS/20121106_BDTPrestudies/Bd2JpsiKS_data_Stripping20_DVv33r1_FTfromDV_v3_20130107_fkruse_combined_tupleA_LLMassWin_sweighted_AllRangeBsExcluded_WithPresel.root", "Bd2JpsiKS");
  EasyTuple etuple_sig(tuples.front().get<0>(), argv[2]);
  TTree& tree = etuple_sig.tree();
  
  dimensions.push_back(DimensionTuple("varKSTauSignfificance", "varKSTauSignfificance", argv[4], argv[5], "varKSTauSignfificance<50&&varKSTauSignfificance>1", "varKSTauSignfificance_2", secondary_observables, false));
  dimensions.push_back(DimensionTuple("B0_AllDIRA_flat", "B0_AllDIRA_flat", argv[4], argv[5], "B0_AllDIRA_flat>0.999", "B0_AllDIRA_flat_2", secondary_observables, false));
  dimensions.push_back(DimensionTuple("B0_AllIPchi2_flat", "B0_AllIPchi2_flat", argv[4], argv[5], "B0_AllIPchi2_flat<100", "B0_AllIPchi2_flat_2", secondary_observables, false));
  
  TIterator* it = tree.GetIteratorOnAllLeaves();
  TLeaf* l = NULL;
  
  //  // loop tree, count nan sweighted events
  //  double nan_sum = 0.0;
  //  for (int i=0; i<tree.GetEntries(); ++i) {
  //    tree.GetEntry(i);
  //    if (!std::isfinite(tree.GetLeaf("KS0_TAUERR")->GetValue())) {
  //      nan_sum += tree.GetLeaf("sweight_sig")->GetValue();
  //    }
  //  }
  //  sdebug << "nan sum in signal: " << nan_sum << endmsg;
  
  while ((l = dynamic_cast<TLeaf*>(it->Next()))) {
    std::string leaf_name = l->GetName();
    std::string leaf_type = l->GetTypeName();
    if (l->GetLen() == 1 && leaf_type != "Bool_t" && leaf_name != argv[4] &&  leaf_name != argv[5]) {
      dimensions.push_back(DimensionTuple(l->GetName(), l->GetName(), argv[4], argv[5], "", l->GetName(), secondary_observables, false));
    }
  }
  delete it;
  
  PlotSweightedTuple(tuples, dimensions, pconfig);
}

int main(int argc, char* argv[]) {
  PlotConfig pconfig("plot_config");
  pconfig.InitializeOptions(argc, argv);
  
  if (argc<6) {
    sinfo << "Usage: " << argv[0] << " tuple_file tree_name run_identifier sig_sweight_name bkg_sweight_name" << endmsg;
    return 1;
  } else {
    Bd2JpsiKSSweights(pconfig, argc, argv);
  }
}
