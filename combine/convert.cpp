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
#include <TH2D.h>
#include <TH3D.h>
#include <THStack.h>
#include <string.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include <fstream>
#include <iostream>
#include <RooFit.h>
#include <RooRealVar.h>
#include <TKey.h>
#include <vector>
#include <map>
using namespace std;

TString sys_to_nom(TString h1_sys_name)
{
    int pos = 0;
    char *s = const_cast<char *>(h1_sys_name.Data());
    while (s[pos] != '_')
        pos++;
    if (h1_sys_name.Contains("ttbar"))
    {
        pos++;
        while (s[pos] != '_')
            pos++;
    }
    s[pos] = 0;
    return TString(s);
}
void sum_TH1D(TH1D *hs, TH1D **h1, int *start, int num)
{
    for (int f = 0; f < num; f++)
    {
        for (int i = 0; i < h1[f]->GetNbinsX(); i++)
        {
            hs->SetBinContent(i + 1 + start[f], h1[f]->GetBinContent(i + 1));
            hs->SetBinError(i + 1 + start[f], h1[f]->GetBinError(i + 1));
        }
    }
    hs->ResetStats();
}
void get_TH1D(TH1D *h1, TString h1_name, TH3D *h3, int like_cut, int ycut_low, int ycut_up, int *xbins, int nbins)
{
    h1->SetBins(nbins, 0, nbins);
    h1->SetName(h1_name);
    TH1D *h3_1 = h3->ProjectionX("3_px", ycut_low, ycut_up, 0, like_cut);

    double value;
    double err2;
    for (int i = 0; i < nbins; i++)
    {
        value = 0;
        err2 = 0;
        for (int bin = xbins[i]; bin < xbins[i + 1]; bin++)
        {
            value += h3_1->GetBinContent(bin + 1);
            err2 += h3_1->GetBinError(bin + 1) * h3_1->GetBinError(bin + 1);
        }
        h1->SetBinContent(i + 1, value);
        h1->SetBinError(i + 1, sqrt(err2));
    }
    // cout<<h1->GetSumOfWeights()<<endl;
    delete h3_1;
}
bool pdf_convert(TH1D hist, TH1D hist_nom, TH1D *hist_up, TH1D *hist_dn)
{
    bool same = true;
    for (int bin = 0; bin < hist.GetNbinsX(); bin++)
    {
        double up = hist.GetBinContent(bin + 1);
        double nom = hist_nom.GetBinContent(bin + 1);
        double dn;
        if (up != nom)
            same = false; //pdf_wight != 1;
        if (up == 0)
            dn = 0;
        else
            dn = nom * nom / up;
        hist_up->SetBinContent(bin + 1, up);
        hist_dn->SetBinContent(bin + 1, dn);
        hist_up->SetBinError(bin + 1, hist.GetBinError(bin + 1));
        hist_dn->SetBinError(bin + 1, hist.GetBinError(bin + 1));
    }
    return !same;
}
void convert(TString input, TString output, double likelihood_cut, vector<double> ycut_user, vector<vector<double>> xbins_user, bool fake_qcd)
{
    // convert
    int like_cut;
    if (likelihood_cut <= 29.0 && likelihood_cut > 4.0)
        like_cut = static_cast<int>(std::round((likelihood_cut - 4.0) / 0.25));
    else
        like_cut = -1;
    const int nycut = ycut_user.size();
    int ycut[nycut + 1];
    for (int i = 1; i < nycut; i++)
        ycut[i] = static_cast<int>(std::round(ycut_user[i] / 0.1));
    ycut[nycut] = 41;
    ycut[0] = -1;

    int xbins[nycut][20];
    int nbins[nycut];
    for (int i = 0; i < nycut; i++)
    {
        nbins[i] = xbins_user[i].size() - 1;
        // xbins[i] = new int[nbins[i]+1];
        for (int j = 0; j < nbins[i] + 1; j++)
        {
            if (xbins_user[i][j] < 300)
                xbins[i][j] = -1;
            else if (xbins_user[i][j] > 3000)
                xbins[i][j] = 271;
            else
                xbins[i][j] = int(std::round((xbins_user[i][j] - 300) / 10));
        }
    }
    TFile *f = new TFile(input);
    TList *list = f->GetListOfKeys();
    TFile *outFile = new TFile(output, "recreate");
    TKey *key;
    TIter iter(list); // or TIter iter(list->MakeIterator());
    static TString classname("TH3D");
    map<TString, vector<TH1D>> hist_pdf;
    int start[nycut + 1];
    int bin_num = 0;
    for (int i = 0; i < nycut; i++)
    {
        start[i] = bin_num;
        bin_num += nbins[i];
    }
    start[nycut] = bin_num;
    TString hist_name;
    map<TString, TH1D> hist_map;
    while ((key = (TKey *)iter()))
    {
        if (key->GetClassName() == classname)
        {
            TH3D *hist3 = (TH3D *)key->ReadObj();
            if (hist3)
            {
                hist_name = TString(hist3->GetName());
                hist_name.ReplaceAll("_sub", "");
                if (hist_name.Contains("pdf") && (hist_name.Contains("_w101") || hist_name.Contains("_w102")))
                {
                    hist_name.ReplaceAll("pdf", "alphas");
                    hist_name.ReplaceAll("_w101", "Up");
                    hist_name.ReplaceAll("_w102", "Down");
                }
                cout << hist_name << endl;
                TH1D *hists = new TH1D(hist_name, "", bin_num, 0, bin_num);
                TH1D *hist1[nycut];
                for (int f = 0; f < nycut; f++)
                {
                    hist1[f] = new TH1D;
                    get_TH1D(hist1[f], hist_name + Form("_%d", f), hist3, like_cut, ycut[f] + 1, ycut[f + 1], xbins[f], nbins[f]);
                }
                sum_TH1D(hists, hist1, start, nycut);
                if (!hist_name.Contains("pdf") || hist_name.Contains("ttbar"))
                    hist_map[hist_name] = *hists;
                else
                    hist_pdf[sys_to_nom(hist_name)].push_back(*hists);
                delete hist3;
                delete hists;
                for (int i = 0; i < nycut; i++)
                    delete hist1[i];
            }
        }
    }
    for (map<TString, TH1D>::iterator iter = hist_map.begin(); iter != hist_map.end(); iter++)
    {
        if (iter->first.Contains("pdf"))
        {
            TH1D *hists_up = new TH1D(iter->first + "Up", "", bin_num, 0, bin_num);
            TH1D *hists_dn = new TH1D(iter->first + "Down", "", bin_num, 0, bin_num);
            if (pdf_convert(iter->second, hist_map[sys_to_nom(iter->first)], hists_up, hists_dn))
            {
                outFile->cd();
                hists_up->Write();
                hists_dn->Write();
            }
            delete hists_up, hists_dn;
        }
        else if (!iter->first.Contains("QCD"))
        {
            outFile->cd();
            iter->second.Write();
        }
    }
    for (map<TString, vector<TH1D>>::iterator iter = hist_pdf.begin(); iter != hist_pdf.end(); iter++)
    {
        TH1D *hists_up = new TH1D(iter->first + "_pdf" + TString(iter->first[0]) + "Up", "", bin_num, 0, bin_num);
        TH1D *hists_dn = new TH1D(iter->first + "_pdf" + TString(iter->first[0]) + "Down", "", bin_num, 0, bin_num);
        double nom, up, dn, temp, err;
        for (int bin = 0; bin < bin_num; bin++)
        {
            nom = hist_map[iter->first].GetBinContent(bin + 1);
            up = nom;
            for (vector<TH1D>::iterator it = iter->second.begin(); it != iter->second.end(); it++)
            {
                temp = it->GetBinContent(bin + 1);
                if (temp < nom)
                {
                    if (temp == 0)
                        temp = 2 * nom;
                    else
                        temp = nom * nom / temp;
                }
                if (temp > up)
                {
                    up = temp;
                    err = it->GetBinError(bin + 1);
                }
            }
            if (up == 0)
                dn = 0;
            else
                dn = nom * nom / up;
            hists_up->SetBinContent(bin + 1, up);
            hists_up->SetBinError(bin + 1, err);
            hists_dn->SetBinContent(bin + 1, dn);
            hists_dn->SetBinError(bin + 1, err);
        }
        outFile->cd();
        hists_up->Write();
        hists_dn->Write();
        delete hists_up, hists_dn;
    }

    if (hist_map.find("QCD_derived_C") != hist_map.end())
    {
        TString ch = "";
        if (input.Contains("M"))
            ch += "_M";
        else
            ch += "_E";
        if (input.Contains("4jets"))
            ch += "4J";
        else
            ch += "3J";
        for (int year = 2015; year < 2019; year++)
        {
            if (input.Contains(Form("%d", year)))
                ch += Form("_%d", year);
        }
        
        TH1D *hist_qcd = (TH1D *)hist_map["QCD_derived_C"].Clone();
        TH1D *hist_qcd_up = (TH1D *)hist_map["QCD_derived_B"].Clone();
        TH1D *hist_qcd_dn = (TH1D *)hist_map["QCD_derived_D"].Clone();
        TH1D *hist_qcd_nup = (TH1D *)hist_map["QCD_derived_C"].Clone();
        TH1D *hist_qcd_ndn = (TH1D *)hist_map["QCD_derived_C"].Clone();
        
        hist_qcd->SetName("QCD");
        hist_qcd_up->SetName("QCD_qshape" + ch + "Up");
        hist_qcd_dn->SetName("QCD_qshape" + ch + "Down");
        hist_qcd_nup->SetName("QCD_qnorm" + ch + "Up");
        hist_qcd_ndn->SetName("QCD_qnorm" + ch + "Down");

        hist_qcd_up->Scale(1.0 / hist_qcd_up->GetSumOfWeights());
        hist_qcd_dn->Scale(1.0 / hist_qcd_dn->GetSumOfWeights());
        TH1D *temp = (TH1D *)hist_qcd_up->Clone();
        hist_qcd_up->Divide(hist_qcd_dn);
        hist_qcd_dn->Divide(temp);
        hist_qcd_up->Multiply(hist_qcd);
        hist_qcd_dn->Multiply(hist_qcd);

        double ns = hist_map["QCD_other_removed_C"].GetSumOfWeights() * hist_map["QCD_MC_CG_B"].GetSumOfWeights() / hist_map["QCD_MC_CG_D"].GetSumOfWeights();
        ns = ns / hist_qcd->GetSumOfWeights();
        hist_qcd_nup->Scale(ns);
        hist_qcd_ndn->Scale(1.0 / ns);
        outFile->cd();
        hist_qcd->Write();
        hist_qcd_up->Write();
        hist_qcd_dn->Write();
        hist_qcd_nup->Write();
        hist_qcd_ndn->Write();
        delete hist_qcd;
        delete hist_qcd_up; delete hist_qcd_dn;
        delete hist_qcd_nup; delete hist_qcd_ndn;
    }
    outFile->Close();
}