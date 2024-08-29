#include "SF_add_tree.cpp"
void SF_add(TString inputFile, bool is_sys, int year, TString cg)
{
    TString trees[] = {"jerUp", "jerDown", "unclusUp", "unclusDown"};
    TString jes_source[] = {"Absolute", Form("Absolute_%d", year), "FlavorQCD", "BBEC1", "EC2", "HF", Form("BBEC1_%d", year), Form("EC2_%d", year), "RelativeBal", Form("RelativeSample_%d", year)};

    SF_add_tree *s, *s1, *s2;
    if (!is_sys)
    {
        s = new SF_add_tree(inputFile, "mytree", true, year, cg);
        delete s;
        for (int i = 0; i < 4; i++)
        {
            s1 = new SF_add_tree(inputFile, trees[i], false, year, cg);
            delete s1;
        }
        for (int i = 0; i < 10; i++)
        {
            s1 = new SF_add_tree(inputFile, "jes_" + jes_source[i] + "Up", false, year, cg);
            delete s1;
            s2 = new SF_add_tree(inputFile, "jes_" + jes_source[i] + "Down", false, year, cg);
            delete s2;
        }
    }
    else
    {
        s = new SF_add_tree(inputFile, "mytree", false, year, cg);
        delete s;
    }
}