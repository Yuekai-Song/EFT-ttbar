// build 2D RooHistPdf and datacard file
// for ttbar and its background
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
#include <TH2D.h>
#include <TH3D.h>
#include <TCanvas.h>
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
#include <vector>
#include "settings.h"
using namespace std;
using namespace RooFit;

class prepare
{
private:
    int begin, end;
    TString category;
    TFile *file;
    int year;
    var xvar, yvar, zvar;
    // int bin_num;
    map<TString, pair<double, double>> xsection;
    int npdf_w[10];
    TString sys_n[32], sys[32], sys_up[32], sys_down[32];
    TString process[10], EW[5], fileTitle[10];
    TString weight_up, weight_dn;
    TString file_up, file_dn;
    TString tree_up, tree_dn;
    bool for_pre;
    vector<TH1 *> qcd_hists;
    const TString weight_base = "Generator_weight*SF_btag*SF_lepton*pu_wt*L1PreFiringWeight_Nom";
    void give_sys_name(TString file, TString weight, int s, int c);
    double renew_weight( TString dir, TString file);
    void draw(TH1 *h1, TString file, TString tree, TString weight, double gw);
    void set_dir(int option);
    void draw_nom(int c);
    void draw_sys(int c, int s);
    void draw_pdf(int c, int p);
    void draw_qcd(TString cg, bool isEnriched);
    void draw_derived(TString cg);
    void add_qcd(bool ready = false);
    void draw_data();
    settings sets;
    void create_hist(TH1 *&hist, TString name);

public:
    TString MC_dir, data_dir, outputDir;
    void set_bins(var xvars, var yvars =  {"", 0, 0, 0}, var zvars = {"", 0, 0, 0});
    prepare(int cut_num, int year_s, bool is_ttx, int option, TString sg, bool ready = false);
    void run();
    ~prepare();
};
void set0(TH1 *h1)
{
    if ((TString)h1->ClassName() != "TH1D")
        return;
    for (int i = 0; i <= h1->GetNbinsX() + 1; i++)
    {
        if (h1->GetBinContent(i) < 0)
        {
            h1->SetBinContent(i, 0);
            h1->SetBinError(i, 0);
        }
    }
}