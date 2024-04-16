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
bool pdf_convert(TH1D hist, TH1D hist_nom, TH1D *hist_up, TH1D *hist_dn)
{
    bool same = true;
    for (int bin = 0; bin < hist.GetNbinsX(); bin++)
    {
        double up = hist.GetBinContent(bin + 1);
        double nom = hist_nom.GetBinContent(bin + 1);
        double dn;
        if (up != nom)
            same = false; // pdf_wight != 1;
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
class convert
{
private:
    int like_cut, *ycut, *nbins, nycut, bin_num, *start;
    TString ch;
    int (*xbins)[20];
    TH1D *hists, **h1;
    TH3D *h3;
    TFile *infile, *outfile;
    map<TString, TH1D> hist_map;
    map<TString, vector<TH1D>> hist_pdf;
    void sum_TH1D();
    void get_TH1D();
    void get_map();
    void process_map();
    void process_pdf();
    void process_qcd();
    void set(TString input, TString output, double likelihood_cut, vector<double> ycut_user, vector<vector<double>> xbins_user);

public:
    convert(TString input, TString output, double likelihood_cut, vector<double> ycut_user, vector<vector<double>> xbins_user);
};
void convert::set(TString input, TString output, double likelihood_cut, vector<double> ycut_user, vector<vector<double>> xbins_user)
{
    if (likelihood_cut <= 33.0 && likelihood_cut > 8.0)
        like_cut = static_cast<int>(std::round((likelihood_cut - 8.0) / 0.25));
    else
        like_cut = -1;
    nycut = ycut_user.size();
    ycut = new int[nycut + 1];
    for (int i = 1; i < nycut; i++)
        ycut[i] = static_cast<int>(std::round(ycut_user[i] / 0.1));
    ycut[nycut] = 41;
    ycut[0] = -1;
    xbins = new int[nycut][20];
    nbins = new int[nycut];
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
    start = new int[nycut + 1];
    bin_num = 0;
    for (int i = 0; i < nycut; i++)
    {
        start[i] = bin_num;
        bin_num += nbins[i];
    }
    start[nycut] = bin_num;
    infile = new TFile(input);
    outfile = new TFile(output, "recreate");
    ch = "";
    if (input.Contains("M"))
        ch += "_M";
    else
        ch += "_E";
    if (input.Contains("4jets"))
        ch += "4j";
    else
        ch += "3j";
    for (int year = 2015; year < 2019; year++)
    {
        if (input.Contains(Form("%d", year)))
            ch += Form("_%d", year);
    }
}
void convert::sum_TH1D()
{
    for (int f = 0; f < nycut; f++)
    {
        for (int i = 0; i < h1[f]->GetNbinsX(); i++)
        {
            hists->SetBinContent(i + 1 + start[f], h1[f]->GetBinContent(i + 1));
            hists->SetBinError(i + 1 + start[f], h1[f]->GetBinError(i + 1));
        }
    }
    hists->ResetStats();
}
void convert::get_TH1D()
{
    double value;
    double err2;
    h1 = new TH1D *[nycut];
    for (int f = 0; f < nycut; f++)
    {
        h1[f] = new TH1D;
        h1[f]->SetBins(nbins[f], 0, nbins[f]);
        h1[f]->SetName(Form("h1_%d", f));
        TH1D *h3_1 = h3->ProjectionX("3_px", ycut[f] + 1, ycut[f + 1], 0, like_cut);
        for (int i = 0; i < nbins[f]; i++)
        {
            value = 0;
            err2 = 0;
            for (int bin = xbins[f][i]; bin < xbins[f][i + 1]; bin++)
            {
                value += h3_1->GetBinContent(bin + 1);
                err2 += h3_1->GetBinError(bin + 1) * h3_1->GetBinError(bin + 1);
            }
            h1[f]->SetBinContent(i + 1, value);
            h1[f]->SetBinError(i + 1, sqrt(err2));
        }
    }
}
void convert::get_map()
{
    cout << infile->GetName() << endl;
    TList *list = infile->GetListOfKeys();
    TKey *key;
    TIter iter(list); // or TIter iter(list->MakeIterator());
    static TString classname("TH3D");
    TString hist_name;
    while ((key = (TKey *)iter()))
    {
        if (key->GetClassName() == classname)
        {
            h3 = (TH3D *)key->ReadObj();
            if (h3)
            {
                hist_name = TString(h3->GetName());
                hist_name.ReplaceAll("_sub", "");
                if (hist_name.Contains("pdf") && (hist_name.Contains("_w101") || hist_name.Contains("_w102")))
                {
                    hist_name.ReplaceAll("pdf", "alphas");
                    hist_name.ReplaceAll("_w101", "Up");
                    hist_name.ReplaceAll("_w102", "Down");
                }
                cout << hist_name << endl;
                hists = new TH1D(hist_name, "", bin_num, 0, bin_num);
                get_TH1D();
                sum_TH1D();
                if (!hist_name.Contains("pdf") || hist_name.Contains("ttbar"))
                    hist_map[hist_name] = *hists;
                else
                    hist_pdf[sys_to_nom(hist_name)].push_back(*hists);
                delete h3;
                delete hists;
                for (int i = 0; i < nycut; i++)
                    delete h1[i];
            }
        }
    }
}
void convert::process_map()
{
    for (map<TString, TH1D>::iterator iter = hist_map.begin(); iter != hist_map.end(); iter++)
    {
        if (iter->first.Contains("pdf"))
        {
            TH1D *hists_up = new TH1D(iter->first + "Up", "", bin_num, 0, bin_num);
            TH1D *hists_dn = new TH1D(iter->first + "Down", "", bin_num, 0, bin_num);
            if (pdf_convert(iter->second, hist_map[sys_to_nom(iter->first)], hists_up, hists_dn))
            {
                outfile->cd();
                hists_up->Write();
                hists_dn->Write();
            }
            delete hists_up, hists_dn;
        }
        else if (!iter->first.Contains("QCD"))
        {
            outfile->cd();
            iter->second.Write();
        }
    }
}
void convert::process_pdf()
{
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
        outfile->cd();
        hists_up->Write();
        hists_dn->Write();
        delete hists_up, hists_dn;
    }
}
void convert::process_qcd()
{
    if (hist_map.find("QCD_derived_C") != hist_map.end())
    {
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
        outfile->cd();
        hist_qcd->Write();
        hist_qcd_up->Write();
        hist_qcd_dn->Write();
        hist_qcd_nup->Write();
        hist_qcd_ndn->Write();
        delete hist_qcd;
        delete hist_qcd_up;
        delete hist_qcd_dn;
        delete hist_qcd_nup;
        delete hist_qcd_ndn;
    }
}
convert::convert(TString input, TString output, double likelihood_cut, vector<double> ycut_user, vector<vector<double>> xbins_user)
{
    set(input, output, likelihood_cut, ycut_user, xbins_user);
    get_map();
    process_map();
    process_pdf();
    process_qcd();
    infile->Close();
    outfile->Close();
}