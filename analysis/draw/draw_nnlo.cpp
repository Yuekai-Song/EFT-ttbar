#include<TROOT.h>
#include<TLine.h>
#include<TStyle.h>
#include<Math/PdfFuncMathCore.h>
#include<TFile.h>
#include<TChain.h>
#include<TLorentzVector.h>
#include<TMath.h>
#include<TLegend.h>
#include<TF1.h>
#include<TH1D.h>
#include<TCanvas.h>
#include<TH2F.h>
#include<THStack.h>
#include<string.h>
#include<fstream>
#include<iostream>
#include<TPaveText.h>
using namespace std;
double p2weight=0.65/0.3;
void seterror0(TH1D* h1){
    for(int i=1; i<=h1->GetNbinsX(); i++){
        h1->SetBinError(i,0);
    }
}
void format_text(TPaveText* lumi){
    lumi->SetFillColor(0);
    lumi->SetTextFont(42);
    lumi->SetTextSize(0.04);
    lumi->SetTextColor(1);
    lumi->SetBorderSize(0);
}
void format_leg(TLegend* leg){
    leg->SetNColumns(1);
    leg->SetColumnSeparation(0.5);
    leg->SetEntrySeparation(0.0);
    leg->SetBorderSize(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.05);
    leg->SetLineColor(1);
    leg->SetLineStyle(1);
    leg->SetLineWidth(3.0);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
}
void format_canvas(TCanvas *c) {
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
void format_pad(TPad* pad1, TPad* pad2){
    pad1->Draw();
    pad2->Draw();
    pad1->cd();
    pad1->SetTopMargin(0.1);
    pad1->SetBottomMargin(0.02);
    pad1->SetLeftMargin(0.15);
    //pad1->SetTopMargin(0);
    pad2->cd();
//	pad2->SetTopMargin(0);
    pad2->SetTopMargin(0.03);
    pad2->SetBottomMargin(0.45);
    pad2->SetLeftMargin(0.15);
}
void format_th_pad2(TH1D* h1, TString xtitle, double* low, double* high){
    int ydivisions=505;
    float up = h1->GetMaximum();
    float down = h1->GetMinimum();
    up = fabs(up-1);
    down = fabs(down-1);
    h1->SetMarkerStyle(20);
    h1->SetMarkerSize(0.4);
    h1->SetStats(kFALSE);
    h1->GetXaxis()->SetTitle(xtitle);
    h1->GetYaxis()->SetTitle("#frac{NNLO}{NLO}");
    h1->SetTitle("");
    h1->GetXaxis()->CenterTitle();
    h1->GetYaxis()->CenterTitle();
    h1->GetYaxis()->SetNdivisions(ydivisions);
    h1->GetXaxis()->SetTitleSize(0.12);
    h1->GetYaxis()->SetTitleSize(0.05*p2weight);
    h1->GetXaxis()->SetTitleOffset(1.8);
    h1->GetYaxis()->SetTitleOffset(1.3/p2weight);
    h1->GetXaxis()->SetLabelSize(0.11);
    h1->GetYaxis()->SetLabelSize(0.05*p2weight);
    h1->GetYaxis()->SetRangeUser(1-0.05,1+0.05);
    return;
    if(up>0.1 || down>0.1){
        h1->GetYaxis()->SetRangeUser(1-0.15,1+0.15);
        *low = 0.85;
        *high = 1.15;
        return;
    }
    if(up > down){
        h1->GetYaxis()->SetRangeUser(1-1.3*up, 1+1.3*up);
        *low = 1-1.3*up;
        *high = 1+1.3*up;
    }  
    else{
        h1->GetYaxis()->SetRangeUser(1-1.3*down, 1+1.3*down);
        *low = 1-1.3*down;
        *high = 1+1.3*down;
    }
}
void format_th_pad1(TH1D* h1, TString xtitle){
    int ydivisions=505;
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
    h1->GetYaxis()->SetRangeUser(0.0, h1->GetMaximum()*1.4);
    //h1->GetYaxis()->SetRangeUser(0.0, 80000);
}
void format_line(TLine* l1){
    l1->SetLineStyle(9);
    l1->SetLineWidth(2);
    l1->SetLineColor(1);
}

void set_ratio(TH1D* h1, double a){
    for(int i=1; i<=h1->GetNbinsX(); i++)
        h1->SetBinContent(i, a);
}
void draw(TString path1, TString path2, TString File){
    double low, high;
    //TString tex[] = {"SM case","(C_{#varphiu}, Re[C_{u#varphi}], Im[C_{u#varphi}]) = (0, 1, 0)", "(C_{#varphiu}, Re[C_{u#varphi}], Im[C_{u#varphi}]) = (1, 0, 0)", "(C_{#varphiu}, Re[C_{u#varphi}], Im[C_{u#varphi}]) = (0, 0, 1)"};
    TString xtitle[] = {"M_{t#bar{t}}^{gen}", "p_{T}^{t-gen}","#Deltay_{t#bar{t}}^{gen}", "M_{t#bar{t}}", "p_{T}^{t}","#Deltay_{t#bar{t}}"};
    TString title[] = {"Mtt_gen", "top_pt_gen", "deltay_gen","Mtt", "top_pt", "deltay"};
    int color[] = {2, 4, 2};
    TString legend[] = {"NLO", "NNLO",};
    TH1D *hmc1, *hmc0; 
    TH1D *hd1, *hd0;
    TFile* file = TFile::Open(path1+File);
    for(int i = 0; i < 6; i++){
        auto c2 = new TCanvas("c2", "c2", 8, 30, 650, 650);
        TPad *pad1 = new TPad("pad1","This is pad1",0.05,0.32,0.95,0.97);
        TPad *pad2 = new TPad("pad2","This is pad2",0.05,0.02,0.95,0.32);
        c2->cd();
        format_pad(pad1, pad2);
        TLegend *leg = new TLegend(0.70, .65, 1.00, .90);
        format_leg(leg);
        format_canvas(c2);
        hmc1 = (TH1D*)file->Get(title[i]+"_nnlo");
        hmc0 = (TH1D*)file->Get(title[i]+"_nlo");
        TH1D* hd = (TH1D*)hmc0->Clone();

        hd1 = (TH1D*)hmc1->Clone();
        hd1->Divide(hd);

        TH1D* hd0 = (TH1D*)hmc0->Clone();
        set_ratio(hd0, 1);
        seterror0(hd0);

        pad1->cd();
        hmc0->Draw("hist");
        format_th_pad1(hmc0, xtitle[i]);
        hmc0->SetLineColor(color[0]);
        hmc0->SetLineWidth(2.0);
        hmc1->Draw("hSame");
        hmc1->SetLineColor(color[1]);
        hmc1->SetLineWidth(2.0);
        leg->AddEntry(hmc0, legend[0], "l");
        leg->AddEntry(hmc1, legend[1], "l");
        leg->Draw("Same");
        /*TPaveText *l = new TPaveText(0.4,0.75,0.5,0.85,"NDC");
        format_text(l);
        l->AddText(tex[s]);
        l->Draw("same");*/

        pad2->cd();
        hd1->Draw("hist");
        hd1->SetLineColor(color[1]);
        hd1->SetLineWidth(2.0);
        format_th_pad2(hd1, xtitle[i], &low, &high);

        seterror0(hd1);

        hd0->Draw("LSame");
        hd0->SetLineStyle(7);
        hd0->SetLineColor(color[0]);

        c2->Print(path1+title[i]+"_nnlo.pdf");
        //c2->Print(path1+path2+title[i]+sys[s]+pro[c]+name+".pdf");
        delete leg;
        delete hmc1;
        delete hmc0;
        delete hd1;
        delete hd0;
        delete c2;
    }
    file->Close();
}
void draw_nnlo(){
    TString path1 = "./2018/";
    TString file[] = {"EWun_E_3jets.root", "EWun_E_4jets.root", "EWun_M_3jets.root", "nnlo_M_4jets.root"};
    TString name[] = {"_E_3jets", "_E_4jets", "_M_3jets", "_M_4jets"};

    TString path2[] = {"EW_un/E_3jets/", "EW_un/E_4jets/", "EW_un/M_3jets/", "EW_un/M_4jets/"};
    for(int i=3; i<4; i++)
        draw(path1, path2[i],  file[i]);
}