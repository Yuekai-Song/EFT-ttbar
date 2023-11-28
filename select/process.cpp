#include "select_tree.cpp"
void process(TString outdir, TString outputFile, TString input, int year, int data_type, int reco_type, bool reco_ttx){
    if (!(data_type == 1 && outputFile.Contains("TTToSemi")) && reco_type != 0)
    {
        cout << "using LHE information when dataset is not MC" << endl;
        return;
    }
    TString tree_name[] = {"mytree", "mytree", "jerUp", "jerDown", "unclusUp", "unclusDown"};
    TString jet_name[] = {"Jet_pt", "Jet_pt_nom", "Jet_pt_jerUp", "Jet_pt_jerDown", "Jet_pt_nom", "Jet_pt_nom"};
    TString MET_name[] = {"MET_pt", "MET_T1Smear_pt", "MET_T1Smear_pt_jerUp", "MET_T1Smear_pt_jerDown", "MET_T1Smear_pt_unclustEnUp", "MET_T1Smear_pt_unclustEnDown"};
    
    TString jes_source[] = {"Absolute", Form("Absolute_%d", year), "FlavorQCD", "BBEC1", "EC2", "HF", Form("BBEC1_%d", year), Form("EC2_%d", year), "RelativeBal", Form("RelativeSample_%d", year)};
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
    if(data_type == 0){
        s = new select_tree(input, outdir+"/"+"new_"+outputFile, tree_name[0], jet_name[0], MET_name[0], year, 0, 0, reco_ttx, num_j, num_e, num_m);
        s->write();
        delete s;
    }
    else if(data_type == 1){
        s = new select_tree(input, outdir+"/"+"new_"+outputFile, tree_name[1], jet_name[1], MET_name[1], year, 1, reco_type, reco_ttx, num_j, num_e, num_m, num_g);
        s->write();
        delete s;
        //uncertainties for JER, MET_uncluster
        for(int sys=2; sys<6; sys++){
            s1 = new select_tree(input, outdir+"/"+"new_"+outputFile, tree_name[sys], jet_name[sys], MET_name[sys], year, 2,  0, reco_ttx, num_j, num_e, num_m);
            s1->write();
            delete s1;
        }
        //uncertainties for different JES sources
        for(int sour=0; sour<10; sour++){
            //cout<<"jes_"+jes_source[sour]+"Up"<<endl;
            s1 = new select_tree(input, outdir+"/"+"new_"+outputFile, "jes_"+jes_source[sour]+"Up", "Jet_pt_jes"+pt_jes_source[sour]+"Up", "MET_T1Smear_pt_jes"+pt_jes_source[sour]+"Up", year, 2, 0, reco_ttx, num_j, num_e, num_m);
            s1->write();
            delete s1;
            s2 = new select_tree(input, outdir+"/"+"new_"+outputFile, "jes_"+jes_source[sour]+"Down", "Jet_pt_jes"+pt_jes_source[sour]+"Down", "MET_T1Smear_pt_jes"+pt_jes_source[sour]+"Down", year, 2, 0, reco_ttx, num_j, num_e, num_m);
            s2->write();
            delete s2;
        }
    }
    else if(data_type == 2){
        s = new select_tree(input, outdir+"/"+"new_"+outputFile, tree_name[1], jet_name[1], MET_name[1], year, 3, 0, reco_ttx, num_j, num_e, num_m);
        s->write();
        delete s;
    }
}