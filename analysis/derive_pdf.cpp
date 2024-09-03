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
void Convert(TH1D** histo1, TH1D** histo2, TH1D* rehist_max, TH1D* rehist_min, int num){
    Double_t max, min;
    int nbins = histo1[0]->GetNbinsX();
    for (int i=0;i<histo1[0]->GetNbinsX();i++){
        max=histo1[0]->GetBinContent(i+1);
        min=histo1[0]->GetBinContent(i+1);
        for(int m=1; m<num; m++){
            if(max<histo1[m]->GetBinContent(i+1))
                max=histo1[m]->GetBinContent(i+1);
            if(min>histo1[m]->GetBinContent(i+1))
                min=histo1[m]->GetBinContent(i+1);
            rehist_max->SetBinContent(i+1, max);
            rehist_min->SetBinContent(i+1, min);
        }
    }
    for (int i=0;i<histo2[0]->GetNbinsX();i++){
        max=histo2[0]->GetBinContent(i+1);
        min=histo2[0]->GetBinContent(i+1);
        for(int m=1; m<num; m++){
            if(max<histo2[m]->GetBinContent(i+1))
                max=histo2[m]->GetBinContent(i+1);
            if(min>histo2[m]->GetBinContent(i+1))
                min=histo2[m]->GetBinContent(i+1);
            rehist_max->SetBinContent(nbins+i+1, max);
            rehist_min->SetBinContent(nbins+i+1, min);
        }
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
void derive_pre(TString cut_off, TString filename, int year){
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
                            "new_W4JetsToLNu_TuneCP5_13TeV-madgraphMLM.root"};
    TString other_con1 = "*((jet_num>=4)||(jet_num==3 && jet_pt[0]>50))";
    TString other_con2 = "*(lep_flavour||((!lep_flavour) && lepton_pt>34))";
    
    
    TString process[]={"ttbar_ci0000","ttbar_ci0100", "ttbar_ci0010", "ttbar_ci0001", "DYJets","STop", "WJets", "QCD"};
    //TString dir="/eos/user/y/yuekai/output3/";
    int edge_i[] = {0, 0, 0, 0, 3, 11, 16};//23,31}; 
    int edge_f[] = {3, 3, 3, 3, 11, 16, 20,}; 
    float lumi = 59.83;
    TString EW_weight[4]={"weight_ci0000","weight_ci0100","weight_ci0010","weight_ci0001"};
    UInt_t nweight;
    TH1D *h1_max[10], *h1_min[10]; 
    TH1D *h2_max, *h2_min;
    TH1D *hist1[150], *hist2[150];
    TString path = Form("./%d/",year);
    TFile *file = new TFile(path+"pdf_w/"+filename,"recreate");
    Double_t xbins[2][20] = {{0,340,360,380,400,420,440,460,480,500,520,540,570,600,640,700,3000}, {0,450,500,570,630,700,820,3000}};
    int nbins[2] = {16, 7};
    TString div_con[] = {"*(rapidity_tt<1.4 && rapidity_tt>-1.4)", "*(rapidity_tt>1.4 || rapidity_tt<-1.4)"};
    int bin_num = nbins[0] + nbins[1];
    //fileNames[0]=fileNames[0].ReplaceAll(".root","_1*.root");
    for(int i=0;i<nsample;i++)
        fileNames[i]=fileNames[i].ReplaceAll(".root","_*.root");
    for(int i=0; i<7; i++){
        cout<<"this is "<<process[i]<<endl;
        for(int j=edge_i[i]; j<edge_f[i]; j++){
            cout<<"\t\t this is "<<fileNames[j]<<endl;
            TChain *tree = new TChain("mytree");
            tree->Add(path+"MC/"+fileNames[j]);
            tree->SetBranchAddress("nLHEPdfWeight",&nweight);
            tree->GetEntry(1);
            TString weight = "Generator_weight*SF_btag*SF_lepton*L1PreFiringWeight_Nom"+other_con1+other_con2;
            renew_weight(&weight, fileNames[j], j, path);
            if(i < 4)
                weight = weight+"*nnlo_wt*"+EW_weight[i];
            for(int num=0;num<nweight;num++){
                hist1[num]=new TH1D(Form("hist1_%d",num), "", nbins[0], xbins[0]);
                hist2[num]=new TH1D(Form("hist2_%d",num), "", nbins[1], xbins[1]);

                hist1[num]->Sumw2();
                hist2[num]->Sumw2();
                tree->Draw(Form("mass_tt>>hist1_%d",num),weight+Form("*LHEPdfWeight[%d]*",num)+cut_off+div_con[0]);
                tree->Draw(Form("mass_tt>>hist2_%d",num),weight+Form("*LHEPdfWeight[%d]*",num)+cut_off+div_con[1]);
            }
            h2_max=new TH1D("h2_max", "", bin_num, 0, bin_num);
            h2_min=new TH1D("h2_min", "", bin_num, 0, bin_num);
            h2_max->Sumw2();
            h2_min->Sumw2();
            Convert(hist1, hist2, h2_max, h2_min, nweight);
            if(j == edge_i[i]){
                h1_max[i]=(TH1D*)h2_max->Clone();
                h1_max[i]->SetName(process[i]+"_pdfUp");
                h1_min[i]=(TH1D*)h2_min->Clone();
                h1_min[i]->SetName(process[i]+"_pdfDown");
            }
            else{
                h1_max[i]->Add(h2_max);
                h1_min[i]->Add(h2_min);
            }
            delete tree;
            delete h2_min;
            delete h2_max;
            for(int num=0; num<nweight; num++){
                delete hist1[num];
                delete hist2[num];
            }
        }
    }
    file->cd();
    for(int i=0; i<7; i++){
        h1_max[i]->Write();
        h1_min[i]->Write();
        delete h1_max[i];
        delete h1_min[i];
    }
    file->Close();
}
void derive_pdf(int i){
    TString cuts[] = {"(jet_num == 3 && likelihood<20.0 && (!lep_flavour))","(jet_num >= 4 && (!lep_flavour))",
        "(jet_num == 3 && likelihood<20.0 && lep_flavour)",  "(jet_num >= 4  && lep_flavour)"};
    TString filename[4]={"pdfw_E_3jets.root", "pdfw_E_4jets.root", "pdfw_M_3jets.root", "pdfw_M_4jets.root"};
    int year = 2018;
    cout<<"start to calculate "<<i<<":"<<endl;
    derive_pre(cuts[i], filename[i], year);
}