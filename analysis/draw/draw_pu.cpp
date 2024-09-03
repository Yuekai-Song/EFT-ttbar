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
void format_th_pad2(TH1D* h1, TString xtitle ){
    int ydivisions=505;
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
    h1->GetYaxis()->SetRangeUser(0.5,1.5);
    return;
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
void draw_pre(TString path, TString filename){
    double low, high;
    TString process[]={"TT", "DY","ST", "WJets"};
    TString title[] = {"PV_npvs", "PV_npvsGood"};
    TFile* file = TFile::Open(path+"pileup/"+filename);
    TH1D *hist0, *hist1, *hist2, *hist3;
    TH1D *hmc0, *hmc1, *hmc2, *hmc3;
    TH1D *hd0, *hd1, *hd2, *hd3;
    TH1D *hdata, *hdatad;
    int color[] = {2, 1, 4, 6};
    TString xtitle[] = {"PV_npvs","PV_npvsGood"};
    TString legend[] = {"nom", "up", "down", "no"};
    TString xvars[] = {"PV_npvs", "PV_npvsGood"};
    Double_t xup[] = {65, 65};
    Double_t xdown[] = {0, 0};
    Int_t bins[]={65, 65};
    TString other_con1 = "*((jet_num>=4)||(jet_num==3 && jet_pt[0]>50))";
    TString other_con2 = "*(lep_flavour||((!lep_flavour) && lepton_pt>34))";
    for(int t=0; t<2; t++){
        TCanvas* c2 = new TCanvas("c1", "c1", 8, 30, 650, 650);
        TPad *pad1 = new TPad("pad1","This is pad1",0.05,0.32,0.95,0.97);
        TPad *pad2 = new TPad("pad2","This is pad2",0.05,0.02,0.95,0.32);
        c2->cd();
        format_pad(pad1, pad2);
        TLegend *leg = new TLegend(0.60, .65, 0.90, .85);
        format_leg(leg);
        format_canvas(c2);
        TChain* tree2 = new TChain("mytree");
        tree2->Add(path+"data/new_data_*.root");
        hdata = new TH1D("data", "", bins[t], xdown[t], xup[t]);
        hdatad = new TH1D("datad", "", bins[t], xdown[t], xup[t]);
        tree2->Draw(title[t]+">>data","1"+other_con1+other_con2);
        tree2->Draw(title[t]+">>datad","1"+other_con1+other_con2);
        delete tree2;
        for(int c=0; c<4; c++){      
            hist0 = (TH1D*)file->Get(title[t]+"_"+process[c]);
            hist1 = (TH1D*)file->Get(title[t]+"_"+process[c]+"_nom");
            hist2 = (TH1D*)file->Get(title[t]+"_"+process[c]+"_up");
            hist3 = (TH1D*)file->Get(title[t]+"_"+process[c]+"_down");
            if(c==0){
                hmc0 = (TH1D*)hist0->Clone();
                hmc1 = (TH1D*)hist1->Clone();
                hmc2 = (TH1D*)hist2->Clone();
                hmc3 = (TH1D*)hist3->Clone();
            }
            else{
                hmc0->Add(hist0);
                hmc1->Add(hist1);
                hmc2->Add(hist2);
                hmc3->Add(hist3);
            }
            delete hist0; delete hist1; delete hist2; delete hist3;
        }
        TH1D* hd = (TH1D*)hmc0->Clone();
        hd0 = (TH1D*)hmc0->Clone();
        hd1 = (TH1D*)hmc1->Clone();
        hd2 = (TH1D*)hmc2->Clone();
        hd3 = (TH1D*)hmc3->Clone();
        hd0->Divide(hd);
        hd1->Divide(hd);
        hd2->Divide(hd);
        hd3->Divide(hd);
        hdatad->Divide(hd);

        pad1->cd();
        
        hmc1->Draw("hist");
        format_th_pad1(hmc1, xtitle[t]);
        hmc1->SetLineColor(color[0]);
        hmc2->Draw("histSame");
        hmc2->SetLineColor(color[1]);
        hmc3->Draw("histSame");
        hmc3->SetLineColor(color[2]);
        hmc0->Draw("histSame");
        hmc0->SetLineColor(color[3]);
        hmc1->SetLineWidth(1.0);
        hmc2->SetLineWidth(1.0);
        hmc3->SetLineWidth(1.0);
        hmc0->SetLineWidth(1.0);
        hdata->Draw("PSame");
        hdata->SetMarkerSize(0.5);
        hdata->SetMarkerStyle(21);
        leg->AddEntry(hmc1, legend[0], "l");
        leg->AddEntry(hmc2, legend[1], "l");
        leg->AddEntry(hmc3, legend[2], "l");
        leg->AddEntry(hmc0, legend[3], "l");
        leg->AddEntry(hdata,"data","p");
        leg->Draw("Same");

        pad2->cd();
        hd2->Draw("hist");
        hd2->SetLineColor(color[1]);
        format_th_pad2(hd2, xtitle[t]);
        seterror0(hd2);
        hd1->Draw("histsame");
        hd1->SetLineColor(color[0]);
        seterror0(hd1);
        hd3->Draw("hSame");
        seterror0(hd3);
        hd3->SetLineColor(color[2]);
        hd0->Draw("hSame");
        seterror0(hd0);
        hd0->SetLineStyle(9);
        hd0->SetLineColor(color[3]);
        hd1->SetLineWidth(1.0);
        hd2->SetLineWidth(1.0);
        hd3->SetLineWidth(1.0);
        hdatad->Draw("PSame");
        hdatad->SetMarkerSize(0.5);
        hdatad->SetMarkerStyle(21);
        c2->Print(path+"pileup/"+title[t]+".pdf");

        delete leg;
        delete hmc1;
        delete hmc2;
        delete hmc3;
        delete hd1;
        delete hd2;
        delete hd3;
        delete hdata; delete hdatad;
        delete pad1; delete pad2;
        delete c2;
    }   
}
void draw_pu(){
    TString filenames[] = {"sys_M_4jets.root","sys_M_3jets.root","sys_E_4jets.root","sys_E_3jets.root"};
    TString path2[] = {"sys_pdf/M_4jets/","sys_pdf/M_3jets/","sys_pdf/E_4jets/","sys_pdf/E_3jets/"};
    int year = 2018;
    TString path = Form("../%d/", year);
    //for(int i=0; i<2; i++)
    draw_pre(path, "pu_semi.root");
}