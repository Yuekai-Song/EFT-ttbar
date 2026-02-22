
#if defined(__CLING__)
#pragma cling add_include_path("/cvmfs/cms.cern.ch/el9_amd64_gcc12/external/python3/3.9.14-b98669a1dd7ad6f9a9402f3cf1f9d11c/include/python3.9")
#pragma cling add_library_path("/cvmfs/cms.cern.ch/el9_amd64_gcc12/external/python3/3.9.14-b98669a1dd7ad6f9a9402f3cf1f9d11c/lib")
#pragma cling load("libAcornAnalysis.so")
#endif
#include "select_tree.cpp"
void process_test(TString outdir, TString outputFile, TString input, int year, bool ttx = false, bool jet_lep = false){
    OP_TYPE op_type;
    OBJECT_SELECT_ORDER order_type;
    if (ttx)
    {
        op_type = select_reco_ttx;
        outputFile += "_ttx";
    }
    else
        op_type = select_reco;
    if (jet_lep)
    {
        order_type = OBJECT_SELECT_ORDER::jet_lepton;
    }
    else
    {
        order_type = OBJECT_SELECT_ORDER::lepton_jet;
        outputFile += "_lj";
    }
    outputFile += ".root";
    select_tree *s;
    CATEGORY cate = CATEGORY::A;
    int num_j, num_e, num_m, num_g, num_lhe;
    read_object r(input, 1);
    num_j = r.nj;
    num_e = r.ne;
    num_m = r.nm;
    num_g = r.ng;
    num_lhe = r.nLHE;
    cout << num_lhe << endl;
    // num_j = 50;
    // num_e = 20;
    // num_m = 20;
    // num_g = 50;
    // num_lhe = 20;
    TFile* dis_file = TFile::Open(Form("./dis/dis_%d.root", year));
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
        tf_file = TFile::Open(Form("./corr/tf_ttx/tf_%d.root", year));
    else
        tf_file = TFile::Open(Form("./corr/tf/tf_%d.root", year));
    select_tree::h_ecorr = (TF1 *)tf_file->Get("Func");

    s = new select_tree(input, outdir+"/"+outputFile, "mytree", "Jet_pt", "MET_pt", year, DATA_TYPE::MC_sys, op_type, order_type, cate, num_j, num_e, num_m, num_g, num_lhe);
    s->write();
    delete s;

    dis_file->Close();
    tf_file->Close();
}