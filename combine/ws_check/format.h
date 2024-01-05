#include <TROOT.h>
#include <TLine.h>
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
#include <TKey.h>
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
void set_ratio(TH1D *h1, double a)
{
    for (int i = 1; i <= h1->GetNbinsX(); i++)
    {
        h1->SetBinContent(i, a);
        h1->SetBinError(i, 0);
    }
}
void format_text(TPaveText *lumi)
{
    lumi->SetFillColor(0);
    lumi->SetTextFont(42);
    lumi->SetTextSize(0.035);
    lumi->SetTextColor(13);
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
    c->SetLeftMargin(0.01);
    c->SetRightMargin(0.00);
    c->SetTopMargin(0.01);
    c->SetBottomMargin(0.01);
    c->SetFrameFillStyle(0);
    c->SetFrameBorderMode(0);
    c->SetFrameFillStyle(0);
    c->SetFrameBorderMode(0);
}
void format_pad(TPad *pad1, TPad *pad2)
{
    pad1->Draw();
    pad2->Draw();
    pad1->cd();
    pad1->SetTopMargin(0.1);
    pad1->SetBottomMargin(0.02);
    pad1->SetLeftMargin(0.14);
    // pad1->SetTopMargin(0);
    pad2->cd();
    // pad2->SetTopMargin(0);
    pad2->SetTopMargin(0.03);
    pad2->SetBottomMargin(0.45);
    pad2->SetLeftMargin(0.14);
}
void get_range_pad2(TH1D *h1[4], double *ranges, double range)
{
    float up = h1[0]->GetMaximum();
    float down = h1[0]->GetMinimum();
    for(int i = 1; i < 4; i++){
        if (up < h1[i]->GetMaximum())
            up = h1[i]->GetMaximum();
        if (down > h1[i]->GetMinimum())
            down = h1[i]->GetMinimum();
    }
    up = fabs(up);
    down = fabs(down);
    if (range != 0)
    {
        *ranges = range;
        return;
    }
    if (up == 0 && down == 0)
    {
        *ranges = 0.1;
        return;
    }
    if (up > down)
        *ranges = +1.3 * up;
    else
        *ranges = +1.3 * down;
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
    if (type == 0)
    { // ratio
        h1->SetLineStyle(7);
        h1->SetLineColor(1);
        h1->SetLineWidth(1);
    }
    if (type == 1)
    { // filled TH1D
        h1->SetFillColorAlpha(color, 0.3);
        h1->SetLineWidth(0);
    }
    else
    { // real TH1D(line)
        h1->SetLineWidth(2);
        h1->SetMarkerColor(color);
        h1->SetMarkerStyle(20);
        h1->SetMarkerSize(0.4);
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
double format_th_pad1(TH1D *h1, TString xtitle, int color)
{
    int ydivisions = 505;
    h1->SetLineColor(color);
    h1->SetLineWidth(2);
    h1->SetMarkerStyle(20);
    h1->SetMarkerSize(0.4);
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
    // h1->GetYaxis()->SetRangeUser(0.0, h1->GetMaximum()*1.4);
    h1->GetYaxis()->SetRangeUser(0.0, 80000);
    return h1->GetMaximum();
    // h1->GetYaxis()->SetRangeUser(0.0, 800000);
}
void format_line(TLine *l1)
{
    l1->SetLineStyle(9);
    l1->SetLineWidth(2);
    l1->SetLineColor(1);
}