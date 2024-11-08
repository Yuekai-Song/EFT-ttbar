#include "select_tree.cpp"
void process(TString outdir, TString outputFile, TString input, int year, int data_type, bool ttx)
{
    OP_TYPE op_type;
    OBJECT_SELECT_ORDER order_type;
    if (ttx)
        op_type = select_reco_ttx;
    else
        op_type = select_reco;
    order_type = OBJECT_SELECT_ORDER::lepton_jet;
    CATEGORY cate = CATEGORY::A;
    outputFile.ReplaceAll(".root", "_A.root");
    TString tree_name[] = {"mytree", "mytree", "jerUp", "jerDown", "unclusUp", "unclusDown"};
    TString jet_name[] = {"Jet_pt", "Jet_pt_nom", "Jet_pt_jerUp", "Jet_pt_jerDown", "Jet_pt_nom", "Jet_pt_nom"};
    TString MET_name[] = {"MET_pt", "MET_T1Smear_pt", "MET_T1Smear_pt_jerUp", "MET_T1Smear_pt_jerDown", 
                          "MET_T1Smear_pt_unclustEnUp", "MET_T1Smear_pt_unclustEnDown"};
    
    TString jes_source[] = {"Absolute", Form("Absolute_%d", year), "FlavorQCD", "BBEC1", "EC2", "HF",
                            Form("BBEC1_%d", year), Form("EC2_%d", year), "RelativeBal", Form("RelativeSample_%d", year)};
    TString pt_jes_source[10];
    for(int i=0; i<10; i++){
        pt_jes_source[i] = jes_source[i];
        pt_jes_source[i] = pt_jes_source[i].ReplaceAll("2015", "2016");
    }
    int num_j, num_e, num_m, num_g;
    read_object r(input, data_type);
    select_tree *s, *s1, *s2;
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
    TFile* tf_file;
    if (ttx)
        tf_file = TFile::Open(Form("./corr/tf_ttx/tf_%d.root", year));
    else
        tf_file = TFile::Open(Form("./corr/tf/tf_%d.root", year));
    select_tree::h_ecorr = (TF1 *)tf_file->Get("Func");

    TFile* map_file;
    if (year == 2015 || year == 2016)
    {
        map_file = TFile::Open("./jet_veto_maps/hotjets-UL16.root");
        select_tree::jet_veto_map = (TH2D *)map_file->Get("h2hot_ul16_plus_hbm2_hbp12_qie11");
        TH2D *hot_mc = (TH2D *)map_file->Get("h2hot_mc");
        select_tree::jet_veto_map->Add(hot_mc);
    }
    else if (year == 2017)
    {
        map_file = TFile::Open("./jet_veto_maps/hotjets-UL17_v2.root");
        select_tree::jet_veto_map = (TH2D *)map_file->Get("h2hot_ul17_plus_hep17_plus_hbpw89");
    }
    else
    {
        map_file = TFile::Open("./jet_veto_maps/hotjets-UL18.root");
        select_tree::jet_veto_map = (TH2D *)map_file->Get("h2hot_ul18_plus_hem1516_and_hbp2m1");
    }

    if(data_type == 0){
        s = new select_tree(input, outdir+"/"+"new_"+outputFile, tree_name[0], jet_name[0], MET_name[0], year,
                            DATA_TYPE::data, op_type, order_type, cate, num_j, num_e, num_m);
        s->write();
        delete s;
    }
    else if(data_type == 1){
        s = new select_tree(input, outdir+"/"+"new_"+outputFile, tree_name[1], jet_name[1], MET_name[1], year,
                            DATA_TYPE::MC, op_type, order_type, cate, num_j, num_e, num_m, num_g);
        s->write();
        delete s;
        //uncertainties for JER, MET_uncluster
        for(int sys=2; sys<6; sys++){
            s1 = new select_tree(input, outdir+"/"+"new_"+outputFile, tree_name[sys], jet_name[sys], MET_name[sys],
                                 year, DATA_TYPE::tree_sys, op_type, order_type, cate, num_j, num_e, num_m, num_g);
            s1->write();
            delete s1;
        }
        //uncertainties for different JES sources
        for(int sour=0; sour<10; sour++){
            //cout<<"jes_"+jes_source[sour]+"Up"<<endl;
            s1 = new select_tree(input, outdir+"/"+"new_"+outputFile, "jes_"+jes_source[sour]+"Up",
                                 "Jet_pt_jes"+pt_jes_source[sour]+"Up", "MET_T1Smear_pt_jes"+pt_jes_source[sour]+"Up",
                                 year, DATA_TYPE::tree_sys, op_type, order_type, cate, num_j, num_e, num_m, num_g);
            s1->write();
            delete s1;
            s2 = new select_tree(input, outdir+"/"+"new_"+outputFile, "jes_"+jes_source[sour]+"Down",
                                 "Jet_pt_jes"+pt_jes_source[sour]+"Down", "MET_T1Smear_pt_jes"+pt_jes_source[sour]+"Down",
                                 year, DATA_TYPE::tree_sys, op_type, order_type, cate, num_j, num_e, num_m, num_g);
            s2->write();
            delete s2;
        }
    }
    else if(data_type == 2){
        s = new select_tree(input, outdir+"/"+"new_"+outputFile, tree_name[1], jet_name[1], MET_name[1], year,
                            DATA_TYPE::MC_sys, op_type, order_type, cate, num_j, num_e, num_m, num_g);
        s->write();
        delete s;
    }
    dis_file->Close();
    tf_file->Close();
    map_file->Close();
}