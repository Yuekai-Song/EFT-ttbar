#include "reco.h"
#include "NuSolver/NuSolveLJ.cpp"

Double_t like_lep(Double_t *pz, Double_t* pars){
    TLorentzVector nu(pars[8], pars[9], pz[0], sqrt(pars[8] * pars[8] + pars[9] * pars[9] + pz[0] * pz[0]));
    TLorentzVector jet(pars[0], pars[1], pars[2], pars[3]);
    TLorentzVector lep(pars[4], pars[5], pars[6], pars[7]);
    Double_t m_w = (nu + lep).M();
    Double_t m_t = (nu + lep + jet).M();
    Double_t pro_w = ROOT::Math::gaussian_pdf(m_w, 9.5, 82.9);
    Double_t pro_t = ROOT::Math::gaussian_pdf(m_t, 16.3, 172.5);
    Double_t log_nupz = -TMath::Log(pro_t) - TMath::Log(pro_w);
    return log_nupz;
}

double RECO::nusolver1(int index_bl)
{
    double met_px = met_pt * cos(met_phi);
    double met_py = met_pt * sin(met_phi);
    TF1 *likelihood_fun = new TF1("likelihood_fun", like_lep, -1000.0, 1000.0, 10);
    Double_t *pars = new Double_t[10];
    pars[0] = mom_jets[index_bl].Px();
    pars[1] = mom_jets[index_bl].Py();
    pars[2] = mom_jets[index_bl].Pz();
    pars[3] = mom_jets[index_bl].E();
    pars[4] = mom_lep.Px();
    pars[5] = mom_lep.Py();
    pars[6] = mom_lep.Pz();
    pars[7] = mom_lep.E();
    pars[8] = met_px;
    pars[9] = met_py;
    likelihood_fun->SetParameters(pars);
    double nupz = likelihood_fun->GetMinimumX(-1000.0, 1000.0);
    mom_nu.SetPxPyPzE(met_px, met_py, nupz, sqrt(met_px * met_px + met_py * met_py + nupz * nupz));
    
    return likelihood_fun->GetMinimum(-1000.0, 1000.0);
}
bool RECO::nusolver2(int index_bl)
{
    NuSolveLJ ns;
    ns.MakeSolution(mom_jets[index_bl], mom_lep, met_pt, met_phi, 1, 1, 0);
    mom_nu = ns.Nu();
    D_nu = sqrt(ns.NuChi2());
    if(mom_nu.E() < 0)
        return false;
    return true;
}
void RECO::nusolver(int index_bl)
{
    if (ttx)
        nusolver2(index_bl);
    else
        nusolver1(index_bl);
}
double RECO::likelihood(int bl, int bh, int j1, int j2)
{
    Double_t mass_whad = (mom_jets[j1] + mom_jets[j2]).M();
    Double_t mass_thad = (mom_jets[j1] + mom_jets[j2] + mom_jets[bh]).M();
    Double_t pro_whad = ROOT::Math::gaussian_pdf(mass_whad, 9.5, 82.9);
    Double_t pro_thad = ROOT::Math::gaussian_pdf(mass_thad, 16.3, 172.5);
    double log_nupz;
    if (!ttx)
        log_nupz = -TMath::Log(pro_thad) - TMath::Log(pro_whad) + nusolver1(bl);
    else
    {
        if(nusolver2(bl))
            log_nupz = -TMath::Log(pro_thad) - TMath::Log(pro_whad) + ROOT::Math::gaussian_pdf(D_nu, 10, 50);
        else
            log_nupz = numeric_limits<double>::max();
    }
    return log_nupz;
}
double RECO::likelihood(int bl, int bh, int j1)
{
    Double_t mass_thad = (mom_jets[j1] + mom_jets[bh]).M();
    Double_t pro_thad = ROOT::Math::gaussian_pdf(mass_thad, 16.3, 172.5);
    Double_t log_nupz;
    if (!ttx)
        log_nupz = -TMath::Log(pro_thad) + nusolver1(bl);
    else
    {
        if (nusolver2(bl))
            log_nupz = -TMath::Log(pro_thad) + ROOT::Math::gaussian_pdf(D_nu, 10, 50);
        else
            log_nupz = numeric_limits<double>::max();
    }
    return log_nupz;
}
double RECO::chi2(int bh, int j1, int j2)
{
    double m_bjj = (mom_jets[index[bh]] + mom_jets[index[j1]] + mom_jets[index[j2]]).M();
    double m_jj = (mom_jets[index[j1]] + mom_jets[index[j2]]).M();
    double chi2_v = ((m_bjj - 172.5) / 16.3) * ((m_bjj - 172.5) / 16.3) + ((m_jj - 82.9) / 9.5) * ((m_jj - 82.9) / 9.5);
    return chi2_v;
}
void RECO::btag_sort()
{
    set_index();
    for (int kk = 0; kk < 2; kk++)
    {
        int max = kk;
        for (int tt = kk + 1; tt < num_jets; tt++)
        {
            if (btag_score[reco_index[tt]] > btag_score[reco_index[max]])
                max = tt;
        }
        int tmp = reco_index[max];
        index[max] = index[kk];
        index[kk] = tmp;
    }
}
void RECO::chi2_sort()
{
    btag_sort();
    int bjet_lep, bjet_had, ljet1, ljet2;
    chi = numeric_limits<double>::max();
    double chi2_v;
    if (num_jets < 4)
    {
        chi = -1;
        return;
    }
    for (int bh = 0; bh < 2; bh++)
    {
        for (int j1 = 2; j1 < num_jets; j1++)
        {
            for (int j2 = j1 + 1; j2 < num_jets; j2++)
            {
                chi2_v = chi2(bh, j1, j2);
                if (chi > chi2_v)
                {
                    chi = chi2_v;
                    bjet_lep = 1 - bh;
                    bjet_had = bh;
                    ljet1 = j1;
                    ljet2 = j2;
                }
            }
        }
    }
    chi2_index[0] = index[bjet_lep];
    chi2_index[1] = index[bjet_had];
    chi2_index[2] = index[ljet1];
    chi2_index[3] = index[ljet2];
    
}
void RECO::like_sort()
{
    btag_sort();
    int bjet_lep, bjet_had, ljet1, ljet2;
    like = numeric_limits<double>::max();
    double minimum;
    // for at least 4 jets
    if (num_jets >= 4)
    {
        for (int bl = 0; bl < 2; bl++)
        {
            for (int j1 = 2; j1 < num_jets; j1++)
            {
                for (int j2 = j1 + 1; j2 < num_jets; j2++)
                {
                    minimum = likelihood(bl, 1 - bl, j1, j2);
                    if (like > minimum)
                    {
                        like = minimum;
                        bjet_lep = bl;
                        bjet_had = 1 - bl;
                        ljet1 = j1;
                        ljet2 = j2;
                    }
                }
            }
        }
        reco_index[0] = index[bjet_lep];
        reco_index[1] = index[bjet_had];
        reco_index[2] = index[ljet1];
        reco_index[3] = index[ljet2];
    }
    // for exactly 3 jets
    else if (num_jets == 3)
    {
        for (int bl = 0; bl < 2; bl++)
        {
            minimum = likelihood(bl, 1 - bl, 2);
            if (like > minimum)
            {
                like = minimum;
                bjet_lep = bl;
                bjet_had = 1 - bl;
            }
        }
        reco_index[0] = index[bjet_lep];
        reco_index[1] = index[bjet_had];
        reco_index[2] = index[2];
    }
}
void RECO::reco_top()
{
    if (gen_reco)
        category = !gen_sort(); //0: non-reco; 1: correct reco; 2: wrong reco
    if (gen_reco == 2)
    {
        if (!category)
        {
            for (int i = 0; i < 4; i++)
                reco_index[i] = gen_index[i];
        }
        else
            return;
    }
    else
        like_sort();

    if (gen_reco == 1)
    {
        if (!(gen_index[0] == index[0] && gen_index[1] == index[1]))
            category = 2;
        if (!((gen_index[2] == index[2] && gen_index[3] == index[3]) || (gen_index[2] == index[3] && gen_index[3] == index[2])))
            category = 2;
    }

    nusolver(reco_index[0]);
    TLorentzVector mom_top, mom_antitop;
    if (num_jets >= 4)
    {
        mass_tlep = (mom_nu + mom_lep + mom_jets[reco_index[0]]).M();
        mass_wlep = (mom_nu + mom_lep).M();
        mass_thad = (mom_jets[reco_index[2]] + mom_jets[reco_index[3]] + mom_jets[reco_index[1]]).M();
        mass_whad = (mom_jets[reco_index[2]] + mom_jets[reco_index[3]]).M();
        if (lep_charge > 0)
        {
            mom_top = mom_nu + mom_lep + mom_jets[reco_index[0]];
            mom_antitop = mom_jets[reco_index[2]] + mom_jets[reco_index[3]] + mom_jets[reco_index[1]];
        }
        else
        {
            mom_top = mom_jets[reco_index[2]] + mom_jets[reco_index[3]] + mom_jets[reco_index[1]];
            mom_antitop = mom_nu + mom_lep + mom_jets[reco_index[0]];
        }
        // cout<<mom_jets[reco_index[0]].Pt()<<" "<<mom_jets[reco_index[1]].Pt()<<" "<<mom_jets[reco_index[2]].Pt()<<" "<<mom_jets[reco_index[3]].Pt()<<endl;
    }
    else
    {
        mass_tlep = (mom_nu + mom_lep + mom_jets[reco_index[0]]).M();
        mass_wlep = (mom_nu + mom_lep).M();
        mass_thad = (mom_jets[reco_index[2]] + mom_jets[reco_index[1]]).M();
        mass_whad = (mom_jets[reco_index[2]]).M();
        if (lep_charge > 0)
        {
            mom_top = mom_nu + mom_lep + mom_jets[reco_index[0]];
            mom_antitop = mom_jets[reco_index[2]] + mom_jets[reco_index[1]];
        }
        else
        {
            mom_top = mom_jets[reco_index[2]] + mom_jets[reco_index[1]];
            mom_antitop = mom_nu + mom_lep + mom_jets[reco_index[0]];
        }
    }
    rectop_pt = mom_top.Pt();
    recantitop_pt = mom_antitop.Pt();
    rapidity_tt = mom_top.Rapidity() - mom_antitop.Rapidity();
    mass_tt = (mom_antitop + mom_top).M();
    mass_t = mom_top.M();
    mass_at = mom_antitop.M();
    // cout<<lep_charge<<" "<<mom_top.Pt()<<" "<<mom_antitop.Pt()<<endl;
}
void RECO::reco_chi()
{
    chi2_sort();
    if (num_jets >= 4)
    {
        mass_bjj = (mom_jets[chi2_index[1]] + mom_jets[chi2_index[2]] + mom_jets[chi2_index[3]]).M();
        mass_jj = (mom_jets[chi2_index[2]] + mom_jets[chi2_index[3]]).M();
        mass_lb = (mom_jets[chi2_index[0]] + mom_lep).M();
    }
    else
    {
        mass_bjj = -1;
        mass_jj = -1;
        mass_lb = -1;
    }
}
bool RECO::diff()
{
    if (num_jets == 3)
        return true;
    else
    {
        for (int i = 0; i < 4; i++)
        {
            if (reco_index[i] != chi2_index[i])
                return false;
        }
        return true;
    }
}
void RECO::reco()
{
    reco_chi();
    reco_top();
}

int RECO::select_LHE(TLorentzVector lhe_part)
{
    for (int i = 0; i < num_jets; i++)
    {
        if (lhe_part.DeltaR(mom_jets[i]) < 0.4)
            return i;
    }
    return -1;
}

bool RECO::gen_sort(){
    btag_sort();
    gen_index[0] = select_LHE(LHE_bl);
    gen_index[1] = select_LHE(LHE_bh);
    gen_index[2] = select_LHE(LHE_j1);
    gen_index[3] = select_LHE(LHE_j2);

    //non-reco or multi-reco
    for (int i = 0; i < 4; i++)
    {
        if (gen_index[i] == -1)
            return false;
        for (int j = i + 1; j < 4; j++)
            if (gen_index[i] == gen_index[j])
                return false;
    }

    //btag wrong
    if (!((gen_index[0] == index[0] && gen_index[1] == index[1]) || (gen_index[0] == index[1] && gen_index[1] == index[0])))
        return false;
    
    return true;
}