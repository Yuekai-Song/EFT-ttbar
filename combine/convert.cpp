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
#include "../analysis/prepare/settings.h"
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
void set0(TH1D *h1)
{
    for (int i = 0; i <= h1->GetNbinsX() + 1; i++)
    {
        if (h1->GetBinContent(i) < 0)
        {
            h1->SetBinContent(i, 0);
            h1->SetBinError(i, 0);
        }
    }
}
class convert
{
private:
    int *nbins, ncut, bin_num, *start;
    map<TString, TString> name_change;
    TString ch;
    vector<vector<int>> vbins[3];
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
    void change_sysname(TString &original);
    void set(TString input, TString output, vector<vector<vector<double>>> xbins_user, vector<var> vars);

public:
    convert(TString input, TString output, vector<vector<vector<double>>> xbins_user, vector<var> vars, map<TString, TString> name_change = {});
};
void convert::change_sysname(TString &original)
{
    if (!original.Contains("Up") && !original.Contains("Down"))
        return;
    TString temp = original;
    temp.ReplaceAll("Up", "");
    temp.ReplaceAll("Down", "");
    temp.ReplaceAll(sys_to_nom(original) + "_", "");
    if (name_change.find(temp) != name_change.end())
        original.ReplaceAll(temp, name_change[temp]);
}
vector<int> divide(vector<double> cuts, var variable)
{
    vector<int> bins;
    bins.clear();
    for (int i = 0; i < cuts.size(); i++)
    {
        if (cuts[i] < variable.xlow)
            bins.push_back(-1);
        else if (cuts[i] > variable.xup)
            bins.push_back(variable.bins + 1);
        else
            bins.push_back(static_cast<int>(std::round((cuts[i] - variable.xlow) / (variable.xup - variable.xlow) * variable.bins)));
    }
    return bins;
}
void convert::set(TString input, TString output, vector<vector<vector<double>>> xbins_user, vector<var> vars)
{
    ncut = xbins_user.size();
    nbins = new int[ncut];
    start = new int[ncut + 1];
    bin_num = 0;
    for (int i = 0; i < xbins_user.size(); i++)
    {
        for (int v = 0; v < 3; v++)
            vbins[v].push_back(divide(xbins_user[i][v], vars[v]));
        nbins[i] = xbins_user[i][2].size() - 1;
        start[i] = bin_num;
        bin_num += nbins[i];
    }
    // for (int i = 0; i < ncut; i++)
    // {
    //     for (int j = 0; j < 3; j++)
    //     {
    //         for (int k = 0; k < vbins[j][i].size(); k++)
    //             cout << vbins[j][i][k] << " ";
    //     }
    //     cout << endl;
    // }
    start[ncut] = bin_num;
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
    for (int i = 0; i < ncut; i++)
    {
        for (int k = 0; k < h1[i]->GetNbinsX(); k++)
        {
            hists->SetBinContent(k + 1 + start[i], h1[i]->GetBinContent(k + 1));
            hists->SetBinError(k + 1 + start[i], h1[i]->GetBinError(k + 1));
        }
    }
    hists->ResetStats();
    for (int i = 0; i < ncut; i++)
        delete h1[i];
}
void convert::get_TH1D()
{
    double value;
    double err2;
    h1 = new TH1D *[ncut];
    TH1D *h3_1;
    int f;
    for (int f = 0; f < ncut; f++)
    {
        h1[f] = new TH1D(Form("h1_%d", f), "", nbins[f], 0, nbins[f]);
        // cout << vbins[1][f][0] + 1 << " " << vbins[1][f][1] << " " << vbins[0][f][0] + 1 << " " << vbins[0][f][1] << endl;
        h3_1 = h3->ProjectionX("3_px", vbins[1][f][0] + 1, vbins[1][f][1], vbins[0][f][0] + 1, vbins[0][f][1]);
        for (int k = 0; k < nbins[f]; k++)
        {
            value = 0;
            err2 = 0;
            for (int bin = vbins[2][f][k]; bin < vbins[2][f][k + 1]; bin++)
            {
                value += h3_1->GetBinContent(bin + 1);
                err2 += h3_1->GetBinError(bin + 1) * h3_1->GetBinError(bin + 1);
            }
            h1[f]->SetBinContent(k + 1, value);
            h1[f]->SetBinError(k + 1, sqrt(err2));
        }
        delete h3_1;
    }
}
void convert::get_map()
{
    cout << infile->GetName() << endl;
    TList *list = infile->GetListOfKeys();
    TKey *key;
    TIter iter(list); // or TIter iter(list->MakeIterator());
    static TString classname("TH3D");
    TString hist_name, temp;
    while ((key = (TKey *)iter()))
    {
        if (key->GetClassName() == classname)
        {
            h3 = (TH3D *)key->ReadObj();
            if (h3)
            {
                hist_name = TString(h3->GetName());
                if (hist_name.Contains("pdf") && (hist_name.Contains("_w101") || hist_name.Contains("_w102")))
                {
                    hist_name.ReplaceAll("pdf", "alphas");
                    hist_name.ReplaceAll("_w101", "Up");
                    hist_name.ReplaceAll("_w102", "Down");
                }
                //change_sysname(hist_name);
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
    if (hist_map.find("QCD_MC_A") != hist_map.end())
    {
        TString En = "", CG = "D", CCG = "C", SCG = "B";
        if (ch.Contains("M"))
            En = "_En";
        TH1D *trans_sys = (TH1D *)hist_map["QCD_MC_A" + En].Clone("trans_sys");
        trans_sys->Divide(&hist_map["QCD_MC_D" + En]);
        TH1D *trans = (TH1D *)hist_map["QCD_MC_B" + En].Clone("trans");
        trans->Multiply(&hist_map["QCD_MC_C" + En]);
        trans->Divide(&hist_map["QCD_MC_D" + En]);
        trans->Divide(&hist_map["QCD_MC_D" + En]);   

        set0(&hist_map["QCD_prompt_" + CG]);
        TH1D *hist_qcd = (TH1D *)hist_map["QCD_prompt_" + CG].Clone("QCD");
        TH1D *hist_qcd_up = (TH1D *)hist_map["QCD_prompt_" + CG].Clone("QCD_shape" + ch + "Up");
        TH1D *hist_qcd_dn = (TH1D *)hist_map["QCD_prompt_" + CG].Clone("QCD_shape" + ch + "Down");
        
        hist_qcd->Multiply(trans);
        hist_qcd_up->Multiply(trans_sys);
        hist_qcd_dn->Divide(trans_sys);

        outfile->cd();
        hist_qcd->Write();
        hist_qcd_up->Write();
        hist_qcd_dn->Write();
        delete hist_qcd;
        delete hist_qcd_up;
        delete hist_qcd_dn;
    }
}
convert::convert(TString input, TString output, vector<vector<vector<double>>> xbins_user, vector<var> vars, map<TString, TString> name_changes = {})
{
    name_change = name_changes;
    set(input, output, xbins_user, vars);
    get_map();
    process_map();
    process_pdf();
    // process_qcd();
    infile->Close();
    outfile->Close();
}