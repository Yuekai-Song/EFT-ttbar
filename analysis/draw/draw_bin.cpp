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
    //pad2->SetTopMargin(0);
    pad2->SetTopMargin(0.03);
    pad2->SetBottomMargin(0.45);
    pad2->SetLeftMargin(0.15);
}
void format_th_pad2(TH1D* h1, TString xtitle, double* low, double* high){
    int ydivisions=505;
    float up = h1->GetMaximum();
    float down = h1->GetMinimum();
    up = fabs(up);
    down = fabs(down);
    h1->SetMarkerStyle(20);
    h1->SetMarkerSize(0.4);
    h1->SetStats(kFALSE);
    h1->GetXaxis()->SetTitle(xtitle);
    h1->GetYaxis()->SetTitle("Corr");
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
    //h1->GetYaxis()->SetRangeUser(1-0.05,1+0.05);
    //return;
    if(up>0.1 || down>0.1){
        h1->GetYaxis()->SetRangeUser(-0.15,0);
        *low = 0.85;
        *high = 1.15;
        return;
    }
    if(up > down){
        h1->GetYaxis()->SetRangeUser(-1.8*up, 0);
        *low = 1-1.3*up;
        *high = 1+1.3*up;
    }  
    else{
        h1->GetYaxis()->SetRangeUser(-1.8*down, 0);
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
    TString xtitle = "M_{t#bar{t}}";
    TString title = "Mtt";
    TString div_con[] = {"*(fabs(rapidity_tt)<0.4)", "*(fabs(rapidity_tt)<1.0 && fabs(rapidity_tt)>0.4)","*(fabs(rapidity_tt)<2.0 && fabs(rapidity_tt)>1.0)", "*(fabs(rapidity_tt)>2.0)"};
    int color[] = {1, 2, 46, 9, 29, 8, kYellow, 93};
    TString EW[] = {"_ci0000","_ci0010","_ci0020","_ci0001","_ci0002","_ci0100","_ci0200"};
    TString legend[] = {"no_EW","ci0000","ci0010","ci0020","ci0001","ci0002","ci0100","ci0200"};
    TH1D *hmc1[6], *hmc0; 
    TH1D *hd1[6]; 
    TFile* file = TFile::Open(path1+File);
    for(int div=0; div<4; div++){
        auto c2 = new TCanvas("c2", "c2", 8, 30, 650, 650);
        TPad *pad1 = new TPad("pad1","This is pad1",0.05,0.32,0.95,0.97);
        TPad *pad2 = new TPad("pad2","This is pad2",0.05,0.02,0.95,0.32);
        c2->cd();
        format_pad(pad1, pad2);
        TLegend *leg = new TLegend(0.70, .65, 1.00, .90);
        format_leg(leg);
        format_canvas(c2);
        hmc0 = (TH1D*)file->Get(title+"_nom"+Form("%d",div));
        pad1->cd();
        hmc0->Draw("hist");
        format_th_pad1(hmc0, xtitle);
        hmc0->SetLineColor(color[0]);
        hmc0->SetLineWidth(2.0);
        leg->AddEntry(hmc0, legend[0], "l");
        for(int i = 0; i < 6; i++){
            hmc1[i] = (TH1D*)file->Get(title+EW[i]+"_nom"+Form("%d",div));
            pad1->cd();
            hmc1[i]->Draw("hSame");
            hmc1[i]->SetLineColor(color[i+1]);
            hmc1[i]->SetLineWidth(2.0);
            leg->AddEntry(hmc1[i], legend[i+1], "l");
            leg->Draw("Same");
            /*TPaveText *l = new TPaveText(0.4,0.75,0.5,0.85,"NDC");
            format_text(l);
            l->AddText(tex[s]);
            l->Draw("same");*/
            hd1[i] = (TH1D*)hmc1[i]->Clone();
            hd1[i]->SetName(Form("hd%d",i));
            hd1[i]->Add(hmc0, -1);
            hd1[i]->Divide(hmc0);
            pad2->cd();
            
            if(i==0){
                hd1[i]->Draw("hist");
                format_th_pad2(hd1[i], xtitle, &low, &high);
            }
            else
                hd1[i]->Draw("histsame");
            hd1[i]->SetLineColor(color[i+1]);
            hd1[i]->SetLineWidth(2.0);
            

            seterror0(hd1[i]);

            /*hd0->Draw("LSame");
            hd0->SetLineStyle(7);
            hd0->SetLineColor(color[0]);*/   
        } 
        c2->Print(path1+path2+"deltay"+Form("%d", div)+".pdf");
        //c2->Print(path1+path2+title[i]+sys[s]+pro[c]+name+".pdf");
        delete leg;
        //delete hmc1;
        //delete hmc0;
        //delete hd1;
        delete c2;
    }
    
    file->Close();
}
void draw_bin(){
    TString path1 = "../2018/";
    //TString file[] = {"EWun_E_3jets.root", "EWun_E_4jets.root", "EWun_M_3jets.root", "nnlo_M_4jets.root"};
    //TString name[] = {"_E_3jets", "_E_4jets", "_M_3jets", "_M_4jets"};

    TString path2 = "bin_divide/";
    draw(path1, path2, "mtt_ytt.root");
}