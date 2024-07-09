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
#include <TLine.h>
#include <TGraph.h>
#include <TKey.h>
#include <TGraphAsymmErrors.h>
#include <fstream>
#include <iostream>
#include <map>
using namespace std;

double p2weight = 0.65 / 0.3;
void sete0(TH1D *h1)
{
    for (int i = 0; i < h1->GetNbinsX(); i++)
    {
        h1->SetBinError(i, 0);
    }
}
void set_ratio(TH1D *h1, double a)
{
    for (int i = 1; i <= h1->GetNbinsX(); i++)
        h1->SetBinContent(i, a);
}
void sys_and_nom(TString hist_name, TString &sys_name, TString &nom_name)
{
    int pos = 0;
    hist_name.ReplaceAll("Up", "");
    hist_name.ReplaceAll("Down", "");
    char *s = const_cast<char *>(hist_name.Data());
    while (s[pos] != '_')
        pos++;
    if (hist_name.Contains("ttbar"))
    {
        pos++;
        while (s[pos] != '_')
            pos++;
    }
    s[pos] = 0;
    nom_name = TString(s);
    sys_name = TString(s + pos + 1);
}
void get_error(TH1D* h1_up, TH1D* h1_dn, TH1D* h1_nom, double *up_vs, double *down_vs, bool envelope = 0)
{
    double nom_v;
    double up_v, up_max;
    double down_v, down_min;
    const int n = h1_nom->GetNbinsX();
    // cout<<sys[s]<<": "<<pro[p]<<": "<<"bin 3: ";
    // cout<<up->GetBinContent(4)<<" "<<down->GetBinContent(4)<<" "<<h1->GetBinContent(4)<<endl;
    for (int b = 0; b < n; b++)
    {
        up_v = h1_up->GetBinContent(b + 1);
        nom_v = h1_nom->GetBinContent(b + 1);
        if (h1_dn)
        {
            down_v = h1_dn->GetBinContent(b + 1);
        }
        else
        {
            if (up_v == 0)
                down_v = 0;
            else
                down_v = nom_v * nom_v / up_v;
        }
        up_max = max(max(up_v, down_v) - nom_v, (double)0);
        down_min = min(min(up_v, down_v) - nom_v, (double)0);
        if (!envelope)
        {
            up_vs[b] += up_max * up_max;
            down_vs[b] += down_min * down_min;
        }
        else
        {
            if (up_max > up_vs[b])
                up_vs[b] = up_max;
            if (down_min < down_vs[b])
                up_vs[b] = down_min;
        }
    }
}

