//build 2D RooHistPdf and datacard file
//for ttbar and its background
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
#include<TGraphAsymmErrors.h>
#include<fstream>
#include<iostream>
#include<RooFit.h>
#include<RooRealVar.h>
#include<TKey.h>
using namespace std;
using namespace RooFit;

class prepare{
private:
    TFile* file;
    int year;
    TString dir, outputDir;
    //Double_t mtt_edges[9];
    //Double_t ytt_edges[10];
    double xbins[20];
    double xlow, xup;

    int nbins;
    bool equal_bin;
    int signal_num; //number of signals needed

    //int bin_num;
    TString fileNames[20];
    map<TString, pair<double, double>> xsection;
    TString cut, cut_name;
    int edge_i[9], edge_f[9];
    TString sys_n[30], sys[30], sys_up[30], sys_down[30];
    TString process[9];
    TString EW[5];
    TString weight_nom, weight_up, weight_dn;
    TString file_up, file_dn;
    TString tree_up, tree_dn;
    TString xvar, title;
    void Floor(TH1D* hist);
    void give_sys_name(TString file, TString weight, int s, int c);
    void renew_weight(TString* weight, TString file);
    void draw(TH1D* h1, TString file, TString tree, TString weight);
    void set_dir();
    void draw(int c);
    void draw(int c, int s);
    void draw_data();
public:
    prepare(TString dir_s, TString outputDir_s);
    void run(TString cut_s, TString cut_name_s, int year_s, int s_num);
    void set_bins(TString xvar_s, TString title_s, int nbin, double *xbin);
    void set_bins(TString xvar_s, TString title_s, int nbin, double xlow_s, double xup_s);
    ~prepare();
};