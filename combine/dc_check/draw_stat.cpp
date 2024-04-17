#include <TROOT.h>
#include <TLine.h>
#include <TStyle.h>
#include <Math/PdfFuncMathCore.h>
#include <TFile.h>
#include <TChain.h>
#include <TLorentzVector.h>
#include <TGraphAsymmErrors.h>
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
using namespace std;
double p2weight = 0.65 / 0.3;
void seterror0(TH1D *h1)
{
    for (int i = 1; i <= h1->GetNbinsX(); i++)
    {
        h1->SetBinError(i, 0);
    }
}
void format_text(TPaveText *lumi)
{
    lumi->SetFillColor(0);
    lumi->SetTextFont(42);
    lumi->SetTextSize(0.035);
    lumi->SetTextColor(1);
    lumi->SetBorderSize(0);
}
void format_leg(TLegend *leg)
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
void format_canvas(TCanvas *c)
{
    c->SetFillColor(0);
    c->SetBorderMode(0);
    c->SetBorderSize(2);
    c->SetTickx(1);
    c->SetTicky(1);
    c->SetLeftMargin(0.16);
    c->SetRightMargin(0.05);
    c->SetTopMargin(0.05);
    c->SetBottomMargin(0.15);
    c->SetFrameFillStyle(0);
    c->SetFrameBorderMode(0);
    c->SetFrameFillStyle(0);
    c->SetFrameBorderMode(0);
}
double format_th_pad1(TH1D *h1, TString xtitle, int color)
{
    int ydivisions = 505;
    h1->SetLineColor(color);
    h1->SetLineWidth(2);
    h1->SetMarkerStyle(20);
    h1->SetMarkerColor(color);
    h1->SetMarkerSize(0);
    h1->SetStats(kFALSE);
    h1->GetXaxis()->SetTitle(xtitle);
    h1->GetYaxis()->SetTitle("Events/bin");
    h1->SetTitle("");
    h1->GetXaxis()->CenterTitle();
    h1->GetYaxis()->CenterTitle();
    h1->GetYaxis()->SetNdivisions(ydivisions);
    h1->GetXaxis()->SetTitleSize(0.0);
    h1->GetYaxis()->SetTitleSize(0.06);
    h1->GetXaxis()->SetTitleOffset(1.0);
    h1->GetYaxis()->SetTitleOffset(1.20);
    h1->GetXaxis()->SetLabelSize(0.0);
    h1->GetYaxis()->SetLabelSize(0.05);
    h1->GetYaxis()->SetRangeUser(0.0, h1->GetMaximum() * 1.3);
    //h1->GetYaxis()->SetRangeUser(0.0, 80000);
    return h1->GetMaximum();
}
void set_th_lable(TH1D *h1, vector<vector<double>> xbins)
{
    double nbin = 0;
    for (int i = 0; i < xbins.size(); i++)
    {
        for (int j = nbin; j < nbin + xbins[i].size() - 1; j++)
            h1->GetXaxis()->SetBinLabel(j + 1, TString::Format("%.0f-%.0f", xbins[i][j - nbin], xbins[i][j - nbin + 1]));
        nbin += xbins[i].size() - 1;
    }
    h1->LabelsDeflate("X");
    h1->LabelsDeflate("Y");
    h1->LabelsOption("v");
}
void format_th_pad2(TH1D *h1, TString xtitle, double range, int color, int type, vector<vector<double>> xbins)
{
    int ydivisions = 505;
    int bins = h1->GetNbinsX();
    h1->SetLineColor(color);
    if (type == 1)
    { // real TH1D(line)
        h1->SetLineWidth(2);
        h1->SetMarkerColor(color);
        h1->SetMarkerStyle(20);
        h1->SetMarkerSize(0.4);
    }
    else
    { // ratio
        h1->SetLineStyle(3);
        h1->SetLineWidth(1);
    }
    seterror0(h1);
    h1->SetStats(kFALSE);
    h1->GetXaxis()->SetTitle(xtitle);
    h1->GetYaxis()->SetTitle("Ratio");
    h1->SetTitle("");
    h1->GetXaxis()->CenterTitle();
    h1->GetYaxis()->CenterTitle();
    h1->GetYaxis()->SetNdivisions(ydivisions);
    h1->GetXaxis()->SetTitleSize(0.12);
    h1->GetYaxis()->SetTitleSize(0.06 * p2weight);
    h1->GetXaxis()->SetTitleOffset(1.8);
    h1->GetYaxis()->SetTitleOffset(1.1 / p2weight);
    h1->GetXaxis()->SetLabelSize(0.08 * 33 / bins);
    h1->GetYaxis()->SetLabelSize(0.05 * p2weight);
    h1->GetYaxis()->SetRangeUser(-range, range);
    set_th_lable(h1, xbins);
}
void format_pad(TPad *pad1, TPad *pad2)
{
    pad1->Draw();
    pad2->Draw();
    pad1->cd();
    pad1->SetTopMargin(0.1);
    pad1->SetBottomMargin(0.02);
    pad1->SetLeftMargin(0.15);
    // pad1->SetTopMargin(0);
    pad2->cd();
    // pad2->SetTopMargin(0);
    pad2->SetTopMargin(0.03);
    pad2->SetBottomMargin(0.45);
    pad2->SetLeftMargin(0.15);
}
void format_line(TLine *l1, bool is_net = false)
{
    l1->SetLineStyle(9);
    if (is_net)
    {
        l1->SetLineWidth(1);
        l1->SetLineStyle(3);
    }
    else
    {
        l1->SetLineWidth(2);
        l1->SetLineStyle(9);
    }
    l1->SetLineColor(1);
}

