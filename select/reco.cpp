#include "reco.h"
#include "NuSolver/NuSolveLJ.cpp"
// #define DEBUG_RECO // Uncomment to unleash debugging
Double_t likelihood_lep(Double_t *pz, Double_t* pars){
    double mw = 82.9, sw = 9.5, mt = 172.5, st = 16.3;
    TLorentzVector nu(pars[8], pars[9], pz[0], sqrt(pars[8] * pars[8] + pars[9] * pars[9] + pz[0] * pz[0]));
    TLorentzVector jet(pars[0], pars[1], pars[2], pars[3]);
    TLorentzVector lep(pars[4], pars[5], pars[6], pars[7]);
    Double_t m_w = (nu + lep).M();
    Double_t m_t = (nu + lep + jet).M();
    //Double_t pro_w = (m_w - mw) * (m_w - mw) / (2 * sw * sw);
    //Double_t pro_t = (m_t - mt) * (m_t - mt) / (2 * st * st);
    //Double_t log_nupz = pro_t + pro_w;
    int bin;
    Double_t log_nupz;
    if (pars[10] > 0)
    {
        bin = RECO::mtl_vs_mwl_4->FindBin(m_t, m_w);
        log_nupz = -log(RECO::mtl_vs_mwl_4->GetBinContent(bin));
    }
    else
    {
        bin = RECO::mtl_vs_mwl_3->FindBin(m_t, m_w);
        log_nupz = -log(RECO::mtl_vs_mwl_3->GetBinContent(bin));
    }
    return log_nupz;
}

