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
#include <TCanvas.h>
#include <TH2F.h>
#include <THStack.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <TPaveText.h>
#include "../select_analysis/prepare/settings.h"
using namespace std;
double p2weight = 0.65 / 0.3;
double renew_weight(TString file)
{ // global weight
    auto c0 = new TCanvas("c0", "c0", 8, 30, 600, 600);
    TChain *tree = new TChain("rawtree");
    tree->Add(file);
    TH1D *nmc = new TH1D("nmc", "", 50, 0, 100);
    // nmc->Sumw2();
    c0->cd();
    tree->Draw("nJet>>nmc", "Generator_weight");
    // cout<<nmc->GetSumOfWeights()<<endl;
    float weight = 1000.0 / (nmc->GetSumOfWeights());
    delete tree;
    delete nmc;
    delete c0;
    return weight;
}
void derive(int cut_num, int year, TString cg, int var, bool ttx)
{
    TString process[] = {"ttbar", "DYJets", "STop", "WJets", "QCD_HT", "QCD_EMEn"};
    TString title[] = {"likelihood", "mass_t", "top_pt", "Mtt", "deltay"};
    TString xvars[] = {"likelihood", "mass_t", "rectop_pt", "mass_tt", "rapidity_tt"};
    Double_t xup[] = {50, 450, 500, 1500, 4};
    Double_t xlow[] = {13, 50, 50, 300, -4};
    Int_t bins[] = {37, 40, 24, 24, 20};
    settings s(cut_num, year, ttx);
    s.set_suf(cg);
    TChain *mytree;
    TH1D *hist, *h1;
    TH1D *hdata;
    TString inpath = Form("./output/%d/", year);
    TString in = "";
    if (ttx)
        in = "_ttx";
    if (s.cut_name.Contains("M"))
        process[5] = "QCD_MuEn";
    TString outpath = Form("./output/%d/CG_roots" + in, year);
    TFile *file = new TFile(outpath + "/QCD_" + s.cut_name + "_" + cg + ".root", "update");
    auto c1 = new TCanvas("c1", "c1", 8, 30, 600, 600); // temporary canvas
    TChain *data_tree = new TChain("mytree");
    data_tree->Add(inpath + "data/" + s.dataName());
    hdata = new TH1D(title[var] + "_data", "", bins[var], xlow[var], xup[var]);
    data_tree->Draw(xvars[var] + ">>" + title[var] + "_data", s.cut);
    hdata->Scale(s.pre_scale);
    file->cd();
    hdata->Write();
    delete hdata;
    delete data_tree;

    for (int k = 0; k < 6; k++)
    {
        h1 = new TH1D(title[var] + "_" + process[k], "", bins[var], xlow[var], xup[var]);
        h1->Sumw2();
        for (int j = s.index_pro[process[k]].first; j < s.index_pro[process[k]].second; j++)
        {
            TChain *tree = new TChain("mytree");
            tree->Add(inpath + "MC/" + s.fileName(j));
            TString weight = "Generator_weight*SF_btag*SF_lepton*pu_wt*L1PreFiringWeight_Nom*" + s.cut;
            if (k == 0)
                weight = weight + "*nnlo_wt";
            double gw = renew_weight(inpath + "MC/" + s.fileName(j));
            gw = gw * s.lumi * s.xs(s.fileName(j)).first * s.xs(s.fileName(j)).second;
            weight = Form("%lf*", gw) + weight;
            c1->cd();
            hist = new TH1D("hist", "", bins[var], xlow[var], xup[var]);
            hist->Sumw2();
            tree->Draw(xvars[var] + ">>hist", weight);
            // cout<<fileNames[j]<<": "<<hist->GetSumOfWeights()<<endl;
            h1->Add(hist);
            delete tree;
            delete hist;
        }
        file->cd();
        h1->Write();
        delete h1;
    }
    delete c1;
    file->Close();
}
void derive_cg(int i, int g, int year, bool is_ttx)
{
    TString cg_n[] = {"A", "B", "C", "D"};
    TString cg = cg_n[g];
    for (int var = 0; var < 5; var++)
        derive(i, year, cg, var, is_ttx);
}