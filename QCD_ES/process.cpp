#include "../select/select_tree.cpp"
void process(TString outdir, TString outputFile, TString input, int year, int data_type, bool ttx, int cates){
    int num_j, num_e, num_m, num_g = 0;
    read_object r(input, data_type);
    OP_TYPE op_type;
    OBJECT_SELECT_ORDER order_type;
    if (ttx)
    {
        op_type = select_reco_ttx;
        order_type = OBJECT_SELECT_ORDER::lepton_jet;
    }
    else
    {
        op_type = select_reco;
        order_type = OBJECT_SELECT_ORDER::jet_lepton;
    }
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

    if(data_type == 0)
        s = new select_tree(input, outdir+"/"+"new_"+outputFile, "mytree", "Jet_pt", "MET_pt", year, DATA_TYPE::data, op_type, order_type, cate, num_j, num_e, num_m);
    else
        s = new select_tree(input, outdir+"/"+"new_"+outputFile, "mytree", "Jet_pt_nom", "MET_T1Smear_pt", year, DATA_TYPE::MC_sys, op_type, order_type, cate, num_j, num_e, num_m);
    s->write();
    delete s;
}