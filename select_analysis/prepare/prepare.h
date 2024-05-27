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
    int xbins, ybins, zbins;
    double xlow, xup, ylow, yup, zlow, zup;
    double *xbin;
    // int bin_num;
    map<TString, pair<double, double>> xsection;
    int npdf_w[9];
    TString sys_n[31], sys[31], sys_up[31], sys_down[31];
    TString process[9], EW[5], fileTitle[9];
    TString weight_up, weight_dn;
    TString file_up, file_dn;
    TString tree_up, tree_dn;
    bool is_corr;
    map<TString, TH1 *> qcd_hists;
    const TString weight_base = "Generator_weight*SF_btag*SF_lepton*pu_wt*L1PreFiringWeight_Nom";
    void give_sys_name(TString file, TString weight, int s, int c);
    void renew_weight(TString *weight, TString dir, TString file);
    void draw(TH1 *h1, TString file, TString tree, TString weight);
    void set_dir(int option);
    void draw_nom(int c);
    void draw_sys(int c, int s);
    void draw_pdf(int c, int p);
    void draw_qcd(TString cg, bool isEnriched);
    void add_qcd();
    void draw_data();
    TString xvar = "";
    int type;
    settings sets;
    void create_hist(TH1 *&hist, TString name);

public:
    TString MC_dir, data_dir, outputDir, QCD_dir;
    void set_bins(bool is_corrs);
    void set_bins(TString xvar_s, TString title, int nbin, double *xbin);
    void set_bins(TString xvar_s, TString title, int nbin, double xlow_s, double xup_s);
    prepare(int cut_num, int year_s, bool is_ttx, int option);
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