#include <TROOT.h>
#include <TStyle.h>
#include <Math/PdfFuncMathCore.h>
#include <TFile.h>
#include <TChain.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <TLegend.h>
#include <TF1.h>
#include <TH1D.h>
#include <TH3D.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <THStack.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include "../select_analysis/prepare/settings.h"
using namespace std;

void renew_weight(TString file, TString *weight, int f, int year)
{ // global weight
    const int nsample = 45;
    Float_t cross_sections[nsample] = {366.91, 89.05, 377.96,
                                       169.9, 147.4, 41.0, 5.7, 1.4, 0.63, 0.15, 0.0036,
                                       3.36, 136.02, 80.95, 35.6, 35.6,
                                       8927.0, 2809.0, 826.3, 544.3,
                                       // 53870.0,
                                       // 1264.0,1345.7, 359.7, 48.9, 12.1, 5.5, 1.3, 0.032,//LO
                                       186100000.0, 23590000, 1555000, 324500, 30310, 6444, 1127, 109.8, 21.98,
                                       6401000.0, 1993000.0, 364000.0, 66600.0, 16620.0, 1101.0,
                                       1367000.0, 381700.0, 87740.0, 21280.0, 7000.0, 622.6, 58.9, 18.12, 3.318, 1.085};
    Float_t K_Factor[nsample] = {1.0, 1.0, 1.0,
                                 1.23, 1.23, 1.23, 1.23, 1.23, 1.23, 1.23, 1.23,
                                 1.0, 1.0, 1.0, 1.0, 1.0,
                                 1.21, 1.21, 1.21, 1.21, // 1.21,1.21,1.21,1.21,
                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    double lumi_s[4] = {19.5, 16.8, 41.48, 59.83};
    double lumi = lumi_s[year - 2015];
    auto c0 = new TCanvas("c0", "c0", 8, 30, 600, 600);
    TChain *tree1 = new TChain("rawtree");
    tree1->Add(file);
    TH1D *nmc = new TH1D("nmc", "", 50, 0, 100);
    // nmc->Sumw2();
    c0->cd();
    tree1->Draw("nJet>>nmc", "Generator_weight");
    // cout<<nmc->GetSumOfWeights()<<endl;
    float weight1 = cross_sections[f] * lumi / (nmc->GetSumOfWeights()) * K_Factor[f] * 1000;
    // cout<<weight1<<endl;
    *weight = Form("%f*", weight1) + (*weight);
    delete tree1;
    delete nmc;
    delete c0;
}
void derive(TString cut, TString cut_name, TString cg, int year, vector<int> xyz_bins, vector<double> xyz_range, bool isEnriched, TString in = "", TString out = "")
{
    const int nsample = 45;
    int xbins = xyz_bins[0];
    int ybins = xyz_bins[1];
    int zbins = xyz_bins[2];
    double xlow = xyz_range[0];
    double xup = xyz_range[1];
    double ylow = xyz_range[2];
    double yup = xyz_range[3];
    double zlow = xyz_range[4];
    double zup = xyz_range[5];
    TString *fileName[2];
    TString fileNames[nsample] = {"new_TTToSemiLeptonic_TuneCP5_13TeV-powheg.root",
                                  "new_TTTo2L2Nu_TuneCP5_13TeV-powheg.root",
                                  "new_TTToHadronic_TuneCP5_13TeV-powheg.root",

                                  "new_DYJetsToLL_M-50_HT-70to100_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                                  "new_DYJetsToLL_M-50_HT-100to200_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                                  "new_DYJetsToLL_M-50_HT-200to400_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                                  "new_DYJetsToLL_M-50_HT-400to600_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                                  "new_DYJetsToLL_M-50_HT-600to800_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                                  "new_DYJetsToLL_M-50_HT-800to1200_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                                  "new_DYJetsToLL_M-50_HT-1200to2500_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                                  "new_DYJetsToLL_M-50_HT-2500toInf_TuneCP5_PSweights_13TeV-madgraphMLM.root",

                                  "new_ST_s-channel_4f_leptonDecays_TuneCP5_13TeV-amcatnlo.root",
                                  "new_ST_t-channel_antitop_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin.root",
                                  "new_ST_t-channel_top_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin.root",
                                  "new_ST_tW_antitop_5f_inclusiveDecays_TuneCP5_13TeV-powheg.root",
                                  "new_ST_tW_top_5f_inclusiveDecays_TuneCP5_13TeV-powheg.root",

                                  "new_W1JetsToLNu_TuneCP5_13TeV-madgraphMLM.root",
                                  "new_W2JetsToLNu_TuneCP5_13TeV-madgraphMLM.root",
                                  "new_W3JetsToLNu_TuneCP5_13TeV-madgraphMLM.root",
                                  "new_W4JetsToLNu_TuneCP5_13TeV-madgraphMLM.root",

                                  "new_QCD_HT50to100_TuneCP5_PSWeights_13TeV-madgraph*.root",
                                  "new_QCD_HT100to200_TuneCP5_PSWeights_13TeV-madgraph*.root",
                                  "new_QCD_HT200to300_TuneCP5_PSWeights_13TeV-madgraph*.root",
                                  "new_QCD_HT300to500_TuneCP5_PSWeights_13TeV-madgraph*.root",
                                  "new_QCD_HT500to700_TuneCP5_PSWeights_13TeV-madgraph*.root",
                                  "new_QCD_HT700to1000_TuneCP5_PSWeights_13TeV-madgraph*.root",
                                  "new_QCD_HT1000to1500_TuneCP5_PSWeights_13TeV-madgraph*.root",
                                  "new_QCD_HT1500to2000_TuneCP5_PSWeights_13TeV-madgraph*.root",
                                  "new_QCD_HT2000toInf_TuneCP5_PSWeights_13TeV-madgraph*.root",

                                  "new_QCD_Pt-30to50_EMEnriched_TuneCP5_13TeV.root",
                                  "new_QCD_Pt-50to80_EMEnriched_TuneCP5_13TeV.root",
                                  "new_QCD_Pt-80to120_EMEnriched_TuneCP5_13TeV.root",
                                  "new_QCD_Pt-120to170_EMEnriched_TuneCP5_13TeV.root",
                                  "new_QCD_Pt-170to300_EMEnriched_TuneCP5_13TeV.root",
                                  "new_QCD_Pt-300toInf_EMEnriched_TuneCP5_13TeV.root",

                                  "new_QCD_Pt-30To50_MuEnrichedPt5_TuneCP5_13TeV.root",
                                  "new_QCD_Pt-50To80_MuEnrichedPt5_TuneCP5_13TeV.root",
                                  "new_QCD_Pt-80To120_MuEnrichedPt5_TuneCP5_13TeV.root",
                                  "new_QCD_Pt-120To170_MuEnrichedPt5_TuneCP5_13TeV.root",
                                  "new_QCD_Pt-170To300_MuEnrichedPt5_TuneCP5_13TeV.root",
                                  "new_QCD_Pt-300To470_MuEnrichedPt5_TuneCP5_13TeV.root",
                                  "new_QCD_Pt-470To600_MuEnrichedPt5_TuneCP5_13TeV.root",
                                  "new_QCD_Pt-600To800_MuEnrichedPt5_TuneCP5_13TeV.root",
                                  "new_QCD_Pt-800To1000_MuEnrichedPt5_TuneCP5_13TeV.root",
                                  "new_QCD_Pt-1000_MuEnrichedPt5_TuneCP5_13TeV.root"};

    TString en = "";
    int edge_dn[] = {0, 20};
    int edge_up[] = {20, 29};
    if (isEnriched)
    {
        en = "_En";
        if (cut_name.Contains("E"))
        {
            edge_dn[1] = 29;
            edge_up[1] = 35;
        }
        else if (cut_name.Contains("M"))
        {
            edge_dn[1] = 35;
            edge_up[1] = 45;
        }
    }
    TString edge_name[] = {"other", "QCD"};
    fileName[0] = fileNames;
    fileName[1] = new TString[nsample];
    for (int i = 0; i < nsample; i++)
    {
        fileName[0][i].ReplaceAll(".root", "_*_" + cg + in + ".root");
        fileName[1][i] = fileName[0][i];
        fileName[1][i].ReplaceAll(cg + in + ".root", "A" + in + ".root");
    }
    TString region[2] = {"Control", "Signal"}; 
    // Lepton_triggers in C and D regions have prescales
    Double_t pre_scale_year[][2] = {{369.84, 130.38}, {1570.17, 162.22}, {1085.83, 224.41}, {1536.28, 474.95}};
    Double_t pre_scale;
    if (cg == "B")
        pre_scale = 1.0;
    else if (cut_name.Contains("M"))
        pre_scale = pre_scale_year[year - 2015][1];
    else
        pre_scale = pre_scale_year[year - 2015][0];

    TString input_path = Form("./output/%d/", year);
    TString output_path = Form("./output/%d/QCD_root" + out + "/", year);
    TFile *file = new TFile(output_path + "QCD_" + cut_name + "_" + cg + "_3D" + en + ".root", "recreate");
    TH3D *hdata, *hmc_b[2], *hmc_qa, *hmc;

    hmc_b[0] = new TH3D("other_MC_CG", "", xbins, xlow, xup, ybins, ylow, yup, zbins, zlow, zup);
    hmc_b[1] = new TH3D("QCD_MC_CG_" + cg + en, "", xbins, xlow, xup, ybins, ylow, yup, zbins, zlow, zup);
    hmc_qa = new TH3D("QCD_MC_SG" + en, "", xbins, xlow, xup, ybins, ylow, yup, zbins, zlow, zup);
    hmc_b[0]->Sumw2();
    hmc_b[1]->Sumw2();
    hmc_qa->Sumw2();

    TChain *data_tree = new TChain("mytree");
    data_tree->Add(input_path + "data/new_data*_" + cg + in + ".root");
    auto c1 = new TCanvas("c1", "c1", 8, 30, 600, 600);
    c1->cd();
    cout << data_tree->GetEntries() << endl;
    hdata = new TH3D("QCD_prompt_CG_" + cg, "", xbins, xlow, xup, ybins, ylow, yup, zbins, zlow, zup); // final QCD;
    if (out.Contains("corr"))
        data_tree->Draw("likelihood:fabs(rapidity_tt):mass_tt >> QCD_prompt_CG_" + cg, cut);
    else
        data_tree->Draw("likelihood:fabs(rapidity_tt):mass_tt_uncorr >> QCD_prompt_CG_" + cg, cut);
    hdata->Scale(pre_scale);
    delete data_tree;

    TChain *MC_tree;
    for (int type = 0; type < 2; type++)
    {
        for (int k = 0; k < 2; k++)
        {
            if (type == 0)
                hmc = hmc_b[k];
            else if (k == 1)
                hmc = hmc_qa;
            else
                continue;
            for (int j = edge_dn[k]; j < edge_up[k]; j++)
            {
                MC_tree = new TChain("mytree");
                MC_tree->Add(input_path + "MC/" + fileName[type][j]);
                TString weight = "Generator_weight*SF_btag*SF_lepton*pu_wt*L1PreFiringWeight_Nom*" + cut;
                if (j < 3)
                    weight = weight + "*nnlo_wt";
                renew_weight(input_path + "MC/" + fileName[type][j], &weight, j, year);
                c1->cd();
                TH3D *hist = new TH3D("hist", "", xbins, xlow, xup, ybins, ylow, yup, zbins, zlow, zup);
                hist->Sumw2();
                if (out.Contains("corr"))
                    MC_tree->Draw("likelihood:fabs(rapidity_tt):mass_tt>>hist", weight);
                else
                    MC_tree->Draw("likelihood:fabs(rapidity_tt):mass_tt_uncorr>>hist", weight);
                hmc->Add(hist);
                delete hist;
                delete MC_tree;
            }
        }
        cout << "finished " + region[type] +  " region" << endl;
    }
    // cout<<hdata->GetSumOfWeights()<<endl;
    cout << hdata->GetSumOfWeights() << " " << hmc_b[0]->GetSumOfWeights()<< endl;
    hdata->Add(hmc_b[0], -1.0); // shape

    file->cd();
    hmc_b[1]->Write();  // QCD_MC_CG (QCD MC samples in CG)
    if (isEnriched)
        hdata->Write(); // data_prompt_CG (data-prompt MCs in CG)
    if (cg == "C")
        hmc_qa->Write();// QCD_MC_SG

    delete hmc_b[0];
    delete hmc_b[1];
    delete hmc_qa;
    delete hdata;
    delete c1;
    file->Close();
}
void derive_qcd(int i, int g, int year, bool isEnriched, bool is_ttx, bool is_corr)
{
    // TString cg[] = {"A", "B", "C", "D"};
    TString cg_n[] = {"A", "B", "C", "D"};
    TString cg = cg_n[g];
    TString in = "", out = "";
    settings s(year, is_ttx, cg);
    if (is_ttx)
    {
        in += "_ttx";
        out += "_ttx";
    }
    if (is_corr)
        out += "_corr";
    derive(s.cuts[i], s.cutsName[i], cg, year, s.xyz_bins, s.xyz_range, isEnriched, in, out);
}