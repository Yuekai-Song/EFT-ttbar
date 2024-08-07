#include <TROOT.h>
#include <TStyle.h>
#include <Math/PdfFuncMathCore.h>
#include <TFile.h>
#include <TChain.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <TLegend.h>
#include <TF1.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TH2D.h>
#include <TH3D.h>
#include <THStack.h>
#include <string.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include <fstream>
#include <iostream>
#include <RooFit.h>
#include <RooRealVar.h>
#include <TKey.h>
#include <map>
using namespace std;
using namespace RooFit;
typedef struct
{
    int id;
    TString name;
    TString bin;
    double yield;
} process;
bool compare(process pro1, process pro2)
{
    return pro1.id < pro2.id;
}
map<int, TString> sys_lumi_year = {{2015, "1.012"}, {2016, "1.012"}, {2017, "1.023"}, {2018, "1.025"}};
map<TString, TString> sys_norm_bg = {{"STop", "1.15"}, {"DYJets", "1.30"}, {"WJets", "1.30"}, {"Eta", "2.0"}};
void sys_and_nom(TString hist_name, TString &sys_name, TString &nom_name)
{
    int pos = 0;
    char *s = const_cast<char *>(hist_name.Data());
    while (s[pos] != '_')
        pos++;
    if (hist_name.Contains("ttbar"))
    {
        pos++;
        while (s[pos] != '_')
            pos++;
    }
    s[pos] = 0;
    nom_name = TString(s);
    sys_name = TString(s + pos + 1);
}
bool Find_contains(std::vector<TString> sys_names, TString sys_name)
{
    for (int i = 0; i < sys_names.size(); i++)
    {
        if (sys_name.Contains(sys_names[i]))
            return true;
    }
    return false;
}
void write_pro(vector<process> pro_v, ofstream &card)
{
    card << "bin \t";
    for (int i = 0; i < pro_v.size(); i++)
        card << pro_v[i].bin << "\t";
    card << endl;
    card << "process \t";
    for (int i = 0; i < pro_v.size(); i++)
        card << pro_v[i].name << "\t";
    card << endl;
    card << "process \t";
    for (int i = 0; i < pro_v.size(); i++)
        card << pro_v[i].id << "\t";
    card << endl;
    card << "rate \t";
    card << std::fixed;
    card << std::setprecision(3);
    for (int i = 0; i < pro_v.size(); i++)
        card << pro_v[i].yield << "\t";
    card << endl;
}
void write_sys(vector<process> pro_v, map<pair<TString, TString>, map<TString, TString>> sys_map, ofstream &card)
{
    vector<bool> nom_index;
    bool in;
    for (map<pair<TString, TString>, map<TString, TString>>::iterator iter = sys_map.begin(); iter != sys_map.end(); iter++)
    {
        nom_index.clear();
        in = false;
        for (int i = 0; i < pro_v.size(); i++)
        {
            auto it = (iter->second).find(pro_v[i].name);
            if (it != (iter->second).end())
            {
                nom_index.push_back(true);
                in = true;
            }
            else
                nom_index.push_back(false);
        }
        if (!in)
            continue;
        card << iter->first.second << "\t " << iter->first.first << " \t";
        for (int i = 0; i < nom_index.size(); i++)
        {
            if(nom_index[i])
                card << (iter->second)[pro_v[i].name] << "\t";
            else
                card << "-\t";
        }
        card << endl;
    }
}
bool is_small_effect(TH1D hist_nom, TH1D hist_up, TH1D hist_dn)
{
    bool is_big = false;
    double up, dn, nom;
    for (int i = 0; i < hist_nom.GetNbinsX(); i++)
    {
        up = hist_up.GetBinContent(i + 1);
        dn = hist_dn.GetBinContent(i + 1);
        nom = hist_nom.GetBinContent(i + 1);
        if (up / nom > 1.003 || dn / nom < 0.997)
            is_big = true;
    }
    return !is_big;
}
void write_card(ofstream &card, TString dir, TString category, std::vector<process> pro_v,
                map<pair<TString, TString>, map<TString, TString>> sys_map, bool autostat)
{
    card << "Datacard for event category: " << category << endl;
    card << "imax 1 number of channels" << endl;
    card << Form("jmax %lu number of processes minus 1", pro_v.size() - 1) << endl;
    card << "kmax * number of nuisance parameters" << endl;
    card << "---------------------------------" << endl;
    card << endl;
    if (dir != "")
        card << "shapes * " << category << " " 
             << "../" + category + ".root $PROCESS $PROCESS_$SYSTEMATIC" << endl;
    else
        card << "shapes * " << category << " " 
             << "./" + category + ".root $PROCESS $PROCESS_$SYSTEMATIC" << endl;
    card << "---------------------------------" << endl;
    card << "bin           " << category << endl;
    card << "observation   "
         << "-1" << endl;
    card << "---------------------------------" << endl;
    card << endl;
    write_pro(pro_v, card);
    // write_sys("cms_lumi", cms_lumi, card);
    write_sys(pro_v, sys_map, card);
    if(autostat)
        card << "* autoMCStats 0" << endl;
    /*card<<"sig_norm"<<"\t lnN \t";
    writeline(sig_norm);
    card<<"DYJets_norm"<<"\t lnN \t";
    writeline(DYJets_norm);
    card<<"STop_norm"<<"\t lnN \t";
    writeline(STop_norm);
    card<<"WJets_norm"<<"\t lnN \t";
    writeline(WJets_norm);*/
    // card<<"qcdn"<<"\t lnN \t";
    // writeline(qcd_n, card);
}

