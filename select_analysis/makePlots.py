import cmsstyle as cms
import ROOT as rt
import Acorn.Analysis.analysis as ana
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--lo', action='store_true', default=False)
parser.add_argument('--lin', action='store_true', default=False)
args = parser.parse_args()

def pad_divide(pad, ndiv, ymin, ymax, dxmin, dxmax, frac=1, size=1., final=False):
    pad.cd()
    hframe = cms.GetcmsCanvasHist(pad)
    LS = hframe.GetXaxis().GetLabelSize()
    TL = hframe.GetXaxis().GetTickLength()
    OF = hframe.GetXaxis().GetLabelOffset()
    hframe.GetXaxis().SetTickLength(0)
    xmax = hframe.GetXaxis().GetXmax()
    xmin = hframe.GetXaxis().GetXmin()
    r = int((xmax - xmin) / ndiv)
    axs = []
    lines = []
    hframe.GetXaxis().SetLabelSize(0)
    for i in range(ndiv):
        for ypos in [ymin, ymax]:
            loc = '-' if ypos == ymax else '+'
            ax = rt.TGaxis(xmin + i * r, ypos, xmin + i * r + r, ypos, dxmin, dxmax, 505, "BS" + loc, 0.0)
            ax.SetLabelFont(hframe.GetLabelFont())
            ax.SetLabelFont(hframe.GetLabelFont())
            if i < ndiv - 1 or not final:
                ax.ChangeLabelByValue(dxmax, -1, 0)
            # ax.ChangeLabelByValue(dxmin, -1, 0)
            ax.SetLabelOffset(OF)
            ax.SetTickLength(TL * ndiv)
            ls = 0 if ypos == ymax else LS
            ax.SetLabelSize(ls * size)
            if ndiv > 1:
                ax.SetNdivisions(503)
            axs.append(ax)
        if i < ndiv - 1:
            l = rt.TLine(xmin + i * r + r, ymin, xmin + i * r + r, ymax * frac)
            l.SetLineWidth(1)
            l.SetLineStyle(7)
            l.SetLineColor(2)
            lines.append(l)
    return axs, lines

latex_sub = rt.TLatex()
latex_sub.SetTextFont(62)
latex_sub.SetTextSize(0.03)
latex_sub.SetTextAlign(23)
latex = rt.TLatex()
latex.SetTextFont(62)
latex.SetTextSize(0.04)
latex.SetTextAlign(23)
latex_pt = rt.TLatex()
latex_pt.SetTextFont(62)
latex_pt.SetTextAlign(21)

selections = {'m3': '#mu, 3jets', 'm4': '#mu, #geq 4jets', 'e3': 'e, 3jets', 'e4': 'e, #geq 4jets'}
deltay = ['deltay_0', 'deltay_1', 'deltay_2', 'deltay_3']
pad_title = ["|#Deltay| < 0.4", "0.4 < |#Deltay| < 1.0", "1.0 < |#Deltay| < 2.0", "|#Deltay| > 2.0"]
ndivs = [2, 2, 2, 1]
pad_subtitle = [["|cos(#theta)| < 0.3", "|cos(#theta)| > 0.3"], ["|cos(#theta)| < 0.5", "|cos(#theta)| > 0.5"], ["|cos(#theta)| < 0.8", "|cos(#theta)| > 0.8"], ["0 < |cos(#theta)| < 1"]]

file = rt.TFile('./output_LO.root' if args.lo else './output_back.root')

hists = ana.TDirToNode(file)
file.Close()

def get_hist(node, path, name):
    for p in path.split('/'):
        node = node[p]
    hist = node[name].Clone()
    # hist.Add(node['SM'], -1)
    hist.Divide(node['SM'])
    return hist

dn = [0.31, 0.5]
up = [1.3, 1.5]
fracs = [0.88, 1]
cms.SetLumi(59.83)

