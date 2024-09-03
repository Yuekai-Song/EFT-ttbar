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
#include<fstream>
#include<iostream>
using namespace std;
double p2weight=0.65/0.3;
void seterror0(TH1D* h1){
    for(int i=1; i<=h1->GetNbinsX(); i++){
        h1->SetBinError(i,0);
    }
}
void renew_weight(TString* weight, TString file, int j, TString path){
    const int nsample = 20;
    Float_t cross_sections[nsample]={366.91, 89.05, 377.96,
                                    169.9, 147.4, 41.0, 5.7, 1.4, 0.63, 0.15, 0.0036,
                                    3.36, 136.02, 80.95, 35.6, 35.6,
                                    8927.0, 2809.0, 826.3, 544.3,};
    Float_t K_Factor[nsample]={1.0, 1.0, 1.0,
                                1.23,1.23,1.23,1.23,1.23,1.23,1.23,1.23,
                                1.0,1.0,1.0,1.0,1.0,
                                1.21,1.21,1.21,1.21};
    float lumi=59.83;
    auto c1 = new TCanvas("c0", "c0", 8, 30, 600, 600); // temporary canvas
    TChain* tree1=new TChain("rawtree"); 
    tree1->Add(path+"MC/"+file);
    TH1D *nmc=new TH1D("nmc","",50,0,100);
    nmc->Sumw2();
    c1->cd();
    tree1->Draw("nJet>>nmc","Generator_weight");
    //cout<<nmc->GetSumOfWeights()<<endl;
    float weight1=cross_sections[j]*lumi/(nmc->GetSumOfWeights())*K_Factor[j]*1000;
    //cout<<weight1<<endl;
    *weight = Form("%f*", weight1) + (*weight);
    delete tree1;
    delete nmc;
    delete c1;
}
void derive_pre(int cn, int l, int year){
    const int nsample = 20;
    TString fileNames[nsample] = {  
                            "new_TTToSemiLeptonic_TuneCP5_13TeV-powheg.root",
                            "new_TTTo2L2Nu_TuneCP5_13TeV-powheg.root",
                            "new_TTToHadronic_TuneCP5_13TeV-powheg.root",

                            "new_DYJetsToLL_M-50_HT-70to100_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-100to200_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-200to400_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-400to600_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-600to800_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-800to1200_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-1200to2500_TuneCP5_PSweights_13TeV-madgraphMLM.root",                           
                            "new_DYJetsToLL_M-50_HT-2500toInf_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                        
                            "new_ST_s-channel_4f_leptonDecays_TuneCP5_13TeV-amcatnlo.root",
                            "new_ST_t-channel_antitop_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin.root",
                            "new_ST_t-channel_top_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin.root",
                            "new_ST_tW_antitop_5f_inclusiveDecays_TuneCP5_13TeV-powheg.root",
                            "new_ST_tW_top_5f_inclusiveDecays_TuneCP5_13TeV-powheg.root",                                                                               
                            
                            "new_W1JetsToLNu_TuneCP5_13TeV-madgraphMLM.root",
                            "new_W2JetsToLNu_TuneCP5_13TeV-madgraphMLM.root",
                            "new_W3JetsToLNu_TuneCP5_13TeV-madgraphMLM.root",
                            "new_W4JetsToLNu_TuneCP5_13TeV-madgraphMLM.root",
                            };
    TString pro[] = {"_TT", "_DY", "_ST", "_WJets"};
    int edge[]={0,3,11,16,20,29};//23,31}; 
    for(int i=0;i<nsample;i++)
        fileNames[i]=fileNames[i].ReplaceAll(".root","*.root");
    TString file_up, file_down; 
    TString cate[] = {"1","(jet_num==3)","(jet_num>=4)"};
    TString lep[] = {"*1","*(!lep_flavour)","*(lep_flavour)"};
    TString cate_name[] = {"_semi","_3jets","_4jets"};
    TString lep_name[] = {"","_E","_M"};
    TString other_con1 = "*((jet_num>=4)||(jet_num==3 && jet_pt[0]>50))";
    TString other_con2 = "*(lep_flavour||((!lep_flavour) && lepton_pt>34))";
    TString xtitle[] = {"PV_npvs", "PV_npvsGood"};
    TString title[] = {"PV_npvs", "PV_npvsGood"};
    TString xvars[] = {"PV_npvs", "PV_npvsGood"};
    Double_t xup[] = {65, 65};
    Double_t xdown[] = {0, 0};
    Int_t bins[]={65, 65};
    TString legend[] = {"nom", "up", "down"};
    TString weight_up, weight_down;
    TH1D *hmc0, *hmc1, *hmc2, *hmc3;
    TH1D *hist0, *hist1, *hist2, *hist3; 
    TString path = Form("./%d/",year);
    TFile* file = new TFile(path+"pileup/pu"+lep_name[l]+cate_name[cn]+"_.root", "recreate");
    for(int i = 0; i < 2; i++){
        for(int c=0; c<4; c++){
            auto c1 = new TCanvas("c1", "c1", 8, 30, 600, 600); // temporary canvas
            c1->cd();
            for(int j=edge[c]; j<edge[c+1]; j++){
                TString weight = cate[cn]+lep[l]+"*Generator_weight*SF_btag*SF_lepton*L1PreFiringWeight_Nom"+other_con1+other_con2;
                TString weight_nom = cate[cn]+lep[l]+"*Generator_weight*SF_btag*SF_lepton*L1PreFiringWeight_Nom*pu_wt"+other_con1+other_con2;
                TString weight_up = cate[cn]+lep[l]+"*Generator_weight*SF_btag*SF_lepton*L1PreFiringWeight_Nom*pu_wt_up"+other_con1+other_con2;
                TString weight_dn = cate[cn]+lep[l]+"*Generator_weight*SF_btag*SF_lepton*L1PreFiringWeight_Nom*pu_wt_dn"+other_con1+other_con2;
                if(c == 0){
                    weight = weight+"*nnlo_wt";
                    weight_nom = weight_nom+"*nnlo_wt";
                    weight_up = weight_up+"*nnlo_wt";
                    weight_dn = weight_dn+"*nnlo_wt";
                }
                renew_weight(&weight, fileNames[j], j, path);
                renew_weight(&weight_nom, fileNames[j], j, path);
                renew_weight(&weight_up, fileNames[j], j, path);
                renew_weight(&weight_dn, fileNames[j], j, path);
                TChain* tree=new TChain("mytree");
                tree->Add(path+"MC/"+fileNames[j]);
                hist0 = new TH1D("CG0","", bins[i], xdown[i], xup[i]);
                hist1 = new TH1D("CG1","", bins[i], xdown[i], xup[i]);
                hist2 = new TH1D("CG2","", bins[i], xdown[i], xup[i]);
                hist3 = new TH1D("CG3","", bins[i], xdown[i], xup[i]);
                
                hist0->Sumw2();
                hist1->Sumw2();
                hist2->Sumw2();
                hist3->Sumw2();

                c1->cd();
                tree->Draw(xvars[i]+">>CG0", weight);            
                tree->Draw(xvars[i]+">>CG1", weight_nom);  
                tree->Draw(xvars[i]+">>CG2", weight_up);  
                tree->Draw(xvars[i]+">>CG3", weight_dn);      

                if(j==edge[c]){
                    hmc0=(TH1D*)hist0->Clone();
                    hmc0->SetName(title[i]+pro[c]);
                    hmc1=(TH1D*)hist1->Clone();
                    hmc1->SetName(title[i]+pro[c]+"_nom");
                    hmc2=(TH1D*)hist2->Clone();
                    hmc2->SetName(title[i]+pro[c]+"_up");
                    hmc3=(TH1D*)hist3->Clone();
                    hmc3->SetName(title[i]+pro[c]+"_down");
                }
                else{
                    hmc0->Add(hist0);
                    hmc1->Add(hist1);
                    hmc2->Add(hist2);
                    hmc3->Add(hist3);
                }              
                delete tree;
                delete hist0;
                delete hist1;
                delete hist2;
                delete hist3;
            }
            file->cd();
            hmc0->Write();
            hmc1->Write();
            hmc2->Write();
            hmc3->Write();
                
            delete hmc0;
            delete hmc1;
            delete hmc2;
            delete hmc3;
            delete c1;
        }
    }
    file->Close();
}
void derive_pu(int i, int j){
    derive_pre(i, j, 2018);
}