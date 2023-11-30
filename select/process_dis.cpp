#include "select_tree.cpp"
void process_dis(TString outdir, TString outputFile, TString input, int year){
    TString tree_name = "mytree";
    TString jet_name = "Jet_pt_nom";
    TString MET_name = "MET_T1Smear_pt";
    
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

    s = new select_tree(input, outdir+"/"+"new_"+outputFile, tree_name, jet_name, MET_name, year, MC, dis_reco_need, num_j, num_e, num_m, num_g);
    s->write();
    delete s;

}