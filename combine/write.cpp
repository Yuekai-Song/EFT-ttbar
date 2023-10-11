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
#include<map>
using namespace std;
using namespace RooFit;
typedef struct{
    int id;
    TString name;
    TString bin;
    double yield;
}process;
bool compare(process pro1, process pro2){
    return pro1.id < pro2.id;
}
TString sys_lumi_year[] = {"1.012", "1.012", "1.023", "1.025"};
void sys_and_nom(TString hist_name, TString& sys_name, TString& nom_name){
    int pos = 0;
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
void write_pro(vector<process> pro_v, ofstream& card){
    card<<"bin \t";
    for(int i=0; i<pro_v.size(); i++)
        card<<pro_v[i].bin<<"\t";
    card<<endl;
    card<<"process \t";
    for(int i=0; i<pro_v.size(); i++)
        card<<pro_v[i].name<<"\t";
    card<<endl;
    card<<"process \t";
    for(int i=0; i<pro_v.size(); i++)
        card<<pro_v[i].id<<"\t";
    card<<endl;
    card<<"rate \t";
    card<< std::fixed;
    card<< std::setprecision(3);
    for(int i=0; i<pro_v.size(); i++)
        card<<pro_v[i].yield<<"\t";
    card<<endl;
}
void write_sys(vector<process> pro_v, map<TString, std::vector<TString>> sys_nom, ofstream& card){
    for(map<TString, std::vector<TString>>::iterator iter=sys_nom.begin(); iter!=sys_nom.end(); iter++){
        card<<iter->first<<"\t shape \t";
        for(int i=0; i<pro_v.size(); i++){
            auto it = find((iter->second).begin(), (iter->second).end(), pro_v[i].name);
            if(it != (iter->second).end())
                card<<"1\t";
            else
                card<<"-\t";
        }
        card<<endl;
    }
}
void write_sys(vector<process> pro_v, map<TString, map<TString, TString>> sys_lnN, ofstream& card){
    for(map<TString, map<TString, TString>>::iterator iter=sys_lnN.begin(); iter!=sys_lnN.end(); iter++){
        card<<iter->first<<"\t lnN \t";
        for(int i=0; i<pro_v.size(); i++){
            auto it = (iter->second).find(pro_v[i].name);
            if(it != (iter->second).end())
                card<<(iter->second)[pro_v[i].name]<<"\t";
            else
                card<<"-\t";
        }
        card<<endl;
    }
}
void write_card(ofstream& card, TString category, std::vector<process> pro_v, map<TString, std::vector<TString>> sys_shape, map<TString, map<TString, TString>> sys_lnN){
    card <<"Datacard for event category: "<< category << endl;
    card<< "imax 1 number of channels"<<endl;
    card<< Form("jmax %lu number of processes minus 1", pro_v.size()-1)<<endl;
    card<< "kmax * number of nuisance parameters"<<endl;
    card<<"---------------------------------"<<endl;
    card<<endl;
    card<< "shapes * "<< category << " "<< category+".root $PROCESS $PROCESS_$SYSTEMATIC" <<endl;
    card<<"---------------------------------"<<endl;
    card<< "bin           "<< category <<endl;
    card<< "observation   "<< "-1"<<endl;
    card<<"---------------------------------"<<endl;
    card<<endl;
    write_pro(pro_v, card);
    //write_sys("cms_lumi", cms_lumi, card);
    write_sys(pro_v, sys_lnN, card);
    write_sys(pro_v, sys_shape, card);
    /*card<<"sig_norm"<<"\t lnN \t";
    writeline(sig_norm);
    card<<"DYJets_norm"<<"\t lnN \t";
    writeline(DYJets_norm);
    card<<"STop_norm"<<"\t lnN \t";
    writeline(STop_norm);
    card<<"WJets_norm"<<"\t lnN \t";
    writeline(WJets_norm);*/
    //card<<"qcdn"<<"\t lnN \t";
    //writeline(qcd_n, card);
}

void write(TString datacard_name, TString cut_name, int year){
    TString path = "./"+datacard_name;
    TString category="ttbar"+cut_name+Form("_%d", year);
    cout<<path+category+".txt"<<endl;

    ofstream card;
    card.open (path+category+".txt");
    TFile *file = TFile::Open(path+category+".root");
    TList *list = file->GetListOfKeys();
    TKey *key;
    TIter iter(list);
    static TString classname("TH1D");
    TString hist_name;

    int num_sig = 0, num_back = 0;
    process pro;
    TString sys_name, nom_name;
    std::vector<process> pro_v;
    map<TString, std::vector<TString>> sys_shape;
    map<TString, map<TString, TString>> sys_lnN;
    while((key = (TKey*)iter())) {
        if(key->GetClassName() == classname) {
            TH1D* hist = (TH1D*)key->ReadObj();
            if(hist) {
                hist_name = TString(hist->GetName());
                //cout<<hist_name<<endl;
                if(hist_name.Contains("Up")){
                    hist_name.ReplaceAll("Up", "");
                    sys_and_nom(hist_name, sys_name, nom_name);
                    sys_shape[sys_name].push_back(nom_name);
                }
                else if(!hist_name.Contains("Down") && !hist_name.Contains("EW_no") && !hist_name.Contains("data_obs")){
                    pro.name = hist_name;
                    pro.yield = hist->GetSumOfWeights();
                    pro.bin = category;
                    if(hist_name.Contains("ttbar")){
                        pro.id = -num_sig;
                        num_sig++;
                    }
                    else{
                        pro.id = num_back + 1;
                        num_back++;
                    }
                    pro_v.push_back(pro);
                    sys_lnN["cms_lumi"][hist_name] = sys_lumi_year[year-2015];
                }
                delete hist;
            }
        }
    }
    sort(pro_v.begin(), pro_v.end(), compare);
    write_card(card, category, pro_v, sys_shape, sys_lnN);
    card.close();
    file->Close();
}