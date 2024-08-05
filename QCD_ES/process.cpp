#include "../select/select_tree.cpp"
void process(TString outdir, TString outputFile, TString input, int year, int data_type, bool ttx, int cates){
    int num_j, num_e, num_m, num_g = 0;
    read_object r(input, data_type);
    OP_TYPE op_type;
    OBJECT_SELECT_ORDER order_type;
    select_tree *s;
    num_j = r.nj;
    num_e = r.ne;
    num_m = r.nm;
    num_g = r.ng;
    CATEGORY cate;
    switch (cates)
    {
    case 0:
        cate = CATEGORY::A;
        break;
    case 1:
        cate = CATEGORY::B;
        break;
    case 2:
        cate = CATEGORY::C;
        break;
    case 3:
        cate = CATEGORY::D;
        break;
    }
    TString names[] = {"_A.root", "_B.root", "_C.root", "_D.root"};
    outputFile.ReplaceAll(".root", names[cates]);
    order_type = OBJECT_SELECT_ORDER::lepton_jet;
    if (ttx)
    {
        op_type = select_reco_ttx;
        outputFile.ReplaceAll(".root", "_ttx.root");
    }
    else
        op_type = select_reco;
    // num_j = 50;
    // num_e = 20;
    // num_m = 20;
    // num_g = 50;
    TFile* dis_file = TFile::Open(Form("../select/dis/dis_%d.root", year));
    RECO::mth_vs_mwh_ttx_4 = (TH2D*)dis_file->Get("mth_vs_mwh_ttx_4");
    RECO::mth_vs_mwh_4 = (TH2D*)dis_file->Get("mth_vs_mwh_4");
    RECO::mtl_vs_mwl_4 = (TH2D*)dis_file->Get("mtl_vs_mwl_4");
    RECO::mtl_vs_mwl_3 = (TH2D*)dis_file->Get("mtl_vs_mwl_3");
    RECO::mth_3 = (TH1D*)dis_file->Get("mth_3");
    RECO::mth_ttx_3 = (TH1D*)dis_file->Get("mth_ttx_3");
    RECO::Dnu_4 = (TH1D*)dis_file->Get("Dnu_4");
    RECO::Dnu_3 = (TH1D*)dis_file->Get("Dnu_3");
    TFile* tf_file;
    if (ttx)
        tf_file = TFile::Open(Form("../select/corr/tf_ttx/tf_%d.root", year));
    else
        tf_file = TFile::Open(Form("../select/corr/tf/tf_%d.root", year));
    select_tree::h_ecorr = (TF1 *)tf_file->Get("Func");
    TFile* map_file;
    if (year == 2015 || year == 2016)
    {
        map_file = TFile::Open("../select/jet_veto_maps/hotjets-UL16.root");
        select_tree::jet_veto_map = (TH2D *)map_file->Get("h2hot_ul16_plus_hbm2_hbp12_qie11");
        TH2D *hot_mc = (TH2D *)map_file->Get("h2hot_mc");
        select_tree::jet_veto_map->Add(hot_mc);
    }
    else if (year == 2017)
    {
        map_file = TFile::Open("../select/jet_veto_maps/hotjets-UL17_v2.root");
        select_tree::jet_veto_map = (TH2D *)map_file->Get("h2hot_ul17_plus_hep17_plus_hbpw89");
    }
    else
    {
        map_file = TFile::Open("../select/jet_veto_maps/hotjets-UL18.root");
        select_tree::jet_veto_map = (TH2D *)map_file->Get("h2hot_ul18_plus_hem1516_and_hbp2m1");
    }

    if(data_type == 0)
        s = new select_tree(input, outdir+"/"+"new_"+outputFile, "mytree", "Jet_pt", "MET_pt", year, DATA_TYPE::data, op_type, order_type, cate, num_j, num_e, num_m);
    else
        s = new select_tree(input, outdir+"/"+"new_"+outputFile, "mytree", "Jet_pt_nom", "MET_T1Smear_pt", year, DATA_TYPE::MC_sys, op_type, order_type, cate, num_j, num_e, num_m, num_g);
    s->write();
    delete s;
    dis_file->Close();
    tf_file->Close();
    map_file->Close();
}