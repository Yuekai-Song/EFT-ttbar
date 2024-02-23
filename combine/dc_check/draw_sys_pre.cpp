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
    if (up == 0.1 || down == 0.1)
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
    if (type == 1)
    { // filled TH1D
        h1->SetFillColorAlpha(color, 0.3);
        h1->SetLineWidth(0);
    }
    else if(type == 2)
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
    // h1->GetYaxis()->SetRangeUser(0.0, h1->GetMaximum()*1.4);
    if (TString(h1->GetName()).Contains("ttbar"))
        h1->GetYaxis()->SetRangeUser(0.0, 80000);
    else
        h1->GetYaxis()->SetRangeUser(0.0, 3000);
    return h1->GetMaximum();
    // h1->GetYaxis()->SetRangeUser(0.0, 800000);
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
void draw_pre(TH1D *hsm, TH1D *hmc[4], TString sys, TString pdf_name, double range0, vector<vector<double>> xbins, vector<double> ycuts)
{
    double range, high;
    TString name[] = {"Up", "Down"};
    int color[] = {2, 4};
    TString xtitle = "M_{t#bar{t}}";
    TString legend[] = {"#theta = +1#sigma", "#theta = -1#sigma"};
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
        cut[i] = Form("%.1f < |#Deltay| < %.1f", ycuts[i], ycuts[i + 1]);

    TLine *l1[ndiv], *l2[ndiv], *l3[bins];
    TPaveText *t[ncuts];
    TH1D *hd[4];
    TCanvas *c2 = new TCanvas("c1", "c1", 8, 30, 650, 650);
    TPad *pad1 = new TPad("pad1", "This is pad1", 0.05, 0.32, 0.95, 0.97);
    TPad *pad2 = new TPad("pad2", "This is pad2", 0.05, 0.02, 0.95, 0.32);
    c2->cd();
    format_pad(pad1, pad2);
    //TLegend *leg = new TLegend(0.75, .55, 0.85, .70);
    format_canvas(c2);
    for (int i = 0; i < 4; i++)
    {
        hd[i] = (TH1D *)hmc[i]->Clone();
        hd[i]->SetName(Form("hd_%d", i));
        hd[i]->Add(hsm, -1);
        hd[i]->Divide(hsm);
    }
    pad1->cd();
    hsm->Draw();
    high = format_th_pad1(hsm, xtitle, 1);

    TLegend *leg = new TLegend(nbins[nnbins - 2] + 1, high * 0.55, nbins[nnbins - 1] - 1, high * 0.75, "", "br");
    format_leg(leg);
    leg->AddEntry(hsm, "SM case", "l");
    for (int i = 0; i < 2; i++)
    {
        hmc[i]->Draw("Same");
        format_th_pad1(hmc[i], xtitle, color[i]);
        leg->AddEntry(hmc[i], legend[i], "l");
    }
    leg->Draw("Same");

    for (int d = 0; d < ndiv; d++)
    {
        l1[d] = new TLine(div[d], 0, div[d], high);
        format_line(l1[d]);
        pad1->cd();
        l1[d]->Draw("same");
    }
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

    pad2->cd();
    get_range_pad2(hd, &range, range0);

    for (int i = 0; i < 2; i++)
    {
        if (i == 0)
            hd[i]->Draw("Ph");
        else
            hd[i]->Draw("PhSame");
        format_th_pad2(hd[i], xtitle, range, color[i], 2, xbins);
    }
    for (int i = 0; i < 2; i++)
    {
        hd[2 + i]->Draw("hSame");
        format_th_pad2(hd[2 + i], xtitle, range, color[i], 1, xbins);
    }

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
    c2->Print("./sys_pdf/" + pdf_name + ".pdf");
    for (int d = 0; d < ndiv; d++)
    {
        delete l1[d];
        delete l2[d];
    }
    for(int i = 0; i < net_num; i++)
        delete l3[i];
    for (int tex = 0; tex < ncuts; tex++)
        delete t[tex];
    delete leg;

    for (int i = 0; i < 3; i++)
        delete lratio[i];
    for (int i = 0; i < 4; i++)
        delete hd[i], hmc[i];
    delete pad1;
    delete pad2;
    delete c2;
}
