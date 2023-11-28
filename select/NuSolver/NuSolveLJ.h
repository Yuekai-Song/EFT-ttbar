#include <math.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Eigen/Dense"
#include<TLorentzVector.h>
using namespace std;

struct Soln2d
{
    Eigen::Vector3d vec;
    double k;
};

class NuSolveLJ
{
private:
    const double Mt_ = 172.5;
    const double Mw_ = 80.4;
    double Ml_ = 0.;
    double Mb_ = 0.;
    const double Mn_ = 0.;

    bool ERROR_ = false;
    Eigen::Matrix3d H_;
    Eigen::Vector2d MET_;
    Eigen::Matrix2d VM_;
    Eigen::Matrix3d V0_;
    Eigen::Matrix3d SIGm2_;

    TLorentzVector nu_;

    bool solved_ = false;

    const Eigen::Matrix3d Unit_;
    static Eigen::Matrix3d make_unit()
    {
        Eigen::Matrix3d ret = Eigen::Matrix3d::Identity();
        ret(2, 2) = -1;
        return ret;
    }
    const Eigen::Matrix3d Deriv_;
    static Eigen::Matrix3d make_deriv()
    {
        Eigen::Matrix3d ret;
        ret << 0., -1., 0.,
            1., 0., 0.,
            0., 0., 0.;
        return ret;
    }

    inline double Cofactor(const Eigen::Matrix3d &M, int row, int col);
    Eigen::Matrix3d MatCross(const Eigen::Vector3d &L, const Eigen::Matrix3d &M);
    inline int swapI(int i);
    Eigen::Matrix3d SwapMat(const Eigen::Matrix3d &G);
    Eigen::Vector3d SwapLine(const Eigen::Vector3d &L);
    std::vector<Eigen::Vector3d> Lfactor(Eigen::Matrix3d M);
    std::vector<Soln2d> IntersectLineEllipse(const Eigen::Vector3d &line, const Eigen::Matrix3d &ellipse);
    std::vector<Soln2d> IntersectEllipses(const Eigen::Matrix3d &ellipse1, const Eigen::Matrix3d &ellipse2);

    double nu_chisq_ = numeric_limits<double>::max();

    void Solve();
    void Reset()
    {
        ERROR_ = false;
        solved_ = false;
        Ml_ = 0.;
        Mb_ = 0.;

        H_ = Eigen::Matrix3d::Zero();
        MET_ = Eigen::Vector2d::Zero();
        VM_ = Eigen::Matrix2d::Zero();
        V0_ = Eigen::Matrix3d::Zero();
        SIGm2_ = Eigen::Matrix3d::Zero();

        nu_chisq_ = numeric_limits<double>::max();
    }

    bool Error()
    {
        return ERROR_;
    }

    double Chi2(const Eigen::Matrix3d &X, const Eigen::Vector3d &soln)
    {
        return (soln.transpose() * X * soln)(0, 0);
    }

    void SetMET(double metx, double mety, double metxerr, double metyerr, double metxyrho);
    void BuildH(const TLorentzVector &lep, const TLorentzVector &bjet);
    int eps_[3][3][3];

public:
    double NuChi2()
    {
        if (!solved_)
            std::cout << "solved_ is false! Check if Solve() is being called!" << std::endl;
        return nu_chisq_;
    }

    TLorentzVector Nu()
    {
        if (!solved_)
            std::cout << "solved_ is false! Check if Solve() is being called!" << std::endl;
        return nu_;
    }

    void MakeSolution(const TLorentzVector &bj, const TLorentzVector &lep, double met_pt, double met_phi, double metunc_x, double metunc_y, double metxy_rho);

    NuSolveLJ(); // configuration
    ~NuSolveLJ();
};