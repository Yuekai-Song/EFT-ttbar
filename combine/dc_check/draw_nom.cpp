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
    c->SetBottomMargin(0.17);
    c->SetFrameFillStyle(0);
    c->SetFrameBorderMode(0);
    c->SetFrameFillStyle(0);
    c->SetFrameBorderMode(0);
}

void format_th(TH1D *h1, TString xtitle, int color)
{
    int ydivisions = 509;
    float up = h1->GetMaximum();
    float down = h1->GetMinimum();
    int bins = h1->GetNbinsX();
    up = fabs(up - 1);
    down = fabs(down - 1);
    h1->SetStats(kFALSE);
    h1->SetLineColor(color);
    h1->SetMarkerColor(color);
    h1->SetLineWidth(2);
    h1->GetXaxis()->SetTitle(xtitle);
    h1->GetYaxis()->SetTitle("Events");
    h1->SetTitle("");
    h1->GetXaxis()->CenterTitle();
    h1->GetYaxis()->CenterTitle();
    h1->GetYaxis()->SetNdivisions(ydivisions);
    h1->GetXaxis()->SetTitleSize(0.04);
    h1->GetYaxis()->SetTitleSize(0.04);
    h1->GetXaxis()->SetTitleOffset(2.2);
    h1->GetYaxis()->SetTitleOffset(2.0);
    h1->GetXaxis()->SetLabelSize(0.04);
    h1->GetYaxis()->SetLabelSize(0.04);
    return;
}

void format_line(TLine *l1)
{
    l1->SetLineStyle(9);
    l1->SetLineWidth(2);
    l1->SetLineColor(1);
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
void draw_pre(TString datacard_name, TString cutname, int year, vector<vector<double>> xbins, vector<double> ycuts)
{
    double high = 0;
    TString pros[] = {"ttbar_ci0000", "Eta", "STop", "WJets", "DYJets", "QCD"};
    TString outpath = "./nom_pdf/" + datacard_name + "/" + cutname + Form("_%d/", year);
    TString filename = "ttbar_" + cutname + Form("_%d.root", year);
    TFile *file = TFile::Open("../datacards/" + datacard_name  + "/original/" + filename);
    //file[3] = TFile::Open("../" + datacard_name + dc_types[2] + "/processed_bg_flat/" + filename);
    TH1D *hmc[5];
    int color[] = {2, 4, 8, 1, 6};
    TString xtitle = "M_{t#bar{t}}";
    //TString legend[4] = {"our", "their"};
    TString legend[6] = {"t#bar{t}", "#eta_{t}", "single t", "WJets", "DYan", "QCD"};

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
    
    TLine *l2[ndiv];
    TPaveText *t[ncuts];
    TCanvas *c2 = new TCanvas("c1", "c1", 8, 30, 650, 650);
    c2->cd();
    TLegend *leg = new TLegend(0.77, .65, 0.97, .80);
    format_leg(leg);
    format_canvas(c2);
    c2->cd();
    for (int c = 0; c < 6; c++)
    {
        hmc[c] = (TH1D *)file->Get(pros[c]);
        cout << hmc[c]->GetSumOfWeights() << endl;
        if (c == 0)
            hmc[c]->Draw();
        else
            hmc[c]->Draw("Same");
        format_th(hmc[c], xtitle, color[c]);
        if(high < hmc[c]->GetMaximum())
            high = hmc[c]->GetMaximum();
        leg->AddEntry(hmc[c], legend[c], "l");
    }
    hmc[0]->GetYaxis()->SetRangeUser(0, 80000);
    high = 80000;
    set_th_lable(hmc[0], xbins);
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
        l2[d] = new TLine(div[d], 0, div[d], high);
        format_line(l2[d]);
        l2[d]->Draw("same");
    }

    c2->Print(outpath + "nom.pdf");
    
    for (int d = 0; d < ndiv; d++)
        delete l2[d];
    for (int tex = 0; tex < ncuts; tex++)
        delete t[tex];
    for (int i = 0; i < 1; i++)
        delete hmc[i];
    delete leg;
    delete c2;
}
void draw_nom(TString datacard_name, TString cut_name, int year)
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
    draw_pre(datacard_name, cut_name, year, xbins, ycuts);
}