void set_error(TGraphAsymmErrors *hg, TH1D *h1, double bin_len, map<TString, TH1D> &hist_map, map<TString, std::vector<TString>> &nom_sys, map<TString, std::vector<TString>> &nom_pdf, bool is_abs)
{
    const int n = h1->GetNbinsX();
    double up_vs[n], down_vs[n], nom;
    double up_en[n], down_en[n];
    TString pro[] = {"ttbar_ci0000", "DYJets", "STop", "WJets"};
    for (int i = 0; i < n; i++)
    {
        up_vs[i] = 0; down_vs[i] = 0;
    }
    for (int p = 0; p < 4; p++)
    {
        for (vector<TString>::iterator it_sys = nom_sys[pro[p]].begin(); it_sys != nom_sys[pro[p]].end(); it_sys++)
            get_error(&hist_map[pro[p] + "_" + *it_sys + "Up"], &hist_map[pro[p] + "_" + *it_sys + "Down"], &hist_map[pro[p]], up_vs, down_vs);
        if (p == 0)
        {
            for (vector<TString>::iterator it_sys = nom_pdf[pro[p]].begin(); it_sys != nom_pdf[pro[p]].end(); it_sys++)
                get_error(&hist_map[pro[p] + "_" + *it_sys], 0, &hist_map[pro[p]], up_vs, down_vs);
        }
        else
        {
            for (int i = 0; i < n; i++)
            {
                up_en[i] = 0; down_en[i] = 0;
            }
            for (vector<TString>::iterator it_sys = nom_pdf[pro[p]].begin(); it_sys != nom_pdf[pro[p]].end(); it_sys++)
                get_error(&hist_map[pro[p] + "_" + *it_sys], 0, &hist_map[pro[p]], up_en, down_en, 1);
            for (int i = 0; i < n; i++)
            {
                up_vs[i] += up_en[i]; down_vs[i] += down_en[i];
            }
        }
    }
    if (!is_abs)
    {
        for (int i = 0; i < n; i++)
        {
            nom = h1->GetBinContent(i + 1);
            if (nom == 0)
            {
                up_vs[i] = 0;
                down_vs[i] = 0;
            }
            else
            {
                up_vs[i] /= nom * nom;
                down_vs[i] /= nom * nom;
            }
        }
    }
    for (int i = 0; i < n; i++)
    {
        hg->SetPointEXlow(i, bin_len / 2.0);
        hg->SetPointEXhigh(i, bin_len / 2.0);
        hg->SetPointEYhigh(i, sqrt(up_vs[i]));
        hg->SetPointEYlow(i, sqrt(down_vs[i]));
    }
}
void format(TLegend *leg)
{
    leg->SetNColumns(1);
    leg->SetColumnSeparation(0.5);
    leg->SetEntrySeparation(0.0);
    leg->SetBorderSize(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.03);
    leg->SetLineColor(1);
    leg->SetLineStyle(1);
    leg->SetLineWidth(1);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
}
void format(TCanvas *c)
{
    c->SetFillColor(0);
    c->SetBorderMode(0);
    c->SetBorderSize(2);
    c->SetTickx(1);
    c->SetTicky(1);
    c->SetLeftMargin(0.18);
    c->SetRightMargin(0.05);
    c->SetTopMargin(0.05);
    c->SetBottomMargin(0.15);
    c->SetFrameFillStyle(0);
    c->SetFrameBorderMode(0);
    c->SetFrameFillStyle(0);
    c->SetFrameBorderMode(0);
}
void format(THStack *hstack, TString xtitle)
{
    hstack->GetXaxis()->SetNdivisions(505);
    hstack->GetXaxis()->SetLabelFont(42);
    hstack->GetXaxis()->SetLabelOffset(0.007);
    hstack->GetXaxis()->SetLabelSize(0.00);
    hstack->GetXaxis()->SetTitleSize(0.04);
    hstack->GetXaxis()->SetTitleOffset(1.3);
    hstack->GetXaxis()->SetTitleFont(42);
    hstack->GetYaxis()->SetNdivisions(505);
    hstack->GetYaxis()->SetLabelFont(42);
    hstack->GetYaxis()->SetLabelOffset(0.007);
    hstack->GetYaxis()->SetLabelSize(0.05);
    hstack->GetYaxis()->SetTitleSize(0.06);
    hstack->GetYaxis()->SetTitleOffset(1.1);
    hstack->GetYaxis()->SetTitleFont(42);
    hstack->GetXaxis()->CenterTitle();
    hstack->GetYaxis()->CenterTitle();
    hstack->GetXaxis()->SetTitle("");
    hstack->GetYaxis()->SetTitle("Events/bin");
    // cout<<hstack->GetMaximum()<<endl;
    // hstack->GetYaxis()->SetRangeUser(0, hstack->GetMaximum() * 1.4);
    hstack->SetMaximum(hstack->GetMaximum() * 1.4);
}
void format(TPad *pad1, TPad *pad2, TPad *pad3)
{
    pad1->Draw();
    pad2->Draw();
    pad3->Draw();
    pad1->cd();
    pad1->SetTopMargin(0.1);
    pad1->SetBottomMargin(0.02);
    pad1->SetLeftMargin(0.15);
    // pad1->SetTopMargin(0);
    pad2->cd();
    // pad2->SetTopMargin(0);
    pad2->SetTopMargin(0.03);
    pad2->SetBottomMargin(0.35);
    pad2->SetLeftMargin(0.15);

    pad3->cd();
    // pad2->SetTopMargin(0);
    pad3->SetTopMargin(0.03);
    pad3->SetBottomMargin(0.35);
    pad3->SetLeftMargin(0.15);
    pad3->SetFillColor(0);
    pad3->SetFillStyle(4000);
    pad3->SetFrameFillStyle(0);
}
void format(TH1D *h1, TString xtitle, bool is_pad2)
{
    if (is_pad2)
    {
        h1->SetMarkerStyle(1);
        //h1->SetMarkerSize(0.1);
        h1->SetLineWidth(2);
        h1->SetLineColor(1);
        h1->SetStats(kFALSE);
        h1->GetXaxis()->SetTitle(xtitle);
        h1->GetYaxis()->SetTitle("Ratio");
        h1->SetTitle("");
        h1->GetXaxis()->CenterTitle();
        h1->GetYaxis()->CenterTitle();
        h1->GetYaxis()->SetNdivisions(505);
        h1->GetXaxis()->SetTitleSize(0.12);
        h1->GetYaxis()->SetTitleSize(0.06 * p2weight);
        h1->GetXaxis()->SetTitleOffset(1.0);
        h1->GetYaxis()->SetTitleOffset(1.1 / p2weight);
        h1->GetXaxis()->SetLabelSize(0.11);
        h1->GetYaxis()->SetLabelSize(0.05 * p2weight);
        h1->GetYaxis()->SetRangeUser(0.5, 1.5);
    }
    else
    {
        h1->SetMarkerSize(0.8);
        h1->SetMarkerStyle(21);
    }
}
void format(TLine *l1)
{
    l1->SetLineWidth(1);
    l1->SetLineStyle(3);
    l1->SetLineColor(1);
}
void format(TGraphAsymmErrors *hg, int color)
{
    gStyle->SetHatchesSpacing(0.65);
    gStyle->SetHatchesLineWidth(1); 
    hg->SetFillColor(color);
    hg->SetFillStyle(3353);
}
void draw_qcd(TString cut_name, int var, int year)
{ // 2, 0
    TString legend[] = {"#eta_{t}", "tt", "DY", "single top", "V+jets", "QCD"};
    TString pro[] = {"Eta", "ttbar_ci0000", "DYJets", "STop", "WJets", "QCD"};
    TString legendd = "data";
    TString xtitle[] = {"-lnL", "M_{t}", "M_{#bar{t}}", "M_{Wl}", "M_{Wh}", "M_{th}", "M_{tl}", "P_{T}^{l}", "P_{T}^{leading-jet}", "jet_num", "p_{T}^{t}", "M_{t#bar{t}}", "M_{t#bar{t}}", "#Deltay_{t#bar{t}}"};
    TString title[] = {"likelihood", "mass_t", "mass_at", "mass_wlep", "mass_whad", "mass_thad", "mass_tlep", "lepton_pt", "leading_pt", "jet_num", "top_pt", "Mtt_un", "Mtt", "deltay"};
    Double_t xup, xdown, bin_len;
    Int_t bins;
    Double_t nums, events;

    TString path = Form("../sys_root/%d/", year);
    int color[] = {93, 2, 46, 9, 29, 8, kYellow};

    TString sys_file = path + title[var] + "_ttbar_" + cut_name + ".root";
    TFile *file = TFile::Open(sys_file);
    TList *list = file->GetListOfKeys();
    TIter iter(list);
    TKey *key;
    map<TString, TH1D> hist_map;
    map<TString, std::vector<TString>> nom_sys;
    map<TString, std::vector<TString>> nom_pdf;
    TString sys_name, nom_name;
    while ((key = (TKey *)iter()))
    {
        if ((TString)key->GetClassName() == "TH1D")
        {
            TH1D *hist = (TH1D *)key->ReadObj();
            if (hist)
            {
                TString hist_name = TString(hist->GetName());
                hist_name.ReplaceAll("_sub", "");
                hist_map[hist_name] = *hist;
                if (hist_name.Contains("Up") || hist_name.Contains("Down"))
                {
                    sys_and_nom(hist_name, sys_name, nom_name);
                    nom_sys[nom_name].push_back(sys_name);
                }
                if (hist_name.Contains("pdf"))
                {
                    sys_and_nom(hist_name, sys_name, nom_name);
                    nom_pdf[nom_name].push_back(sys_name);
                }
            }
            delete hist;
        }
    }
    file->Close();
    TH1D *nmc, *hist, *h1[6];
    TH1D *hdata, *hdatad, *hmc;
    TGraphAsymmErrors *hmcdg, *hmcg;

    auto c1 = new TCanvas("c1", "c1", 8, 30, 600, 600); // temporary canvas
    auto c2 = new TCanvas("c2", "c2", 8, 30, 650, 650);
    TPad *pad1 = new TPad("pad1", "This is pad1", 0.05, 0.32, 0.95, 0.97);
    TPad *pad2 = new TPad("pad2", "This is pad2", 0.05, 0.02, 0.95, 0.32);
    TPad *pad3 = new TPad("pad3", "This is pad3", 0.05, 0.02, 0.95, 0.32);
    c2->cd();
    format(pad1, pad2, pad3);
    TLegend *leg = new TLegend(0.70, .65, 1.00, .90);
    format(leg);
    format(c2);

    hdata = &hist_map["data_obs"];
    hdata->Sumw2(0);
    bins = hdata->GetNbinsX();
    xdown = hdata->GetXaxis()->GetBinLowEdge(1);
    xup = hdata->GetXaxis()->GetBinUpEdge(bins);
    bin_len = (xup - xdown) / (1.0 * bins);

    hmc = new TH1D("mc", "", bins, xdown, xup);
    hmc->Sumw2();
    hdatad = (TH1D *)hdata->Clone();
    hdatad->SetName("datad");
    THStack *hstack = new THStack("hstack", "");
    nums = 0;
    events = hdata->GetSumOfWeights();

    // h1[4] = (TH1D*)qcd->Get(title[var]+"_QCD_derived");
    // cout<<legend[4]<<": "<<h1[4]->GetSumOfWeights()<<endl;
    // h1[4]->SetFillColor(color[4]);
    // hstack->Add(h1[4]);
    // hmc->Add(h1[4]);
    // nums+=h1[4]->GetSumOfWeights();
    // leg->AddEntry(h1[4], legend[4], "f");
    for (int k = 5; k >= 0; k--)
    {
        h1[k] = &hist_map[pro[k]];
        // cout<<legend[k]<<": "<<h1[k]->GetSumOfWeights()<<endl;
        hmc->Add(h1[k]);
        nums += h1[k]->GetSumOfWeights();
        h1[k]->SetFillColor(color[k]);
        hstack->Add(h1[k]);
    }
    for (int k = 0; k < 6; k++)
    {
        leg->AddEntry(h1[k], legend[k], "f");
    }
    leg->AddEntry(hdata, legendd, "p");

    sete0(hmc);
    hdatad->Divide(hmc);
    TH1D *hratio = (TH1D *)hmc->Clone();
    hratio->Divide(hmc);
    hmcdg = new TGraphAsymmErrors(hratio);
    hmcg = new TGraphAsymmErrors(hmc);
    //set_error(hmcdg, hmc, bin_len, hist_map, nom_sys, nom_pdf, 0);
    //set_error(hmcg, hmc, bin_len, hist_map, nom_sys, nom_pdf, 1);
    // cout<<nums<<" "<<events<<endl;

    pad1->cd();
    hstack->Draw("hist");
    format(hstack, xtitle[var]);
    leg->Draw("same");
    hdata->Draw("PSame");
    format(hdata, "", 0);
    //hmcg->Draw("2Same");
    //format(hmcg, 1);

    pad2->cd();
    gStyle->SetOptStat(0);
    format(hdatad, xtitle[var], 1);
    hdatad->Draw();
    //hmcdg->Draw("2same");
    //format(hmcdg, 2);
    TLine *l1[bins];
    for (int r = 0; r < 3; r++)
    {
        l1[r] = new TLine(xdown, 0.75 + 0.25 * r, xup, 0.75 + 0.25 * r);
        format(l1[r]);
        l1[r]->Draw("same"); 
    }
    TLine *l2[bins];
    for (int d = 1; d < bins; d++)
    {
        l2[d - 1] = new TLine(xdown + d * bin_len, 0.5, xdown + d * bin_len, 1.5);
        format(l2[d - 1]);
        l2[d - 1]->Draw("same"); 
    }
    pad3->cd();
    gStyle->SetOptStat(0);
    TH1D *hc = (TH1D *)hdatad->Clone();
    hc->SetName("clone");
    hc->Draw("PY+");
    hc->GetYaxis()->SetTitleSize(0);

    c2->Print(Form("../qcd_pdf/%d/", year) + title[var] + "_" + cut_name + ".pdf");

    for (int i = 0; i < 3; i++)
        delete l1[i];
    for (int i = 0; i < bins - 1; i++)
        delete l2[i];
    delete hc;
    delete hdatad;
    delete hratio;
    delete hmc;
    delete hmcg;
    delete hmcdg;
    delete hstack;
    delete leg;
    delete pad1;
    delete pad2;
    delete pad3;
    delete c1;
    delete c2;
}