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
#include <thread>
#include <mutex>
#include "settings.h"
#include "sys.h"
using namespace std;
class prepare
{
private:
    std::mutex _lock;
    TString category;
    TFile *file;
    var xvar, yvar, zvar;
    int op;
    // int bin_num;
    int npdf_w[10];
    TString process[10], EW[5], fileTitle[10];
    TString weight_up, weight_dn;
    TString file_up, file_dn;
    TString tree_up, tree_dn;
    bool for_pre;
    vector<TH1 *> qcd_hists;
    TH1* h_other = nullptr;
    bool use_other = false;
    const TString weight_base = "Generator_weight*SF_btag*SF_lepton*pu_wt*L1PreFiringWeight_Nom";
    void draw(TH1 *h1, TString file, TString tree, TString weight = "");
    void set_dir();
    void draw_nom();
    void draw_sys(stype s);
    void draw_pdf(int p);
    void draw_qcd(TString cg, bool isEnriched);
    void draw_derived(TString cg);
    void add_qcd();
    void draw_data();
    settings sets;
    void create_hist(TH1 *&hist, TString name);
    void write(TH1 *hist);
    sys *psys;
public:
    const int npdf = 51;
    const int npro = 10;
    const int nEW = 5;
    int begin, end;
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