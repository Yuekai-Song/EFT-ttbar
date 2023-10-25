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
#include<TKey.h>
#include<TPaveText.h>
using namespace std;
double p2weight=0.65/0.3;
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
void seterror0(TH1D* h1){
    for(int i=1; i<=h1->GetNbinsX(); i++){
        h1->SetBinError(i,0);
    }
}
void set_ratio(TH1D* h1, double a){
    for(int i=1; i<=h1->GetNbinsX(); i++){
        h1->SetBinContent(i, a);
        h1->SetBinError(i, 0);
    }
}
void format_text(TPaveText* lumi){
    lumi->SetFillColor(0);
    lumi->SetTextFont(42);
    lumi->SetTextSize(0.035);
    lumi->SetTextColor(13);
    lumi->SetBorderSize(0);
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
void get_range_pad2(TH1D* h1, double* ranges, double range){
    float up = h1->GetMaximum();
    float down = h1->GetMinimum();
    up = fabs(up);
    down = fabs(down);
    if(range != 0){
        *ranges = range;
        return;
    }
    if(up>0.1 || down>0.1){
        *ranges = 0.15;
        return;
    }
    if(up > down)
        *ranges = +1.3*up;
    else
        *ranges = +1.3*down;
}
void set_th_lable(TH1D* h1, int* nbins){
    double xbins[][20] = {{300,340,360,380,400,420,440,460,480,500,520,540,570,600,640,700,3000}, 
                          {300,450,500,570,600,700,820,3000}};
    for(int i=0; i<2; i++){
        for(int j=nbins[i]; j<nbins[i+1]; j++){
            h1->GetXaxis()->SetBinLabel(j+1, TString::Format("%.0f-%.0f", xbins[i][j-nbins[i]], xbins[i][j-nbins[i]+1]));
        }
    }
    h1->LabelsDeflate("X");
    h1->LabelsDeflate("Y");
    h1->LabelsOption("v");
}
void format_th_pad2(TH1D* h1, TString xtitle, double range, int color, int type, int* nbins){
    int ydivisions=505;
    h1->SetLineColor(color);
    if(type == 0){//ratio
        h1->SetLineStyle(7);
        h1->SetLineColor(1);
        h1->SetLineWidth(1);
    }
    if(type == 1){//filled TH1D
        h1->SetFillColorAlpha(color, 0.3);
        h1->SetLineWidth(0);
    }
    else{//real TH1D(line)
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
    h1->GetYaxis()->SetTitleSize(0.06*p2weight);
    h1->GetXaxis()->SetTitleOffset(1.8);
    h1->GetYaxis()->SetTitleOffset(1.1/p2weight);
    h1->GetXaxis()->SetLabelSize(0.11);
    h1->GetYaxis()->SetLabelSize(0.05*p2weight);
    h1->GetYaxis()->SetRangeUser(-range, range);
    set_th_lable(h1, nbins);
}
double format_th_pad1(TH1D* h1, TString xtitle, int color){
    int ydivisions=505;
    h1->SetLineColor(color);
    h1->SetLineWidth(2);
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
    //h1->GetYaxis()->SetRangeUser(0.0, h1->GetMaximum()*1.4);
    h1->GetYaxis()->SetRangeUser(0.0, 80000);
    return h1->GetMaximum();
    //h1->GetYaxis()->SetRangeUser(0.0, 800000);
}
void format_line(TLine* l1){
    l1->SetLineStyle(9);
    l1->SetLineWidth(2);
    l1->SetLineColor(1);
}
void draw_pre(TH1D* hsm, TH1D* hmc[4], TString sys, TString other_name, double range0, TString path){
    double range, high;
    TString name[] = {"Up", "Down"}; 
    int color[] = {2, 4};
    TString xtitle = "M_{t#bar{t}}";
    TString legend[] = {"up", "down"};
    int div = 16;
    int nbins[] = {0, 16, 23};
    int bins = 23;
    TString cut[2] = {"|#Deltay|<1.4", "|#Deltay|>1.4"};
    TLine *l1, *l2;
    TPaveText *t[2];

    TH1D *hd[4], *hratio[3];
    TCanvas* c2 = new TCanvas("c1", "c1", 8, 30, 650, 650);
    TPad *pad1 = new TPad("pad1","This is pad1",0.05,0.32,0.95,0.97);
    TPad *pad2 = new TPad("pad2","This is pad2",0.05,0.02,0.95,0.32);
    c2->cd();
    format_pad(pad1, pad2);
    TLegend *leg = new TLegend(0.75, .55, 1.00, .70);
    format_leg(leg);
    format_canvas(c2);
    for(int i=0; i<4; i++){
        hd[i] = (TH1D*)hmc[i]->Clone();
        hd[i]->SetName(Form("hd_%d", i));
        hd[i]->Add(hsm, -1);
        hd[i]->Divide(hsm);
    }
    pad1->cd();
    hsm->Draw("hist");
    high = format_th_pad1(hsm, xtitle, 1);
    leg->AddEntry(hsm, "SM case", "l");
    for(int i=0; i<2; i++){
        hmc[i]->Draw("histSame");
        format_th_pad1(hmc[i], xtitle, color[i]);
        leg->AddEntry(hmc[i], sys+"_"+legend[i], "l");
    }
    leg->Draw("Same");
    
    l1 = new TLine(div, 0, div, high);
    format_line(l1);
    pad1->cd();
    l1->Draw("same");

    for(int tex=0; tex<2; tex++){
        t[tex] = new TPaveText(0.35+0.38*tex, 0.77, 0.44+0.38*tex, 0.87, "NDC");
        format_text(t[tex]);
        t[tex]->AddText(cut[tex]);
        t[tex]->Draw("same");
    }

    pad2->cd();
    get_range_pad2(hd[0], &range, range0);
    for(int r=0; r<3; r++){
        hratio[r] = new TH1D(Form("ratio%d",r), "", bins, 0, bins);
        set_ratio(hratio[r], 0.5*range*(r-1));
        if(r == 0)
            hratio[r]->Draw("L");
        else
            hratio[r]->Draw("LSame");
        format_th_pad2(hratio[r], xtitle, range, 1, 0, nbins);
    }
    for(int i=0; i<2; i++){
        hd[i]->Draw("PhSame");
        format_th_pad2(hd[i], xtitle, range, color[i], 2, nbins);
    }
    for(int i=0; i<2; i++){
        hd[2+i]->Draw("hSame");
        format_th_pad2(hd[2+i], xtitle, range, color[i], 1, nbins);
    }

    l2 = new TLine(div, -range, div, range);
    format_line(l2);
    l2->Draw("same");

    c2->Print("./sys_pdf/"+path+"/"+sys+other_name+".pdf");
   
    delete l1, l2;
    for(int tex=0; tex<2; tex++)
        delete t[tex];
    delete leg;

    for(int i=0; i<3; i++)
        delete hratio[i];
    for(int i=0; i<4; i++)
        delete hd[i], hmc[i];
    delete pad1; delete pad2;
    delete c2;
        
}
void draw_sys(TString cutname, int year, TString path){  
    TString inpath = "../../combine/";
    TString filename = "ttbar"+cutname+Form("_%d.root", year);
    TFile* file_ori = TFile::Open(inpath+"datacard_ttx/original/"+filename);
    TFile* file = TFile::Open(inpath+"datacard_ttx/"+path+"/"+filename);
    static TString classname("TH1D");
    TH1D *hsm;
    TH1D *hmc[4];
    map<TString, TH1D> hist_map;
    map<TString, std::vector<TString>> sys_nom;
    TString sys_name, nom_name;
    TList *list = file->GetListOfKeys();
    TList *list_ori = file_ori->GetListOfKeys();
    TKey *key, *key_ori;
    TIter iter(list), iter_ori(list_ori);
    map<TString, double> sys_range = {{"sb_co", 0.25}, {"mtop3", 0.20}};
    double range;
    while((key = (TKey*)iter())){
        if(key->GetClassName() == classname){
            TH1D* hist = (TH1D*)key->ReadObj();
            if(hist){
                TString hist_name = TString(hist->GetName());
                hist_map[hist_name] = *hist;
                if(hist_name.Contains("Up") ){
                    sys_and_nom(hist_name, sys_name, nom_name);
                    sys_nom[sys_name].push_back(nom_name);
                }
            }
            delete hist;
        }
    }
    while((key_ori = (TKey*)iter_ori())){
        if(key_ori->GetClassName() == classname){
            TH1D* hist = (TH1D*)key_ori->ReadObj();
            if(hist){
                TString hist_name = TString(hist->GetName()) + "_ori";
                hist_map[hist_name] = *hist;
            }
            delete hist;
        }
    }
    for(map<TString, std::vector<TString>>::iterator it_sys=sys_nom.begin(); it_sys!=sys_nom.end(); it_sys++){
        for(vector<TString>::iterator it_nom=it_sys->second.begin(); it_nom!=it_sys->second.end(); it_nom++){
            if(sys_range.find(it_sys->first) == sys_range.end())
                range = 0;
            else
                range = sys_range[it_sys->first];
            hsm = &hist_map[*it_nom];
            hmc[0] = &hist_map[*it_nom+"_"+it_sys->first+"Up"];
            hmc[1] = &hist_map[*it_nom+"_"+it_sys->first+"Down"];
            hmc[2] = &hist_map[*it_nom+"_"+it_sys->first+"Up_ori"];
            hmc[3] = &hist_map[*it_nom+"_"+it_sys->first+"Down_ori"];
            cout<<hsm->GetMaximum()<<endl;
            draw_pre(hsm, hmc, it_sys->first, "_" + *it_nom + cutname + Form("_%d", year), range, path);
        }
    }
    file->Close();
    file_ori->Close();
}