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
    lumi->SetTextSize(0.05);
    lumi->SetTextColor(13);
    lumi->SetBorderSize(0);
}
void format_leg(TLegend* leg){
    leg->SetNColumns(1);
    leg->SetColumnSeparation(0.5);
    leg->SetEntrySeparation(0.0);
    leg->SetBorderSize(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.04);
    leg->SetLineColor(1);
    leg->SetLineStyle(1);
    leg->SetLineWidth(1);
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
void format_th_pad2(TH1D* h1, TString xtitle, double* low, double* high, int s){
    int ydivisions=505;
    double range[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.06, 0, 0.1, 0, 0};
    float up = h1->GetMaximum();
    float down = h1->GetMinimum();
    up = fabs(up-1);
    down = fabs(down-1);
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
    h1->GetYaxis()->SetTitleSize(0.06*p2weight);
    h1->GetXaxis()->SetTitleOffset(1.8);
    h1->GetYaxis()->SetTitleOffset(1.1/p2weight);
    h1->GetXaxis()->SetLabelSize(0.11);
    h1->GetYaxis()->SetLabelSize(0.05*p2weight);
    if(range[s] != 0){
        h1->GetYaxis()->SetRangeUser(1-range[s], 1+range[s]);
        *low = 1-range[s];
        *high = 1+range[s];
        return;
    }
    if(up>0.1 || down>0.1){
        h1->GetYaxis()->SetRangeUser(0.85,1.15);
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
    //h1->GetYaxis()->SetRangeUser(0.0, 800000);
}
void format_line(TLine* l1){
    l1->SetLineStyle(9);
    l1->SetLineWidth(2);
    l1->SetLineColor(1);
}
void draw_pre(TString path, TString path2, TString filename){
    double low, high;
    TString sys[] = {"jes","jer","unclus","SF_lepton","sj","L1PF","muR","muF","ISR","FSR","mtop","hdamp","TuneCP5","nnlo","EW_un","pdf", "qcds"};
    TString process[]={"TT", "DYJets","STop", "WJets", "QCD"};
    TString path3[] = {"jes/","jer/","unclus/","sl/","sj/","L1PF/","muR/","muF/","ISR/","FSR/","mtop/","hdamp/","TuneCP5/","nnlo/", "EW_un/", "pdf/", "qcds/"};
    TString title[] = {"Mtt", "deltay"};
    TFile* file = TFile::Open(path+filename);
    TH1D *hmc1, *hmc2, *hmc3;
    TH1D *hd1, *hd2, *hd3;
    int color[] = {2, 1, 4};
    TString xtitle[] = {"M_{t#bar{t}}","#Deltay_{t#bar{t}}"};
    TString legend[] = {"nom", "up", "down"};
    for(int t=0; t<2; t++){
        for(int c=0; c<5; c++){
            for(int s=0; s<17; s++){
                TCanvas* c2 = new TCanvas("c1", "c1", 8, 30, 650, 650);
                TPad *pad1 = new TPad("pad1","This is pad1",0.05,0.32,0.95,0.97);
                TPad *pad2 = new TPad("pad2","This is pad2",0.05,0.02,0.95,0.32);
                c2->cd();
                format_pad(pad1, pad2);
                TLegend *leg = new TLegend(0.60, .65, 0.90, .85);
                format_leg(leg);
                format_canvas(c2);
                hmc1 = (TH1D*)file->Get(title[t]+"_"+process[c]+"_nom");
                hmc2 = (TH1D*)file->Get(title[t]+"_"+process[c]+"_"+sys[s]+"_up");
                hmc3 = (TH1D*)file->Get(title[t]+"_"+process[c]+"_"+sys[s]+"_down");
                if(hmc2 == NULL){
                    delete pad1; delete pad2; delete leg;
                    delete hmc1; delete hmc2; delete hmc3;
                    delete c2;
                    continue;
                }
                TH1D* hd = (TH1D*)hmc1->Clone();
                hd1 = (TH1D*)hmc1->Clone();
                hd2 = (TH1D*)hmc2->Clone();
                hd3 = (TH1D*)hmc3->Clone();
                hd1->Divide(hd);
                hd2->Divide(hd);
                hd3->Divide(hd);

                pad1->cd();
                hmc1->Draw("hist");
                format_th_pad1(hmc1, xtitle[t]);
                hmc1->SetLineColor(color[0]);
                hmc2->Draw("histSame");
                hmc2->SetLineColor(color[1]);
                hmc3->Draw("histSame");
                hmc3->SetLineColor(color[2]);
                hmc1->SetLineWidth(2.0);
                hmc2->SetLineWidth(2.0);
                hmc3->SetLineWidth(2.0);
                leg->AddEntry(hmc1, legend[0], "l");
                leg->AddEntry(hmc2, legend[1], "l");
                leg->AddEntry(hmc3, legend[2], "l");
                leg->Draw("Same");

                pad2->cd();
                hd2->Draw("hist");
                hd2->SetLineColor(color[1]);
                format_th_pad2(hd2, xtitle[t], &low, &high, s);
                seterror0(hd2);
                hd3->Draw("hSame");
                seterror0(hd3);
                hd3->SetLineColor(color[2]);
                hd1->Draw("hSame");
                seterror0(hd1);
                hd1->SetLineStyle(9);
                hd1->SetLineColor(color[0]);
                hd1->SetLineWidth(2.0);
                hd2->SetLineWidth(2.0);
                hd3->SetLineWidth(2.0);
                c2->Print(path+path2+path3[s]+title[t]+"_"+process[c]+".pdf");

                delete leg;
                delete hmc1;
                delete hmc2;
                delete hmc3;
                delete hd1;
                delete hd2;
                delete hd3;
                delete pad1; delete pad2;
                delete c2;
            }
        }
    }
    //if(h1 == NULL)
    //    cout<<"right"<<endl;
        
}
void draw_sys(){
    TString filenames[] = {"sys_M_4jets.root","sys_M_3jets.root","sys_E_4jets.root","sys_E_3jets.root"};
    TString path2[] = {"sys_pdf/M_4jets/","sys_pdf/M_3jets/","sys_pdf/E_4jets/","sys_pdf/E_3jets/"};
    int year = 2018;
    TString path = Form("%d/", year);
    for(int i=0; i<2; i++)
        draw_pre(path, path2[i], filenames[i]);
}