double RECO::nusolver1(int index_bl)
{
    double met_px = met_pt * cos(met_phi);
    double met_py = met_pt * sin(met_phi);
    TF1 *likelihood_fun = new TF1("likelihood_fun", likelihood_lep, -1000.0, 1000.0, 11);
    Double_t *pars = new Double_t[11];
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
    pars[10] = num_jets >= 4 ? 1 : -1;
    likelihood_fun->SetParameters(pars);
    double nupz = likelihood_fun->GetMinimumX(-1000.0, 1000.0);
    mom_nu.SetPxPyPzE(met_px, met_py, nupz, sqrt(met_px * met_px + met_py * met_py + nupz * nupz));
#ifdef DEBUG_RECO
    cout << "nusolver1: " << nupz << " " << likelihood_fun->GetMinimum(-1000.0, 1000.0) << endl;
#endif
    return likelihood_fun->GetMinimum(-1000.0, 1000.0);
}
double RECO::nusolver2(int index_bl)
{
    NuSolveLJ ns;
    ns.MakeSolution(mom_jets[index_bl], mom_lep, met_pt, met_phi, 1, 1, 0);
    mom_nu = ns.Nu();
    D_nu = sqrt(ns.NuChi2());
    double log_nupz;
    int bin;
    if(mom_nu.E() < 0)
        log_nupz = numeric_limits<double>::infinity();
    else
    {   
        if(num_jets >= 4)
        {
            bin = Dnu_4->FindBin(D_nu);
            log_nupz = -log(Dnu_4->GetBinContent(bin));
        }
        else
        {
            bin = Dnu_3->FindBin(D_nu);
            log_nupz = -log(Dnu_3->GetBinContent(bin));
        }
    }
#ifdef DEBUG_RECO
    cout << "nusolver2: " << mom_nu.Pz() << " " << D_nu << endl;
#endif
    return log_nupz;
}
double RECO::nusolver(int index_bl)
{
    if (ttx)
        return nusolver2(index_bl);
    else
        return nusolver1(index_bl);
}
double RECO::likelihood_had(int bh, int j1, int j2)
{
    Double_t m_w = (mom_jets[j1] + mom_jets[j2]).M();
    Double_t m_t = (mom_jets[j1] + mom_jets[j2] + mom_jets[bh]).M();
    int bin;
    double log_nupz;
#ifdef DEBUG_RECO
    cout << "In likelihood for 4 jets: hadronic mass: " << m_w << " " << m_t << endl;
#endif
    //double mwh = 82.9, swh = 9.5, mth = 172.5, sth = 16.3;
    //Double_t pro_whad = (m_w - mwh) * (m_w - mwh) / (2 * swh * swh);
    //Double_t pro_thad = (m_t - mth) * (m_t - mth) / (2 * sth * sth);
    if (!ttx)
    {
        bin = mth_vs_mwh_4->FindBin(m_t, m_w);
        log_nupz = -log(mth_vs_mwh_4->GetBinContent(bin));
    }
    else
    {
        bin = mth_vs_mwh_ttx_4->FindBin(m_t, m_w);
        log_nupz = -log(mth_vs_mwh_ttx_4->GetBinContent(bin));
    }
    return log_nupz;
}
double RECO::likelihood_had(int bh, int j1)
{
    Double_t m_t = (mom_jets[j1] + mom_jets[bh]).M();
    int bin;
    double log_nupz;
#ifdef DEBUG_RECO
    cout << "In likelihood for 3 jets: hadronic mass: " << m_t << endl;
#endif
    //double mth = 172.5, sth = 16.3;
    //Double_t pro_thad = (m_t - mth) * (m_t - mth) / (2 * sth * sth);
    if (!ttx)
    {
        bin = mth_3->FindBin(m_t);
        log_nupz = -log(mth_3->GetBinContent(bin));
    }
    else
    {
        bin = mth_ttx_3->FindBin(m_t);
        log_nupz = -log(mth_ttx_3->GetBinContent(bin));
    }
    return log_nupz;
}
double RECO::chi2(int bh, int j1, int j2)
{
    Double_t m_w = (mom_jets[j1] + mom_jets[j2]).M();
    Double_t m_t = (mom_jets[j1] + mom_jets[j2] + mom_jets[bh]).M();
    int bin;
    double log_nupz;
#ifdef DEBUG_RECO
    cout << "In chi2: mass: " << m_w << " " << m_t << endl;
#endif
    double mwh = 82.9, swh = 9.5, mth = 172.5, sth = 16.3;
    Double_t pro_whad = (m_w - mwh) * (m_w - mwh) / (2 * swh * swh);
    Double_t pro_thad = (m_t - mth) * (m_t - mth) / (2 * sth * sth);
    return pro_whad + pro_thad;
}
void RECO::btag_sort()
{
    set_index();
    for (int kk = 0; kk < 2; kk++)
    {
        int max = kk;
        for (int tt = kk + 1; tt < num_jets; tt++)
        {
            if (btag_score[index[tt]] > btag_score[index[max]])
                max = tt;
        }
        int tmp = index[max];
        index[max] = index[kk];
        index[kk] = tmp;
    }
#ifdef DEBUG_RECO
    cout << "btag_sort: " << endl; 
    for (int i = 0; i < num_jets; i++)
        cout << index[i] << " ";
    cout << endl;
#endif
}
void RECO::chi2_sort()
{
    btag_sort();
    int bjet_lep, bjet_had, ljet1, ljet2;
    chi = numeric_limits<double>::infinity();
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
    int bjet_lep = 0, bjet_had = 0, ljet1 = 0, ljet2 = 0;
    like = numeric_limits<double>::infinity();
    double minimum;
    double minimum_lep;
    TLorentzVector mom_nu_temp;
    double D_nu_temp;
    // for at least 4 jets
    if (num_jets >= 4)
    {
        for (int bl = 0; bl < 2; bl++)
        {
            minimum_lep = nusolver(index[bl]);
            for (int j1 = 2; j1 < num_jets; j1++)
            {
                for (int j2 = j1 + 1; j2 < num_jets; j2++)
                {
                    minimum = likelihood_had(index[1 - bl], index[j1], index[j2]) + minimum_lep;
#ifdef DEBUG_RECO
                    cout << index[bl] << ", " << index[1 - bl] << ", " << index[j1] << ", " << index[j2] << ", " << minimum << endl;
#endif
                    if (like > minimum)
                    {
                        like = minimum;
                        mom_nu_temp = mom_nu;
                        D_nu_temp = D_nu;
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
            minimum = likelihood_had(index[1 - bl], index[2]) + nusolver(index[bl]);
#ifdef DEBUG_RECO
            cout << index[bl] << ", " << index[1 - bl] << ", " << index[2]  << ", " << minimum << endl;
#endif
            if (like > minimum)
            {
                like = minimum;
                mom_nu_temp = mom_nu;
                D_nu_temp = D_nu;
                bjet_lep = bl;
                bjet_had = 1 - bl;
            }
        }
        reco_index[0] = index[bjet_lep];
        reco_index[1] = index[bjet_had];
        reco_index[2] = index[2];
    }
    mom_nu = mom_nu_temp;
    D_nu = D_nu_temp;
#ifdef DEBUG_RECO
    cout << "like_sort:" << endl;
    for (int i = 0; i < min(num_jets, 4); i++)
        cout << reco_index[i] << " ";
    cout << endl;
#endif
}
bool RECO::reco_top()
{
    if (LHE)
        category = gen_sort(); //0: non-reco; 1: correct reco; 2: wrong reco
    if (gen_reco)
    {
        if (category)
        {
            for (int i = 0; i < min(num_jets, 4); i++)
                reco_index[i] = gen_index[i];
            if (nusolver(gen_index[0]) > numeric_limits<double>::max())
                return false;
        }
        else
            return false;
    }
    else
    {
        like_sort();
        if (like > numeric_limits<double>::max()) //like = +inf, every configuration is wrong
            return false;
        if (category)
        {
            if (!(gen_index[0] == reco_index[0] && gen_index[1] == reco_index[1]))
                category = 2;
            if (num_jets >= 4)
            {
                if (!((gen_index[2] == reco_index[2] && gen_index[3] == reco_index[3]) || (gen_index[2] == reco_index[3] && gen_index[3] == reco_index[2])))
                    category = 2;
            }
            else
            {
                if (!(gen_index[2] == reco_index[2]))
                    category = 2;
            }
        }
    }

#ifdef DEBUG_RECO
    cout << "reco_sort:" << endl;
    for (int i = 0; i < min(num_jets, 4); i++)
        cout << reco_index[i] << " ";
    cout << endl;
#endif
    
    mom_tl = mom_nu + mom_lep + mom_jets[reco_index[0]];
    mom_wl = mom_nu + mom_lep;
    if (num_jets >= 4)
    {
        mom_th = mom_jets[reco_index[2]] + mom_jets[reco_index[3]] + mom_jets[reco_index[1]];
        mom_wh = mom_jets[reco_index[2]] + mom_jets[reco_index[3]];
    }
    else
    {
        mom_th = mom_jets[reco_index[2]] + mom_jets[reco_index[1]];
        mom_wh = mom_jets[reco_index[2]];
    }
    return true;
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
bool RECO::reco()
{
    //reco_chi();
    return reco_top();
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
#ifdef DEBUG_RECO
    cout << "gen_sort:" << endl;
    for (int i = 0; i < 4; i++)
        cout << gen_index[i] << " ";
    cout << endl;
#endif
    //non-reco or multi-reco
    int ngen_jets = 4;
    if (num_jets == 3 && (gen_index[2] == -1 || gen_index[3] == -1))
    {
        ngen_jets = 3;
        if (gen_index[2] == -1)
        {
            gen_index[2] = gen_index[3];
            gen_index[3] = -1;
        }
    }
    for (int i = 0; i < ngen_jets; i++)
    {
        if (gen_index[i] == -1)
            return false;
        for (int j = i + 1; j < ngen_jets; j++)
            if (gen_index[i] == gen_index[j])
                return false;
    }

    //btag wrong
    if (!((gen_index[0] == index[0] && gen_index[1] == index[1]) || (gen_index[0] == index[1] && gen_index[1] == index[0])))
        return false;
    
    return true;
}