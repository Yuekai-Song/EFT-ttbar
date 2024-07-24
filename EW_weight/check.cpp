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
#include <fstream>
#include <iostream>
using namespace std;

void format_leg(TLegend *leg)
{
    // leg->SetNColumns(1);
    // leg->SetColumnSeparation(0.5);
    leg->SetEntrySeparation(0.0);
    leg->SetBorderSize(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.03);
    leg->SetLineColor(1);
    leg->SetLineStyle(1);
    leg->SetLineWidth(3.0);
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
    c->SetTopMargin(0.05);
    c->SetBottomMargin(0.15);
    c->SetFrameFillStyle(0);
    c->SetFrameBorderMode(0);
    c->SetFrameFillStyle(0);
    c->SetFrameBorderMode(0);
}
void format_th(TH1F *h1, TString xtitle, double min)
{
    int ydivisions = 505;
    h1->SetMarkerStyle(20);
    h1->SetMarkerSize(0.4);
    h1->SetStats(kFALSE);
    h1->GetXaxis()->SetTitle(xtitle);
    h1->SetTitle("");
    h1->GetXaxis()->CenterTitle();
    h1->GetYaxis()->CenterTitle();
    h1->GetYaxis()->SetNdivisions(ydivisions);
    h1->GetXaxis()->SetTitleSize(0.04);
    h1->GetYaxis()->SetTitleSize(0.04);
    h1->GetXaxis()->SetTitleOffset(1.0);
    h1->GetYaxis()->SetTitleOffset(1.50);
    h1->GetXaxis()->SetLabelSize(0.03);
    h1->GetYaxis()->SetLabelSize(0.03);
    h1->GetYaxis()->SetRangeUser(0.25, 0.35);
    // h1->GetYaxis()->SetRangeUser(0.0, 80000);
}
double model1(TH1F *h2[4], double y, double z, double k, int row)
{
    double cor[4];
    for (int i = 0; i < 4; i++)
    {
        cor[i] = 1 + h2[i]->GetBinContent(row);
    }
    double val = k * k * cor[2] + 0.298038 * cor[0] * y * (2.35528 + y) + cor[3] * (0.41333 - k * k - 0.298038 * (1.17764 + y) * (1.17764 + y) + (z - 1) * (z - 1)) - cor[1] * (z * z - 2 * z);
    return val - 1;
}
double model2(TH1F *h2[5], double y, double z, double k, int row)
{
    double cor[5];
    for (int i = 0; i < 5; i++)
    {
        cor[i] = 1 + h2[i]->GetBinContent(row);
    }
    double c1 = 0.5 * (cor[3] - 2 * cor[0] + cor[4]);
    double c2 = cor[3] - cor[1];
    double c3 = -cor[3] + cor[2];
    double c4 = 0.5 * (-3 * cor[3] + 4 * cor[0] - cor[4]);
    double c5 = cor[1];
    return (c1 * y * y + c2 * (z - 1) * (z - 1) + c3 * k * k + c4 * y + c5 - 1);
    // return c4/c1;
}
double model3(TH1F *h2[6], double y, double z, double k, int row)
{
    double cor[6];
    for (int i = 0; i < 6; i++)
    {
        cor[i] = 1 + h2[i]->GetBinContent(row);
    }
    double c1 = 0.5 * (cor[3] - 2 * cor[0] + cor[4]);
    double c2 = 0.5 * (cor[3] - 2 * cor[1] + cor[5]);
    double c3 = -cor[3] + cor[2];
    double c4 = 0.5 * (-3 * cor[3] + 4 * cor[0] - cor[4]);
    double c5 = 0.5 * (-3 * cor[3] + 4 * cor[1] - cor[5]);
    double c6 = cor[3];
    //cout << c4 / c1 << endl;
    return (c1 * y * y + c2 * z * z + c3 * k * k + c4 * y + c5 * z + c6 - 1);
    // return c4/c1;
}
void check()
{
    TString filename[] = {"ci0100.root", "ci0010.root", "ci0001.root", "ci0000.root", "ci0200.root", "ci0020.root"};
    TString h_name[] = {"h100", "h010", "h001", "h000", "h200", "h020"};
    TString id[] = {"id7", "id13", "id10"};
    TString varname[] = {"M_{t#bar{t}} [GeV]", "#Deltay_{t#bar{t}}", "p_{T}^{t} [GeV]"};
    TString ytitle[] = {"(d#delta#sigma/dM_{t#bar{t}})/(d#sigma_{Lo}/dM_{t#bar{t}})",
						"(d#delta#sigma/d#Deltay_{t#bar{t}})/(d#sigma_{LO}/d#Deltay_{t#bar{t}})",
                        "(d#delta#sigma/dp_{T}^{t})/(d#sigma_{LO}/dp_{T}^{t})"};
    TString name[] = {"Mtt", "deltay", "top_pt"};
    double loc[] = {0.3, 0.3, 0.3};
    double min[] = {-0.05, -0.05, -0.3};
    TH1F *h2[6], *hc[7], *hist[7];
    TFile *infile[6], *cfile[7];
    int value[][3] = {{0,5,0},{-1,0,0}};

    TString check_name;
    int color[] = {2, 46, 9, 29, 8, kYellow, 93};
    for (int var = 0; var < 3; var++)
    {
        for (int i = 0; i < 6; i++)
        {
            infile[i] = TFile::Open("./EW_files/" + filename[i]);
            h2[i] = (TH1F *)infile[i]->Get(id[var]);
            h2[i]->SetName(h_name[i]);
        }
        auto c2 = new TCanvas("c2", "c2", 8, 30, 650, 650);
        format_canvas(c2);
        c2->cd();
        TLegend *leg = new TLegend(loc[var], .75, loc[var] + 0.40, .95, "(C_{#varphit}, C_{t#varphi}, C_{t#varphi}^{I})");
        leg->SetNColumns(3);
		leg->SetColumnSeparation(0.03);
        format_leg(leg);
        for (int i = 0; i < 1; i++)
        {
            check_name = Form("ci0%d%d%d", value[i][0], value[i][1], value[i][2]);
            TString legname = Form("(%d, %d, %d)", int(value[i][0]), int(value[i][1]), int(value[i][2]));
            cfile[i] = TFile::Open("./EW_files/" + check_name + ".root");
            hc[i] = (TH1F *)cfile[i]->Get(id[var])->Clone();
            hc[i]->SetName(Form("hc_%d", i));
            hist[i] = (TH1F *)hc[i]->Clone();
            hist[i]->SetName(Form("hist_%d", i));
            for (int j = 0; j < hc[i]->GetNbinsX() + 2; j++)
            {
                hist[i]->SetBinContent(j, model3(h2, value[i][0], value[i][1], value[i][2], j));
                hist[i]->SetBinError(j, 0);
            }
            c2->cd();
            if (i == 0)
            {
                hc[i]->Draw("hist");
                hc[i]->GetYaxis()->SetTitle(ytitle[var]);
                format_th(hc[i], varname[var], min[var]);
            }
            else
                hc[i]->Draw("histsame");
            hc[i]->SetLineColor(color[i]);
            hc[i]->SetLineWidth(2);
            hist[i]->Draw("Psame");
            hist[i]->SetMarkerSize(0.5);
            hist[i]->SetMarkerStyle(21);
            leg->AddEntry(hc[i], legname, "l");
        }
        leg->AddEntry(hist[0], "model", "P");
        c2->cd();
        leg->Draw("same");
        c2->Print("./pdf/" + name[var] + ".pdf");
        delete leg;
        for (int i = 0; i < 1; i++)
        {
            delete hist[i];
            delete hc[i];
            cfile[i]->Close();
        }
        for (int i = 0; i < 6; i++)
        {
            delete h2[i];
            infile[i]->Close();
        }
        delete c2;
    }
}