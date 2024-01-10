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
using namespace std;
using namespace RooFit;

class prepare_3D
{
private:
    int begin, end;
    TString category;
    TFile *file;
    int year;
    TString dir, outputDir, qcd_dir, pdf_dir;

    int xbins, ybins, zbins;
    double xlow, xup, ylow, yup, zlow, zup;
    // int bin_num;
    TString fileNames[20];
    map<TString, pair<double, double>> xsection;
    TString cut, cut_name;
    int edge_i[9], edge_f[9];
    int npdf_w[9];
    TString sys_n[30], sys[30], sys_up[30], sys_down[30];
    TString process[9];
    TString EW[5];
    TString weight_up, weight_dn;
    TString file_up, file_dn;
    TString tree_up, tree_dn;
    TString other_con1, other_con2;

    void give_sys_name(TString file, TString weight, int s, int c);
    void renew_weight(TString *weight, TString file);
    void draw(TH3D *h1, TString file, TString tree, TString weight);
    void set_dir(int option);
    void draw(int c);
    void draw_sys(int c, int s);
    void draw_pdf(int c, int p);
    void draw_data();

public:
    prepare_3D(TString cut_s, TString cut_name_s, int year_s, int *xy_bins, double *xy_range, int option);
    ~prepare_3D();
};