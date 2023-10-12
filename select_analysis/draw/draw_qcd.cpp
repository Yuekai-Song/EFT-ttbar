#include<TROOT.h>
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
#include<TGraph.h>
#include<TKey.h>
#include<TGraphAsymmErrors.h>
#include<fstream>
#include<iostream>
#include<map>
using namespace std;

double p2weight=0.65/0.3;
void sete0(TH1D* h1){
    for(int i=0; i<h1->GetNbinsX(); i++){
        h1->SetBinError(i,0);
    }
}
void set_ratio(TH1D* h1, double a){
    for(int i=1; i<=h1->GetNbinsX(); i++)
        h1->SetBinContent(i, a);
}
void sys_and_nom(TString hist_name, TString& sys_name, TString& nom_name){
    int pos = 0;
    hist_name.ReplaceAll("Up", "");
    hist_name.ReplaceAll("Down", "");
    char* s = const_cast<char*>(hist_name.Data());
    while(s[pos] != '_')
        pos++;
    if(hist_name.Contains("ttbar")){
        pos++;
        while(s[pos] != '_')
            pos++;
    }
    s[pos] = 0;
    nom_name = TString(s);
    sys_name = TString(s+pos+1);
}
void get_error(TString nom_name, map<TString, TH1D> &hist_map, map<TString, std::vector<TString>> &nom_sys, double* up_vs, double* down_vs){
    double nom_v;
    double up_v, up_max;
    double down_v, down_min;
    TH1D *h1_up, *h1_dn, *h1_nom;

    for(vector<TString>::iterator it_sys=nom_sys[nom_name].begin(); it_sys!=nom_sys[nom_name].end(); it_sys++){
        h1_up = &hist_map[nom_name + "_" + *it_sys + "Up"];
        h1_dn = &hist_map[nom_name + "_" + *it_sys + "Down"];
        h1_nom = &hist_map[nom_name];
        const int n = h1_nom->GetNbinsX();
        //cout<<sys[s]<<": "<<pro[p]<<": "<<"bin 3: ";
        //cout<<up->GetBinContent(4)<<" "<<down->GetBinContent(4)<<" "<<h1->GetBinContent(4)<<endl;
        for(int b=0; b<n; b++){
            up_v = h1_up->GetBinContent(b+1);
            down_v = h1_dn->GetBinContent(b+1);
            nom_v = h1_nom->GetBinContent(b+1);
            up_max = max(up_v, down_v);
            down_min = min(up_v, down_v);
            if(up_max <= nom_v){
                up_vs[b] += 0;
                down_vs[b] += (nom_v - down_min) * (nom_v - down_min);
            }
            else if(down_min >= nom_v){
                up_vs[b] += (up_max - nom_v) * (up_max - nom_v);
                down_vs[b] += 0;
            }
            else{
                up_vs[b] += (up_max- nom_v) * (up_max - nom_v);
                down_vs[b] += (nom_v - down_min) * (nom_v - down_min);
            }
        }
    }
}
void set_error(TGraphAsymmErrors* hg, TH1D* h1, double bin_len, map<TString, TH1D> &hist_map, map<TString, std::vector<TString>> &nom_sys, bool is_abs){
    const int n = h1->GetNbinsX();
    double up_vs[n], down_vs[n], nom;
    TString pro[] = {"ttbar_ci0000", "DYJets", "STop", "WJets"};
    for(int i=0; i<n; i++){
        up_vs[i] = 0;
        down_vs[i] = 0;
    }
    for(int p=0; p<4; p++){
        get_error(pro[p], hist_map, nom_sys, up_vs, down_vs);
    }
    if(!is_abs){
        for(int i=0; i<n; i++){
            nom = h1->GetBinContent(i+1);
            up_vs[i] /= nom * nom;
            down_vs[i] /= nom * nom;
        }
    }
    for(int i=0; i<n; i++){
        hg->SetPointEXlow(i, bin_len/2.0);
        hg->SetPointEXhigh(i, bin_len/2.0);
        hg->SetPointEYhigh(i, sqrt(up_vs[i]));
        hg->SetPointEYlow(i, sqrt(down_vs[i]));
    }
}
void format_leg(TLegend* leg){
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
void format_stack(THStack *hstack, TString xtitle) {
    hstack->GetXaxis()->SetNdivisions(505);
    hstack->GetXaxis()->SetLabelFont(42);
    hstack->GetXaxis()->SetLabelOffset(0.007);
    hstack->GetXaxis()->SetLabelSize(0.00);
    hstack->GetXaxis()->SetTitleSize(0.04);
    hstack->GetXaxis()->SetTitleOffset(1.3);
    hstack->GetXaxis()->SetTitleFont(42);
    hstack->GetYaxis()->SetNdivisions(505);
    hstack->GetYaxis()->SetLabelFont(42);
    hstack->GetYaxis()->SetLabelOffset(0.007);
    hstack->GetYaxis()->SetLabelSize(0.05);
    hstack->GetYaxis()->SetTitleSize(0.06);
    hstack->GetYaxis()->SetTitleOffset(1.1);
    hstack->GetYaxis()->SetTitleFont(42);
    hstack->GetXaxis()->CenterTitle();
    hstack->GetYaxis()->CenterTitle();
    hstack->GetXaxis()->SetTitle("");
    hstack->GetYaxis()->SetTitle("Events/bin");
    //cout<<hstack->GetMaximum()<<endl;
    //hstack->GetYaxis()->SetRangeUser(0, hstack->GetMaximum() * 1.4);
    hstack->SetMaximum(hstack->GetMaximum() * 1.4);
}
void format_pad(TPad* pad1, TPad* pad2, TPad* pad3){
    pad1->Draw();
    pad2->Draw();
    pad3->Draw();
    pad1->cd();
    pad1->SetTopMargin(0.1);
    pad1->SetBottomMargin(0.02);
    pad1->SetLeftMargin(0.15);
    //pad1->SetTopMargin(0);
    pad2->cd();
    //pad2->SetTopMargin(0);
    pad2->SetTopMargin(0.03);
    pad2->SetBottomMargin(0.35);
    pad2->SetLeftMargin(0.15);

    pad3->cd();
    //pad2->SetTopMargin(0);
    pad3->SetTopMargin(0.03);
    pad3->SetBottomMargin(0.35);
    pad3->SetLeftMargin(0.15);
    pad3->SetFillColor(0);
    pad3->SetFillStyle(4000);
    pad3->SetFrameFillStyle(0);
}
void format_th(TH1D* h1, TString xtitle){
    int ydivisions=505;
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
    h1->GetXaxis()->SetTitleOffset(1.0);
    h1->GetYaxis()->SetTitleOffset(1.1/p2weight);
    h1->GetXaxis()->SetLabelSize(0.11);
    h1->GetYaxis()->SetLabelSize(0.05*p2weight);
    h1->GetYaxis()->SetRangeUser(0.5, 1.5);
}
void draw_pre(TString cut_name, int var, int year){//2, 0
    TString legend[] = {"tt","DY","single top","V+jets","QCD"};
    TString pro[] = {"ttbar_ci0000","DYJets","STop","WJets"};
    TString legendd = "data";
    TString xtitle[] = {"lnL","M_{t}","M_{#bar{t}}","M_{Wl}","M_{Wh}","M_{th}","M_{tl}","P_{T}^{l}","P_{T}^{leading-jet}","jet_num","p_{T}^{t}","M_{t#bar{t}}","#Deltay_{t#bar{t}}"};
    TString title[] = {"likelihood","mass_t","mass_at","mass_wlep","mass_whad","mass_thad","mass_tlep","lepton_pt","leading_pt","jet_num","top_pt","Mtt", "deltay"};
    Double_t xup, xdown, bin_len;
    Int_t bins;
    Double_t nums, events;

    TString path = Form("../sys_root/%d/",year);
    int edge[]={0,3,11,16,20,29};//23,31}; 
    int color[] = {2, 46, 9, 29, 8, kYellow, 93};
    
    TString sys_file = path+title[var]+"_"+cut_name+".root";
    TFile *file = TFile::Open(sys_file);
    TList *list = file->GetListOfKeys();
    TIter iter(list);
    TKey *key;
    map<TString, TH1D> hist_map;
    map<TString, std::vector<TString>> nom_sys;
    TString sys_name, nom_name;
    while((key = (TKey*)iter())){
        if(key->GetClassName() == "TH1D"){
            TH1D* hist = (TH1D*)key->ReadObj();
            if(hist){
                TString hist_name = TString(hist->GetName());
                hist_map[hist_name] = *hist;
                sys_and_nom(hist_name, sys_name, nom_name);
                nom_sys[nom_name].push_back(sys_name);
            }
            delete hist;
        }
    }
    file->Close();
    TH1D *nmc, *hist, *h1[5];
    TH1D *hdata, *hdatad, *hmc;
    TH1D *hratio[3];
    TGraphAsymmErrors *hmcdg, *hmcg;
    
    
    auto c1 = new TCanvas("c1", "c1", 8, 30, 600, 600); // temporary canvas
    auto c2 = new TCanvas("c2", "c2", 8, 30, 650, 650);
    TPad *pad1 = new TPad("pad1","This is pad1",0.05,0.32,0.95,0.97);
    TPad *pad2 = new TPad("pad2","This is pad2",0.05,0.02,0.95,0.32);
    TPad *pad3 = new TPad("pad3","This is pad3",0.05,0.02,0.95,0.32);
    c2->cd();
    format_pad(pad1, pad2, pad3);
    TLegend *leg = new TLegend(0.70, .65, 1.00, .90);
    format_leg(leg);
    format_canvas(c2);
    
    hdata = &hist_map["data_obs"];
    bins = hdata->GetNbinsX();
    xdown = hdata->GetXaxis()->GetBinLowEdge(1);
    xup = hdata->GetXaxis()->GetBinUpEdge(bins);
    bin_len = (xup - xdown)/(1.0 * bins);

    hmc = new TH1D("mc", "", bins, xdown, xup);
    hmc->Sumw2();
    hdatad = (TH1D*)hdata->Clone();
    hdatad->SetName("datad");
    THStack* hstack = new THStack("hstack", "");
    nums = 0;
    events = hdata->GetSumOfWeights();

    //h1[4] = (TH1D*)qcd->Get(title[var]+"_QCD_derived");
    //cout<<legend[4]<<": "<<h1[4]->GetSumOfWeights()<<endl;
    //h1[4]->SetFillColor(color[4]);
    //hstack->Add(h1[4]);
    //hmc->Add(h1[4]);
    //nums+=h1[4]->GetSumOfWeights();
    //leg->AddEntry(h1[4], legend[4], "f");
    for (int k = 3; k >= 0; k--){
        h1[k] = &hist_map[pro[k]];
        //cout<<legend[k]<<": "<<h1[k]->GetSumOfWeights()<<endl;
        hmc->Add(h1[k]);
        nums += h1[k]->GetSumOfWeights();
        h1[k]->SetFillColor(color[k]);
        hstack->Add(h1[k]);
    }
    for(int k=0; k<4; k++){
        leg->AddEntry(h1[k], legend[k], "f");
    }
    leg->AddEntry(hdata, legendd, "p");
    for(int r=0; r<3; r++){
        hratio[r]=new TH1D(Form("ratio%d",r), "", bins, xdown, xup);
        set_ratio(hratio[r], 0.75+0.25*r);
    }
    sete0(hmc);
    hdatad->Divide(hmc);
    hmcdg = new TGraphAsymmErrors(hratio[1]);
    hmcg = new TGraphAsymmErrors(hmc);
    set_error(hmcdg, hmc, bin_len, hist_map, nom_sys, 0);
    set_error(hmcg, hmc, bin_len, hist_map, nom_sys, 1);
    //cout<<nums<<" "<<events<<endl;


    pad1->cd();
    hstack->Draw("hist");
    format_stack(hstack, xtitle[var]);
    leg->Draw("same");
    hdata->Draw("PSame");
    hdata->SetMarkerSize(0.8);
    hdata->SetMarkerStyle(21);
    hmcg->Draw("2Same");
    hmcg->SetFillColor(1);
    hmcg->SetFillStyle(3005);

    pad2->cd();
    gStyle->SetOptStat(0);
    format_th(hdatad, xtitle[var]);
    hdatad->Draw("P");        
    pad2->cd();
    hmcdg->Draw("2Same");
    hmcdg->SetFillColor(1);
    hmcdg->SetFillStyle(3005);
    for(int r=0; r<3; r++){
        hratio[r]->Draw("LSame");
        hratio[r]->SetLineStyle(7);
        hratio[r]->SetLineColor(1);
    }

    pad3->cd();
    gStyle->SetOptStat(0);
    TH1D* hc = (TH1D*)hdatad->Clone();
    hc->SetName("clone");
    hc->Draw("PY+"); 
    hc->GetYaxis()->SetTitleSize(0);

    c2->Print(Form("../qcd_pdf/%d/",year)+title[var]+"_"+cut_name+".pdf");

    for(int i=0; i<3; i++)
        delete hratio[i];
    delete hc;
    delete hdatad;
    delete hmc;
    delete hmcg;
    delete hmcdg;
    delete hstack;
    delete leg;
    delete pad1;
    delete pad2;
    delete pad3;
    delete c1;
    delete c2;
}
void draw_qcd(){
    TString cutsName[] = {"E_3jets", "E_4jets", "M_3jets", "M_4jets"};
    int year[] = {2015, 2016, 2017, 2018};
    int var[] = {0, 1, 10, 11, 12};
    for(int i=0; i<4; i++){
        for(int y=0; y<4; y++){
            for(int v=0; v<5; v++)
                draw_pre(cutsName[i], var[v], year[y]);
        }
    }
}