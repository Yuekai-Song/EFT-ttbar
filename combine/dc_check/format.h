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
#include <TLatex.h>
#include <TPaveText.h>
#include "../binning.h"
using namespace std;

void seterror0(TH1D *h1)
{
    for (int i = 1; i <= h1->GetNbinsX(); i++)
    {
        h1->SetBinError(i, 0);
    }
}
void drawText(TLatex *latex, TString text, double posX, double posY, int font, int align, double size)
{
    latex->SetTextFont(font);
    latex->SetTextAlign(align);
    latex->SetTextSize(size);
    latex->DrawLatex(posX, posY, text);
}
void format_text(TPaveText *lumi, bool option)
{
    lumi->SetFillColor(0);
    lumi->SetTextFont(42);
    if (option)
    {
        lumi->SetTextColor(1);
        lumi->SetTextSize(0.045);
    }
    else
    {
        lumi->SetTextSize(0.028);
        lumi->SetTextColor(13);
    }
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

class DiCanvas
{
private:
    TLatex *latex;
    bool pad1_th = false, pad2_th = false; 
    TString xtitle, file_name;
    vector<vector<double>> xbins;
    TLegend *leg;
    double p2weight = 0.65 / 0.3;
    double off_y = 1.1;
    double off_x = 1.8;
    double lab_s, mar_l = 0.09;
    int ndiv, bins, ncuts, nnbins, net_num;
    int *div, *nbins;
    TLine *lratio[3];
    TLine **l1, **l2, **l3;
    TPaveText **ty, **tz, *tch, *textra; 
    TCanvas *c;
    TPad *pad1, *pad2;
    void format_canvas();
    void format_pad();

public:
    void draw_up(TH1D *h1, int color, TString legt, TString lumi = "", vector<TString> ycut = {}, vector<TString> zcut = {}, TString cut_name = "", TString extra = "");
    void draw_dn(TH1D *h1, TString style, double range, int color, int type);
    DiCanvas(TString Xtitle, vector<vector<double>> xbins, TString pdf_name);
    ~DiCanvas();
};
DiCanvas::~DiCanvas()
{
    pad1->cd();
    leg->Draw("Same");
    c->Print(file_name);
    for (int d = 0; d < ndiv; d++)
    {
        delete l1[d];
        delete l2[d];
    }
    for(int i = 0; i < net_num; i++)
        delete l3[i];
    for (int tex = 0; tex < ncuts; tex++)
    {
        delete tz[tex];
        delete ty[tex];
    }
    delete leg;
    for (int i = 0; i < 3; i++)
        delete lratio[i];
    delete latex;
    delete textra;
    delete tch;
    delete pad1;
    delete pad2;
    delete c;
}
DiCanvas::DiCanvas(TString Xtitle, vector<vector<double>> xbin, TString pdf_name)
{
    latex = new TLatex();
    latex->SetNDC();
    latex->SetTextAngle(0);
    latex->SetTextColor(kBlack);
    xtitle = Xtitle;
    file_name = pdf_name + ".pdf";
    xbins = xbin;
    ndiv = xbins.size() - 1;
    nnbins = xbins.size() + 1;
    ncuts = xbins.size();
    div = new int [ndiv];
    nbins = new int [nnbins];
    nbins[0] = 0;
    for (int i = 1; i < nnbins; i++)
        nbins[i] = nbins[i - 1] + xbins[i - 1].size() - 1;
    for (int i = 0; i < ndiv; i++)
        div[i] = nbins[i + 1];
    bins = nbins[nnbins - 1];
    double can_h;
    if (ncuts <= 4)
    {
        can_h = 700;
        lab_s = 0.11;
        mar_l = 0.14;
        off_x = 1.9;
    }
    else if (ncuts == 5)
    {
        can_h = 550.0 * 55 / bins;
        lab_s = 0.17;
        mar_l = 0.095;
        off_y = 0.85;
        off_x = 1.9;
    }
    else if (ncuts == 7)
    {
        can_h = 500;
        lab_s = 0.20;
        off_y = 0.8;
        off_x = 2.0;
    }
    c = new TCanvas("c1", "c1", 8, 30, 800, can_h);
    pad1 = new TPad("pad1", "This is pad1", 0.05, 0.32, 0.95, 0.97);
    pad2 = new TPad("pad2", "This is pad2", 0.05, 0.02, 0.95, 0.32);
    format_pad();
    format_canvas();
    
}
void DiCanvas::draw_up(TH1D *h1, int color, TString legt, TString lumi, vector<TString> ycut, vector<TString> zcut, TString cut_name, TString extra)
{
    pad1->cd();
    if (!pad1_th)
        h1->Draw();
    else
        h1->Draw("same");
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
    h1->GetYaxis()->SetTitleOffset(off_y);
    h1->GetXaxis()->SetLabelSize(0.0);
    h1->GetYaxis()->SetLabelSize(0.05);
    if (TString(h1->GetName()).Contains("ttbar"))
        h1->GetYaxis()->SetRangeUser(0.0, 80000);
    else
        h1->GetYaxis()->SetRangeUser(0.0, 3000);
    double high = h1->GetMaximum();
    if (!pad1_th)
    {
        leg = new TLegend(nbins[nnbins - 2] + 1, high * 0.55, nbins[nnbins - 1] - 1, high * 0.75, "", "br");
        format_leg(leg);
    }
    leg->AddEntry(h1, legt, "l");
    if (pad1_th)
        return;

    double ycut_posi = 0.95;
    if (zcut.size() == 0)
        ycut_posi = 1.05;
    ty = new TPaveText *[ncuts];
    tz = new TPaveText *[ncuts];
    for (int tex = 0; tex < ncuts; tex++)
    {
        double mid_row = (nbins[tex] + nbins[tex + 1]) / 2.0;
        double mid_col = high * 0.9;
        // cout << mid << endl;
        if (tex == ncuts / 2 - 1)
        {
            textra = new TPaveText(mid_row - 1, mid_col * 0.75, mid_row + 1, mid_col * 0.85);
            format_text(textra, 1);
            textra->AddText(extra);
            textra->Draw("same");
        }
        if (tex == ncuts / 2)
        {
            tch = new TPaveText(mid_row - 1, mid_col * 0.75, mid_row + 1, mid_col * 0.85);
            format_text(tch, 1);
            tch->AddText(cut_name);
            tch->Draw("same");
        }
        tz[tex] = new TPaveText(mid_row - 1, mid_col * 0.95, mid_row + 1, mid_col * 1.05);
        ty[tex] = new TPaveText(mid_row - 1, mid_col * (ycut_posi - 0.1), mid_row + 1, mid_col * ycut_posi);
        if (ycut.size() != 0)
        {
            format_text(ty[tex], 0);
            ty[tex]->AddText(ycut[tex]);
            ty[tex]->Draw("same");
        }
        if (zcut.size() != 0)
        {
            format_text(tz[tex], 0);
            tz[tex]->AddText(zcut[tex]);
            tz[tex]->Draw("same");
        }
    }
    l1 = new TLine *[ndiv];
    for (int d = 0; d < ndiv; d++)
    {
        l1[d] = new TLine(div[d], 0, div[d], high);
        format_line(l1[d]);
        pad1->cd();
        l1[d]->Draw("same");
    }
    c->cd();
    drawText(latex, lumi + " fb^{#minus1}", 0.93, 1 - 1.5 * pad1->GetTopMargin(), 42, 31, 0.033);
    drawText(latex, "CMS", 1.5 * mar_l, 1 - 1.5 * pad1->GetTopMargin(), 61, 11, 0.0375);
    pad1_th = true;
}

void DiCanvas::format_canvas()
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
void DiCanvas::format_pad()
{
    c->cd();
    pad1->Draw();
    pad2->Draw();
    pad1->cd();
    pad1->SetTopMargin(0.03);
    pad1->SetBottomMargin(0.02);
    pad1->SetLeftMargin(mar_l);
    pad1->SetRightMargin(0.0);
    // pad1->SetTopMargin(0);
    pad2->cd();
    // pad2->SetTopMargin(0);
    pad2->SetTopMargin(0.10);
    pad2->SetBottomMargin(0.45);
    pad2->SetLeftMargin(mar_l);
    pad2->SetRightMargin(0.0);
}
void DiCanvas::draw_dn(TH1D *h1, TString style, double range, int color, int type)
{
    int ydivisions = 505;
    pad2->cd();
    if (!pad2_th)
        h1->Draw(style);
    else
        h1->Draw(style + "Same");
    h1->SetLineColor(color);
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
    h1->GetXaxis()->SetTitleOffset(off_x);
    h1->GetYaxis()->SetTitleOffset(off_y / p2weight);
    h1->GetXaxis()->SetLabelSize(lab_s * 33 / bins);
    h1->GetYaxis()->SetLabelSize(0.05 * p2weight);
    h1->GetYaxis()->SetRangeUser(-range, range);
    set_th_lable(h1, xbins);
    if (pad2_th)
        return;
    for (int r = 0; r < 3; r++)
    {
        lratio[r] = new TLine(0, 0.5 * range * (r - 1), bins, 0.5 * range * (r - 1));
        lratio[r]->Draw("Same");
        format_line(lratio[r], 1);
    }
    l2 = new TLine*[ndiv];
    l3 = new TLine*[bins];
    for (int d = 0; d < ndiv; d++)
    {
        l2[d] = new TLine(div[d], -range, div[d], range);
        format_line(l2[d]);
        l2[d]->Draw("same");
    }
    net_num = 0;
    for (int d = 0; d < ndiv + 1; d++)
    {
        for(int bin = nbins[d] + 1; bin < nbins[d + 1]; bin++, net_num++)
        {
            l3[net_num] = new TLine(bin, -range, bin, range);
            format_line(l3[net_num], 1);
            l3[net_num]->Draw("same");
        } 
    }
    pad2_th = true;
}