def plot(sel, lin=True):
    canv, pads_u, pads_r, _ = cms.cmsJoinCanvas('canv_' + sel, 4, [0] * 4, [1000 * n for n in ndivs], dn[0], up[0], dn[1], up[1], 'p_{T}^{t} [GeV]', 'Corr.', 'Ratio',  square=False, logy=False, iPos=0, extraSpace=0.0, yTitOffset=0.65, purity=False, ratio=True, Rspace=0.01)
    t = canv.GetTopMargin()
    scale = 1
    axs, lines = [], []
    allhists = []
    lin_str = '_lin' if lin else '_quad'
    for j in range(len(deltay)):
        pads_u[j].cd()
        pads_u[j].SetGrid(0, 1)
        cms.GetcmsCanvasHist(pads_u[j]).Draw()
        dy = deltay[j]
        ctg1_rw = get_hist(hists, f'{sel}/{dy}', 'ctg1' + lin_str + '_rw')
        ctg2_rw = get_hist(hists, f'{sel}/{dy}', 'ctg2' + lin_str + '_rw')
        ctg1_me = get_hist(hists, f'{sel}/{dy}', 'ctg1' + lin_str + '_me')
        ctg2_me = get_hist(hists, f'{sel}/{dy}', 'ctg2' + lin_str + '_me')
        cms.cmsDraw(ctg1_me, 'P', lwidth=2, lcolor=2, msize=0, fstyle=0)
        # cms.cmsDraw(ctg2_me, 'P', lwidth=2, lcolor=4, msize=0, fstyle=0)
        cms.cmsDraw(ctg1_rw, 'P', lwidth=2, lstyle=7, lcolor=2, lalpha=0.5, msize=0, fstyle=0)
        # cms.cmsDraw(ctg2_rw, 'P', lwidth=2, lstyle=7, lcolor=4, lalpha=0.5, msize=0, fstyle=0)
        if j == len(deltay) - 1:
            leg = cms.cmsLeg(pads_u[j].GetLeftMargin() + 0.05, 0.7, 1 - pads_u[j].GetRightMargin() - 0.01, 0.8, textSize=0.03, columns=1)
            leg.AddEntry(ctg1_me, 'c_{tg} = 1, ME', 'l')
            leg.AddEntry(ctg1_rw, 'c_{tg} = 1, 3D rw', 'l')
            # leg.AddEntry(ctg2_me, 'c_{tg} = 2, ME', 'l')
            # leg.AddEntry(ctg2_rw, 'c_{tg} = 2, 3D rw', 'l')
            leg.Draw('same')
        pads_r[j].cd()
        pads_r[j].SetGrid(0, 1)
        cms.GetcmsCanvasHist(pads_r[j]).Draw()
        ctg1_r = ctg1_me.Clone()
        ctg1_r.Divide(ctg1_rw)
        # ctg2_r = ctg2_me.Clone()
        # ctg2_r.Divide(ctg2_rw)
        cms.cmsDraw(ctg1_r, 'P', lwidth=2, lcolor=2, msize=0, fstyle=0)
        # cms.cmsDraw(ctg2_r, 'P', lwidth=2, lcolor=4, msize=0, fstyle=0)
        allhists += [ctg1_rw, ctg2_rw, ctg1_me, ctg2_me, ctg1_r]
        pads = [pads_u[j], pads_r[j]]
        for i in range(2):
            pads[i].cd()
            ax, line = pad_divide(pads[i], ndivs[j], dn[i], up[i], 0, 1000, frac=fracs[i], size=1., final=(j==3))
            axs += ax
            lines += line
            for a in ax:
                a.Draw()
            for l in line:
                l.Draw()

            pad_width = 1. - pads[i].GetLeftMargin() - pads[i].GetRightMargin()
            if i == 0:
                pads[i].cd()
                latex.DrawLatexNDC(pads[i].GetLeftMargin() + pad_width * 0.5, 0.9, pad_title[j])
                pad_width /= ndivs[j]
                ytext = 0.87 * (1 - pads[i].GetBottomMargin() - pads[i].GetTopMargin()) + pads[i].GetBottomMargin() - 0.01
                for k in range(ndivs[j]):
                    latex_sub.DrawLatexNDC(pads[i].GetLeftMargin() + pad_width * (k + 0.5), ytext, pad_subtitle[j][k])

    pads_u[0].cd()
    latex_pt.DrawLatexNDC(0.52, 1 - t + cms.lumiTextOffset * t * scale + 0.008, selections[sel] + ', RECO, ' + ('Interference, ' if lin else 'BSM, ') + ('extra parton vetoed' if args.lo else 'with extra partons'))
    for i in range(4):
        cms.UpdatePad(pads_u[i])
        cms.UpdatePad(pads_r[i])
    canv.Print(f'./plot_browser/3D_rw/{sel}{lin_str}' + ('' if not args.lo else '_LO') + '.pdf')
for sel in selections:
    plot(sel, lin=args.lin)