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
#include<TH2D.h>
#include<TH3D.h>
#include<THStack.h>
#include<string.h>
#include<TGraph.h>
#include<TGraphAsymmErrors.h>
#include<fstream>
#include<iostream>
#include<RooFit.h>
#include<RooRealVar.h>
#include<TKey.h>
#include<vector>
#include<map>
using namespace std;
TString sys_to_nom(TString h1_sys_name){
    int pos = 0;
    char* s = const_cast<char*>(h1_sys_name.Data());
    while(s[pos] != '_')
        pos++;
    if(h1_sys_name.Contains("ttbar")){
        pos++;
        while(s[pos] != '_')
            pos++;
    }
    s[pos] = 0;
    return TString(s);
}
void sum_TH1D(TH1D* hs, TH1D** h1, int* start, int num){
    for(int f=0; f<num; f++){
        for(int i=0; i<h1[f]->GetNbinsX(); i++){
            hs->SetBinContent(i+1+start[f], h1[f]->GetBinContent(i+1));
            hs->SetBinError(i+1+start[f], h1[f]->GetBinError(i+1));
        }
    }
    hs->ResetStats();
}
void get_TH1D(TH1D* h1, TString h1_name, TH3D* h3, int like_cut, int ycut_low, int ycut_up, int* xbins, int nbins){
    h1->SetBins(nbins, 0, nbins);
    h1->SetName(h1_name);
    TH1D* h3_1 = h3->ProjectionX("3_px", ycut_low, ycut_up, 0, like_cut);

    double value;
    double err2;
    for(int i=0; i<nbins; i++){
        value = 0;
        err2 = 0;
        for(int bin=xbins[i]; bin<xbins[i+1]; bin++){
            value += h3_1->GetBinContent(bin+1);
            err2 += h3_1->GetBinError(bin+1)*h3_1->GetBinError(bin+1);
        }
        h1->SetBinContent(i+1, value);
        h1->SetBinError(i+1, sqrt(err2));
    }
    //cout<<h1->GetSumOfWeights()<<endl;
    delete h3_1;
}
void pdf_convert(TH1D hist, TH1D hist_nom, TH1D* hist_up, TH1D* hist_dn){
    for(int bin=0; bin<hist.GetNbinsX(); bin++){
        double up = hist.GetBinContent(bin+1);
        double nom = hist_nom.GetBinContent(bin+1);
        double dn;
        if(up == 0)
            dn = 0;
        else
            dn = nom*nom/up;
        hist_up->SetBinContent(bin+1, up);
        hist_dn->SetBinContent(bin+1, dn);
        hist_up->SetBinError(bin+1, hist.GetBinError(bin+1));
        hist_dn->SetBinError(bin+1, hist.GetBinError(bin+1));
    }
}
void convert(TString input, TString output, double likelihood_cut, vector<double> ycut_user, vector<vector<double>> xbins_user){
    //convert
    int like_cut;
    if(likelihood_cut <= 50.0 && likelihood_cut >= 13.0)
        like_cut = int(likelihood_cut-13.0);
    else
        like_cut = -1;
    const int nycut = ycut_user.size();
    int ycut[nycut+1];
    for(int i=1; i<nycut; i++)
        ycut[i] = int((ycut_user[i]-0.0)/0.1);
    ycut[nycut] = 41;
    ycut[0] = -1;

    int xbins[nycut][20];
    int nbins[nycut];
    for(int i=0; i<nycut; i++){
        nbins[i] = xbins_user[i].size() - 1;
        //xbins[i] = new int[nbins[i]+1];
        for(int j=0; j<nbins[i]+1; j++){
            if(xbins_user[i][j]<300)
                xbins[i][j] = -1;
            else if(xbins_user[i][j]>3000)
                xbins[i][j] = 271;
            else
                xbins[i][j] = int((xbins_user[i][j]-300)/10);
        }
    }
    TFile *f = new TFile(input);
    TList *list = f->GetListOfKeys();
    TFile *outFile = new TFile(output, "recreate");
    TKey *key;
    TIter iter(list); //or TIter iter(list->MakeIterator());
    static TString classname("TH3D");
    int start[nycut+1];
    int bin_num = 0;
    for(int i=0; i<nycut; i++){
        start[i] = bin_num;
        bin_num += nbins[i];      
    }
    start[nycut] = bin_num;
    TString hist_name;
    map<TString, TH1D> hist_map;
    while((key = (TKey*)iter())) {
        if(key->GetClassName() == classname) {
            TH3D* hist3 = (TH3D*)key->ReadObj();
            if(hist3) {
                hist_name = TString(hist3->GetName());
                hist_name.ReplaceAll("_sub", "");
                if(hist_name.Contains("pdf"))//pdf name wrong
                    hist_name.ReplaceAll("pdf", "_pdf");
                cout<<hist_name<<endl;
                TH1D* hists = new TH1D(hist_name, "", bin_num, 0 ,bin_num);
                TH1D* hist1[nycut];
                for(int f=0; f<nycut; f++){
                    hist1[f] = new TH1D;
                    get_TH1D(hist1[f], hist_name+Form("_%d", f), hist3, like_cut, ycut[f]+1, ycut[f+1], xbins[f], nbins[f]);
                }
                sum_TH1D(hists, hist1, start, nycut);
                hist_map[hist_name] = *hists;
                delete hist3;
                delete hists;
                for(int i=0; i<nycut; i++)
                    delete hist1[i];
            }
        }
    }
    for(map<TString, TH1D>::iterator iter=hist_map.begin(); iter!=hist_map.end(); iter++){
        if(iter->first.Contains("pdf")){
            TH1D* hists_up = new TH1D(iter->first+"Up", "", bin_num, 0 ,bin_num);
            TH1D* hists_dn = new TH1D(iter->first+"Down", "", bin_num, 0 ,bin_num);
            pdf_convert(iter->second, hist_map[sys_to_nom(iter->first)], hists_up, hists_dn);
            outFile->cd();
            hists_up->Write();
            hists_dn->Write();
            delete hists_up, hists_dn;
        }
        else{
            outFile->cd();
            iter->second.Write();
        }
    }
    outFile->Close();
}