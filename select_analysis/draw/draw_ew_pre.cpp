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
    c->SetLeftMargin(0.18);
    c->SetRightMargin(0.05);
    c->SetTopMargin(0.01);
    c->SetBottomMargin(0.05);
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
    pad1->SetLeftMargin(0.15);
    // pad1->SetTopMargin(0);
    pad2->cd();
    // pad2->SetTopMargin(0);
    pad2->SetTopMargin(0.03);
    pad2->SetBottomMargin(0.45);
    pad2->SetLeftMargin(0.15);
}
void format_th_pad2(TH1D *h1, TString xtitle, double *low, double *high)
{
    int ydivisions = 505;
    float up = h1->GetMaximum();
    float down = h1->GetMinimum();
    up = fabs(up - 1);
    down = fabs(down - 1);
    h1->SetMarkerStyle(20);
    h1->SetMarkerSize(0.4);
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
    h1->GetXaxis()->SetLabelSize(0.08);
    h1->GetYaxis()->SetLabelSize(0.05 * p2weight);
    h1->GetYaxis()->SetRangeUser(0.9, 1.1);
    *low = 0.9;
    *high = 1.1;
    return;
}
double format_th_pad1(TH1D *h1, TString xtitle)
{
    int ydivisions = 505;
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
    h1->GetYaxis()->SetRangeUser(0.0, h1->GetMaximum() * 1.4);
    return h1->GetMaximum();
    // h1->GetYaxis()->SetRangeUser(0.0, 800000);
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
void model(TH1D *h1, TH1D *h2[5], double y, double z, double k)
{
    double cor[5];
    double c1, c2, c3, c4, c5;
    for (int bin = 0; bin < h1->GetNbinsX(); bin++)
    {
        for (int i = 0; i < 5; i++)
        {
            cor[i] = h2[i]->GetBinContent(bin + 1);
        }
        c1 = 0.5 * (cor[3] - 2 * cor[0] + cor[4]);
        c2 = cor[3] - cor[1];
        c3 = -cor[3] + cor[2];
        c4 = 0.5 * (-3 * cor[3] + 4 * cor[0] - cor[4]);
        c5 = cor[1];
        h1->SetBinContent(bin + 1, c1 * y * y + c2 * (z - 1) * (z - 1) + c3 * k * k + c4 * y + c5);
        h1->SetBinError(bin + 1, 0);
    }
}
void draw_pre(TString cutname, int year, TString sys, int type, double value[5], int index[5], bool rel, vector<vector<double>> xbins, vector<double> ycuts)
{
    double low, high;
    TString process[] = {"ttbar_ci0100", "ttbar_ci0010", "ttbar_ci0001", "ttbar_ci0000", "ttbar_ci0200"};
    TString inpath = "../../combine/";
    TString outpath = "../ew_pdf/" + cutname + Form("_%d/", year);
    TString filename = "ttbar_" + cutname + Form("_%d.root", year);
    TFile *file = TFile::Open(inpath + "datacard/original/" + filename);
    TH1D *hmc1[5], *h1[5], *hd1[5];
    TH1D *hno, *hd;
    int color[] = {2, 1, 8, 4, 6};

    TString xtitle = "M_{t#bar{t}}";
    TString legend[5];
    TString legend_l[3] = {"Re(C_{u#varphi}) = ", "Im(C_{u#varphi}) = ", "C_{#varphiu}) = "};
    TString name[3] = {"Kappa", "Kappat", "Ztt"};
    for (int i = 0; i < 5; i++)
        legend[i] = legend_l[type] + Form("%.2f", value[i]);
    // TString legend[] = {"#kappa = 1", "#kappa = 0.5", "#kappa = 0", "#kappa = 1.5", "#kappa = 2.0"};

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
    int bins = nbins[nnbins];
    TString cut[ncuts];
    cut[0] = Form("|#Deltay|<%.1f", ycuts[1]);
    cut[ncuts - 1] = Form("|#Deltay|>%.1f", ycuts[ncuts - 2]);
    for (int i = 1; i < ncuts - 1; i++)
        cut[i] = Form("%.1f<|#Deltay|<%.1f", ycuts[i - 1], ycuts[i]);
    TLine *l1[ndiv], *l2[ndiv];
    TPaveText *t[ncuts];

    for (int c = 0; c < 5; c++)
    {
        h1[c] = (TH1D *)file->Get(process[c] + sys);
    }
    if (rel == true)
        hno = (TH1D *)file->Get("EW_no");
    else if (sys != "")
        hno = (TH1D *)file->Get("ttbar_ci0000");
    else
        hno = (TH1D *)h1[3]->Clone();
    hno->SetName("hno");
    hd = (TH1D *)hno->Clone();
    hd->SetName("hd");
    hd->Divide(hno);
    seterror0(hd);
    TCanvas *c2 = new TCanvas("c1", "c1", 8, 30, 650, 650);
    TPad *pad1 = new TPad("pad1", "This is pad1", 0.05, 0.32, 0.95, 0.97);
    TPad *pad2 = new TPad("pad2", "This is pad2", 0.05, 0.02, 0.95, 0.32);
    c2->cd();
    format_pad(pad1, pad2);
    TLegend *leg = new TLegend(0.70, .55, 0.95, .75);
    format_leg(leg);
    format_canvas(c2);
    for (int i = 0; i < 5; i++)
    {
        hmc1[i] = (TH1D *)h1[0]->Clone();
        hmc1[i]->SetName(Form("hmc_%d", i));
        if (type == 0)
            model(hmc1[i], h1, 0, value[i], 0);
        else if (type == 1)
            model(hmc1[i], h1, 0, 0, value[i]);
        else if (type == 2)
            model(hmc1[i], h1, value[i], 0, 0);
        hd1[i] = (TH1D *)hmc1[i]->Clone();
        hd1[i]->SetName(Form("hd_%d", i));
        hd1[i]->Divide(hno);
        cout << hmc1[i]->GetSumOfWeights() << endl;
    }

    pad1->cd();
    hno->Draw("hist");
    double high1 = format_th_pad1(hno, xtitle);
    hno->SetLineColor(28);
    hno->SetLineWidth(2);
    for (int i = 0; i < 5; i++)
    {
        hmc1[i]->Draw("histSame");
        hmc1[i]->SetLineColor(color[i]);
        hmc1[i]->SetLineWidth(2);
    }
    for (int i = 0; i < 5; i++)
        leg->AddEntry(hmc1[index[i]], legend[index[i]], "l");
    leg->Draw("Same");

    for (int d = 0; d < ndiv; d++)
    {
        l1[d] = new TLine(div[d], 0, div[d], high1);
        format_line(l1[d]);
        pad1->cd();
        l1[d]->Draw("same");
    }
    for (int tex = 0; tex < ncuts; tex++)
    {
        t[tex] = new TPaveText(0.2 + 0.19 * tex, 0.75, 0.25 + 0.19 * tex, 0.85, "NDC");
        format_text(t[tex]);
        t[tex]->AddText(cut[tex]);
        t[tex]->Draw("same");
    }

    pad2->cd();
    hd->Draw("hist");
    hd->SetLineColor(28);
    format_th_pad2(hd, xtitle, &low, &high);
    set_th_lable(hd, xbins);
    hd->SetLineStyle(9);
    hd->SetLineWidth(2);
    for (int i = 0; i < 5; i++)
    {
        hd1[i]->Draw("histSame");
        hd1[i]->SetLineColor(color[i]);
        hd1[i]->SetLineWidth(2);
        // seterror0(hd1[i]);
    }

    for (int d = 0; d < ndiv; d++)
    {
        l2[d] = new TLine(div[d], low, div[d], high);
        format_line(l2[d]);
        l2[d]->Draw("same");
    }
    sys.ReplaceAll("Up", "");
    c2->Print(outpath + name[type] + sys + ".pdf");
    for (int d = 0; d < ndiv; d++)
    {
        delete l1[d];
        delete l2[d];
    }
    for (int tex = 0; tex < ncuts; tex++)
        delete t[tex];
    for (int i = 0; i < 5; i++)
    {
        delete h1[i];
    }
    for (int i = 0; i < 5; i++)
    {
        delete hmc1[i];
        delete hd1[i];
    }
    delete hno;
    delete hd;
    delete leg;
    delete pad1;
    delete pad2;
    delete c2;
}