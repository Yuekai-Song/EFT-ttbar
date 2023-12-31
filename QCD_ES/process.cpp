#include "select_tree.cpp"
void process(TString outdir, TString outputFile, TString input, int year, int cate){
    int num_j, num_e, num_m, num_g = 0;
    bool type = false;
    if(outputFile.Contains("EGamma") || outputFile.Contains("SingleElectron") || outputFile.Contains("SingleMuon") || outputFile.Contains("DoubleEG"))
        type = true;
    read_object r(input, type);
    select_tree *s;
    num_j = r.nj;
    num_e = r.ne;
    num_m = r.nm;
    num_g = r.ng;
    /*num_j = 50;
    num_e = 20;
    num_m = 20;
    num_g = 50;*/
    s = new select_tree(input, outdir+"/"+"new_"+outputFile, year, cate, type, num_j, num_e, num_m, num_g);
    s->write();
    delete s;
}