void set_ud(TH1D* h1, TH1D* h1_up, TH1D* h1_dn)
{
    for (int i = 1; i <= h1->GetNbinsX(); i++)
    {
        h1_up->SetBinContent(i, h1->GetBinContent(i) + h1->GetBinError(i));
        h1_dn->SetBinContent(i, h1->GetBinContent(i) - h1->GetBinError(i));
    }
}
void draw_pre(TString datacard_name, TString cutname, int year, vector<vector<double>> xbins, vector<double> ycuts, int type)
{
    double high = 0;
    TString pros[] = {"EW_no", "STop", "WJets", "DYJets", "QCD"};
    int begin[3] = {0, 1, 0};
    int end[3] = {1, 4, 4};
    TString name[3] = {"Sig", "Back", "SandB"}; 
    TString outpath = "./stat_pdf/" + datacard_name + "/" + cutname + Form("_%d/", year);
    TString filename = "ttbar_" + cutname + Form("_%d.root", year);
    TFile *file = TFile::Open("../" + datacard_name  + "/original/" + filename);
    //file[3] = TFile::Open("../" + datacard_name + dc_types[2] + "/processed_bg_flat/" + filename);
    TH1D *hmc, *hmc_up, *hmc_dn;
    TH1D *hd_up, *hd_dn;
    int color[] = {1, 2, 4, 8, 1, 6};
    TString xtitle = "M_{t#bar{t}}";
    //TString legend[4] = {"our", "their"};

    const int ndiv = xbins.size() - 1;
    const int nnbins = xbins.size() + 1;
    const int ncuts = ycuts.size();
    int div[ndiv];
    int nbins[nnbins];
    nbins[0] = 0;
    for (int i = 1; i < nnbins; i++)
        nbins[i] = nbins[i - 1] + xbins[i - 1].size() - 1;
    for (int i = 0; i < ndiv; i++)
        div[i] = nbins[i + 1];
    int bins = nbins[nnbins - 1];

    TString cut[ncuts];
    cut[0] = Form("|#Deltay| < %.1f", ycuts[1]);
    cut[ncuts - 1] = Form("|#Deltay| > %.1f", ycuts[ncuts - 1]);
    for (int i = 1; i < ncuts - 1; i++)
        cut[i] = Form("%.1f< |#Deltay| < %.1f", ycuts[i], ycuts[i + 1]);
    
    TLine *l1[ndiv], *l2[ndiv], *l3[bins];
    TPaveText *t[ncuts];
    TCanvas *c2 = new TCanvas("c1", "c1", 8, 30, 650, 650);
    TPad *pad1 = new TPad("pad1", "This is pad1", 0.05, 0.32, 0.95, 0.97);
    TPad *pad2 = new TPad("pad2", "This is pad2", 0.05, 0.02, 0.95, 0.32);
    c2->cd();
    format_pad(pad1, pad2);
    TLegend *leg = new TLegend(0.77, .62, 0.97, .77);
    format_leg(leg);
    format_canvas(c2);
    pad1->cd();
    hmc = (TH1D *)file->Get(pros[begin[type]]);
    for (int c = begin[type] + 1; c < end[type]; c++)
        hmc->Add((TH1D *)file->Get(pros[c]));        
    //cout << hmc->GetSumOfWeights() << endl;
    hmc->Draw();
    hmc_up = (TH1D *)hmc->Clone();
    hmc_dn = (TH1D *)hmc->Clone();
    hmc_up->SetName("up");
    hmc_dn->SetName("dn");
    set_ud(hmc, hmc_up, hmc_dn);
    hmc_up->Draw("Same");
    hmc_dn->Draw("Same");
    format_th_pad1(hmc, xtitle, color[0]);
    format_th_pad1(hmc_up, xtitle, color[1]);
    format_th_pad1(hmc_dn, xtitle, color[2]);
    high = hmc->GetMaximum();
    hmc->GetYaxis()->SetRangeUser(0, high * 1.3);
    high *= 1.3;
    leg->AddEntry(hmc, "nom", "l");
    leg->AddEntry(hmc_up, "StatUp", "l");
    leg->AddEntry(hmc_dn, "StatDn", "l");
    leg->Draw("Same");

    for (int tex = 0; tex < ncuts; tex++)
    {
        double mid_row = (nbins[tex] + nbins[tex + 1]) / 2.0;
        double mid_col = high * 0.9;
        //cout << mid << endl;
        t[tex] = new TPaveText(mid_row - 1, mid_col * 0.95, mid_row + 1, mid_col * 1.05);
        format_text(t[tex]);
        t[tex]->AddText(cut[tex]);
        t[tex]->Draw("same");
    }

    for (int d = 0; d < ndiv; d++)
    {
        l1[d] = new TLine(div[d], 0, div[d], high);
        format_line(l1[d]);
        l1[d]->Draw("same");
    }
    pad2->cd();
    hd_up = (TH1D *)hmc_up->Clone();
    hd_up->SetName("hd_up");
    hd_up->Add(hmc, -1);
    hd_up->Divide(hmc);
    hd_dn = (TH1D *)hmc_dn->Clone();
    hd_dn->SetName("hd_dn");
    hd_dn->Add(hmc, -1);
    hd_dn->Divide(hmc);
    hd_up->Draw("h");
    hd_dn->Draw("hSame");
    double range = hd_up->GetMaximum() * 1.3;
    format_th_pad2(hd_up, xtitle, range, color[1], 1, xbins);
    format_th_pad2(hd_dn, xtitle, range, color[2], 1, xbins);

    TLine *lratio[3];
    for (int r = 0; r < 3; r++)
    {
        lratio[r] = new TLine(0, 0.5 * range * (r - 1), bins, 0.5 * range * (r - 1));
        lratio[r]->Draw("Same");
        format_line(lratio[r], 1);
    }
    for (int d = 0; d < ndiv; d++)
    {
        l2[d] = new TLine(div[d], -range, div[d], range);
        format_line(l2[d]);
        l2[d]->Draw("same");
    }
    int net_num = 0;
    for (int d = 0; d < ndiv + 1; d++)
    {
        for(int bin = nbins[d] + 1; bin < nbins[d + 1]; bin++, net_num++)
        {
            l3[net_num] = new TLine(bin, -range, bin, range);
            format_line(l3[net_num], 1);
            l3[net_num]->Draw("same");
        } 
    }
    c2->Print(outpath + name[type] + ".pdf");

    for (int d = 0; d < ndiv; d++)
    {
        delete l1[d];
        delete l2[d];
    }
    for(int i = 0; i < net_num; i++)
        delete l3[i];
    for (int tex = 0; tex < ncuts; tex++)
        delete t[tex];
    delete hmc;
    delete hmc_up;
    delete hmc_dn;
    delete hd_up;
    delete hd_dn;
    delete leg;
    delete c2;
}
void draw_stat(TString datacard_name, TString cut_name, int year)
{
    vector<double> ycuts;
    vector<vector<double>> xbins;
    if (datacard_name.Contains("2cuts")) {
        ycuts = {0.0, 1.4};
        xbins = {{300,340,360,380,400,420,440,460,480,500,520,540,570,600,640,700,3000},
                                   {300,450,500,570,630,700,820,3000}};
    }
    else
    {
        ycuts = {0.0, 0.4, 1.0, 2.0};
        xbins = {{0,340,380,420,460,500,600,3000}, {0,350,400,450,500,550,600,700,800,3000}, 
                               {0,450,500,550,600,650,700,800,1000,3000}, {0,650,700,750,800,900,1000,1200,3000}};
    }
    for (int i = 0; i < 3; i++)
        draw_pre(datacard_name, cut_name, year, xbins, ycuts, i);
}