void write(TString datacard_name, TString dir, TString cut_name, int year, bool lnN_bg,
           vector<TString> lnNed_sys, vector<TString> saved, vector<TString> sys_of_shapeU,
           bool autostat, vector<TString> pro_drop)
{
    TString path = "./datacards/" + datacard_name + "/" + dir;
    TString category = "ttbar" + cut_name + Form("_%d", year);
    cout << path + category + ".txt" << endl;

    ofstream card;
    card.open(path + category + ".txt");
    TFile *file = TFile::Open("./datacards/" + datacard_name + "/" + category + ".root");
    TList *list = file->GetListOfKeys();
    TKey *key;
    TIter iter(list);
    static TString classname("TH1D");
    TString hist_name;

    int num_sig = 0, num_back = 0;
    process pro;
    TString sys_name, nom_name;
    std::vector<process> pro_v;
    map<pair<TString, TString>, map<TString, TString>> sys_map;
    map<TString, TH1D> hist_map;
    while ((key = (TKey *)iter()))
    {
        if (key->GetClassName() == classname)
        {
            TH1D *hist = (TH1D *)key->ReadObj();
            if (hist)
            {
                hist_name = TString(hist->GetName());
                hist_map[hist_name] = *hist;
                // cout<<hist_name<<endl;
                delete hist;
            }
        }
    }
    for (map<TString, TH1D>::iterator iter = hist_map.begin(); iter != hist_map.end(); iter++)
    {
        
        hist_name = iter->first;
        if (hist_name.Contains("Up"))
        {
            hist_name.ReplaceAll("Up", "");
            sys_and_nom(hist_name, sys_name, nom_name);
            if (!nom_name.Contains("ttbar") && !nom_name.Contains("QCD") && lnN_bg)
                continue;
            if (find(saved.begin(), saved.end(), sys_name) == saved.end())
                continue;
            if (find(lnNed_sys.begin(), lnNed_sys.end(), sys_name) != lnNed_sys.end())//is_small_effect(hist_map[nom_name], hist_map[hist_name + "Up"], hist_map[hist_name + "Down"]))
            {
                sys_map[{"lnN", sys_name}][nom_name] = Form("%.6f/%.6f", hist_map[hist_name + "Up"].GetSumOfWeights() / hist_map[nom_name].GetSumOfWeights(),
                                                hist_map[hist_name + "Down"].GetSumOfWeights() / hist_map[nom_name].GetSumOfWeights());
                continue;
            }
            if (find(sys_of_shapeU.begin(), sys_of_shapeU.end(), sys_name) != sys_of_shapeU.end())
                sys_map[{"shapeU", sys_name}][nom_name] = "1";
            else
                sys_map[{"shape", sys_name}][nom_name] = "1";
        }
        else if (!hist_name.Contains("Down") && !hist_name.Contains("EW_no") && !hist_name.Contains("data_obs"))
        {
            if (find(saved.begin(), saved.end(), "cms_lumi") != saved.end())
                sys_map[{"lnN", Form("cms_lumi_%d", year)}][hist_name] = sys_lumi_year[year];

            if (find(saved.begin(), saved.end(), "stat") != saved.end() && hist_name.Contains("ttbar"))
                sys_map[{"lnN", "stat"}][hist_name] = "1.000001";
            
            if (!hist_name.Contains("ttbar") && !hist_name.Contains("Eta") && !hist_name.Contains("QCD") && lnN_bg)
                sys_map[{"lnN", hist_name + "_norm"}][hist_name] = sys_norm_bg[hist_name];
        
            if (find(pro_drop.begin(), pro_drop.end(), hist_name) != pro_drop.end())
                continue;
            pro.name = hist_name;
            pro.yield = hist_map[pro.name].GetSumOfWeights();
            pro.bin = category;
            if (pro.name.Contains("ttbar"))
            {
                pro.id = -num_sig;
                num_sig++;
            }
            else
            {
                pro.id = num_back + 1;
                num_back++;
            }
            pro_v.push_back(pro);
            // need to add some lnN sys manually
        }
    }
    sys_map[{"lnN", "Eta_norm"}]["Eta"] = sys_norm_bg["Eta"];
    sort(pro_v.begin(), pro_v.end(), compare);
    write_card(card, dir, category, pro_v, sys_map, autostat);
    card.close();
    file->Close();
}