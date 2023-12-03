#include "select_tree.cpp"
void process_corr(TString outdir, TString outputFile, TString input, int year){
    TString tree_name = "mytree";
    TString jet_name = "Jet_pt";
    TString MET_name = "MET_pt";
    
    int num_j, num_e, num_m, num_g;
    read_object r(input, 1);
    select_tree *s;
    num_j = r.nj;
    num_e = r.ne;
    num_m = r.nm;
    num_g = r.ng;
    /*num_j = 50;
    num_e = 20;
    num_m = 20;
    num_g = 50;*/
    TFile* dis_file = TFile::Open(Form("./dis/dis_%d.root", year));
    RECO::mth_vs_mwh_ttx_4 = (TH2D*)dis_file->Get("mth_vs_mwh_ttx_4");
    RECO::mth_vs_mwh_4 = (TH2D*)dis_file->Get("mth_vs_mwh_4");
    RECO::mtl_vs_mwl_4 = (TH2D*)dis_file->Get("mtl_vs_mwl_4");
    RECO::mtl_vs_mwl_3 = (TH2D*)dis_file->Get("mtl_vs_mwl_3");
    RECO::mth_3 = (TH1D*)dis_file->Get("mth_3");
    RECO::mth_ttx_3 = (TH1D*)dis_file->Get("mth_ttx_3");
    RECO::Dnu_4 = (TH1D*)dis_file->Get("Dnu_4");
    RECO::Dnu_3 = (TH1D*)dis_file->Get("Dnu_3");
    TFile* tf_file = TFile::Open(Form("./corr/tf_%d.root", year));
    select_tree::ecorr = (TF1 *)tf_file->Get("fitFunc");
    s = new select_tree(input, outdir+"/"+"new_"+outputFile, tree_name, jet_name, MET_name, year, MC, e_corr_3jets, num_j, num_e, num_m, num_g);
    s->write();
    delete s;
    dis_file->Close();
    tf_file->Close();
}