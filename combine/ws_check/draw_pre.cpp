#include "format.h"
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TFile.h>
#include <TChain.h>
#include <TMath.h>
#include <TLegend.h>
#include <TF1.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <string.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include <fstream>
#include <iostream>
#include <RooFit.h>
#include <RooRealVar.h>
#include <RooSimultaneous.h>
#include <RooWorkspace.h>
#include <RooCategory.h>
void get_th(TH1D *&h1, TFile *file, vector<TString> var_name, vector<double> vals, int ch_index)
{
    TH1D *h1_b;
    RooWorkspace *w = (RooWorkspace *)file->Get("w");
    RooSimultaneous *sbpdf = (RooSimultaneous *)w->pdf("model_s");
    RooSimultaneous *bpdf = (RooSimultaneous *)w->pdf("model_b");

    RooCategory *chan = w->cat("CMS_channel");
    RooRealVar *mass_tt = w->var("CMS_th1x");
    RooRealVar *vars[var_name.size()];
    for (int i = 0; i < var_name.size(); i++)
    {
        vars[i] = w->var(var_name[i]);
        vars[i]->setVal(vals[i]);
    }
    chan->setIndex(ch_index);
    RooAbsPdf *sbcatpdf = sbpdf->getPdf(chan->getLabel());
    RooAbsPdf *bcatpdf = bpdf->getPdf(chan->getLabel());
    h1 = (TH1D *)sbcatpdf->createHistogram("CMS_th1x");
    h1->SetDirectory(0);
    h1_b = (TH1D *)bcatpdf->createHistogram("CMS_th1x");
    h1->Add(h1_b, -1);
    //cout << h1->GetSumOfWeights() << endl;
    delete h1_b;
}
void draw_pre(TH1D *hsm, TH1D *hmc[4], TString legend[4], TString pdf_name, double range0, vector<vector<double>> xbins, vector<double> ycuts)
{
    double range, high;
    int color[] = {2, 4, 8, 6};
    TString xtitle = "M_{t#bar{t}}";
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
        cut[i] = Form("%.1f < |#Deltay| < %.1f", ycuts[i], ycuts[i]);

    TLine *l1[ndiv], *l2[ndiv];
    TPaveText *t[ncuts];
    TH1D *hd[4], *hratio[3];
    TCanvas *c2 = new TCanvas("c1", "c1", 8, 30, 650, 650);
    TPad *pad1 = new TPad("pad1", "This is pad1", 0.05, 0.32, 0.95, 0.97);
    TPad *pad2 = new TPad("pad2", "This is pad2", 0.05, 0.02, 0.95, 0.32);
    c2->cd();
    format_pad(pad1, pad2);
    TLegend *leg = new TLegend(0.70, .55, 0.95, .70);
    format_leg(leg);
    format_canvas(c2);
    for (int i = 0; i < 4; i++)
    {
        hd[i] = (TH1D *)hmc[i]->Clone();
        hd[i]->SetName(Form("hd_%d", i));
        hd[i]->Add(hsm, -1);
        hd[i]->Divide(hsm);
    }
    pad1->cd();
    hsm->Draw("hist");
    high = format_th_pad1(hsm, xtitle, 1);
    leg->AddEntry(hsm, "SM case", "l");
    for (int i = 0; i < 4; i++)
    {
        hmc[i]->Draw("histSame");
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
        // cout << mid << endl;
        t[tex] = new TPaveText(mid_row - 1, mid_col * 0.95, mid_row + 1, mid_col * 1.05);
        format_text(t[tex]);
        t[tex]->AddText(cut[tex]);
        t[tex]->Draw("same");
    }

    pad2->cd();
    get_range_pad2(hd, &range, range0);
    for (int r = 0; r < 3; r++)
    {
        hratio[r] = new TH1D(Form("ratio%d", r), "", bins, 0, bins);
        set_ratio(hratio[r], 0.5 * range * (r - 1));
        if (r == 0)
            hratio[r]->Draw("L");
        else
            hratio[r]->Draw("LSame");
        format_th_pad2(hratio[r], xtitle, range, 1, 0, xbins);
    }
    for (int i = 0; i < 4; i++)
    {
        hd[i]->Draw("PhSame");
        format_th_pad2(hd[i], xtitle, range, color[i], 2, xbins);
    }

    for (int d = 0; d < ndiv; d++)
    {
        l2[d] = new TLine(div[d], -range, div[d], range);
        format_line(l2[d]);
        l2[d]->Draw("same");
    }
    c2->Print(pdf_name + ".pdf");
    for (int d = 0; d < ndiv; d++)
    {
        delete l1[d];
        delete l2[d];
    }
    for (int tex = 0; tex < ncuts; tex++)
        delete t[tex];
    delete leg;

    for (int i = 0; i < 3; i++)
        delete hratio[i];
    for (int i = 0; i < 4; i++)
        delete hd[i], hmc[i];
    delete pad1;
    delete pad2;
    delete c2;
}