import os
# To list the files in a specific folders

def sys_and_nom(h1_sys_name: str) -> tuple:
    h1_sys_name_new = h1_sys_name.replace(".pdf", "")
    #names of sys with 201* and M/E need to be same for all years and all channels
    years = ["2015", "2016", "2017", "2018"]
    cates = ["E3j", "M3j", "E4j", "M4j"]
    for year in years:
        for cate in cates:
            h1_sys_name_new = h1_sys_name_new.replace("_" + cate + "_" + year, "-ch")
        h1_sys_name_new = h1_sys_name_new.replace("_" + year, "unco")
        h1_sys_name_new = h1_sys_name_new.replace(year, "unco")

    leps = ["Muon", "Elec"]
    for lep in leps:
        h1_sys_name_new = h1_sys_name_new.replace(lep, "lep")
    
    #pdf_w, jes_, SF_
    shift = 1
    odds = ["pdf_w", "jes", "SF", "nnlo_wt", "EW", "btag"]
    for odd in odds:
        if odd in h1_sys_name_new:
            shift += 1
    if "btag_jes" in h1_sys_name_new:
        shift -= 1
    pos = 0
    i = 1
    # print(h1_sys_name_new, " ", shift)
    while h1_sys_name_new[pos] != '_' or i < shift:
        if h1_sys_name_new[pos] == '_':
            i += 1
        pos += 1

    sys = h1_sys_name_new[:pos]
    nom = h1_sys_name_new[pos + 1: ]
    sys = sys.replace("_", "-")
    nom = nom.replace("_ci0000", "")
    return sys, nom

def get_channel_name(path_name: str) -> str:
    pos = len(path_name) - 1
    while path_name[pos] != '/':
        pos = pos - 1
    return path_name[pos + 1 :]

def sys_type(sys: str) -> str:
    if "pdf-w" not in sys:
        return "sys"
    elif int(sys.replace("pdf-w", "")) < 50:
        return "pdf_l50"
    else:
        return "pdf_g50"

def list_files(dir: str) -> tuple:
    pdfdict = dict()
    sys_nom = {"sys": dict(), "pdf_l50": dict(), "pdf_g50": dict()}
    for root, dirs, files in os.walk(dir):
        if dirs == []:
            for file in files:
                if file.endswith('.pdf'):
                    sys, nom = sys_and_nom(file)
                    if sys not in sys_nom[sys_type(sys)].keys():
                        sys_nom[sys_type(sys)][sys] = set()
                    sys_nom[sys_type(sys)][sys].add(nom) 
                    channel = get_channel_name(root)
                    pdfdict[sys + "_" + channel + "_" + nom] = os.path.join(root, file)
    return pdfdict, sys_nom

# To generate latex files

def gentex(filename: str, filedict: dict, sys: str,
              processes: list) -> None:
    channels = ["E_3jets", "M_3jets", "E_4jets", "M_4jets"]
    years = ["2015", "2016", "2017", "2018"]

    with open(filename, 'a', encoding='utf-8') as f:
        f.write('\n')
        f.write('\\subsection{%s}\n' % sys)

        for process in processes:
            #f.write('\\subsection{%s}\n' % process)
            for t1 in range(0, 2):
                f.write('\n')
                f.write('\\begin{frame}\n')

                f.write('\\frametitle{%s}\n' % (process))
                f.write('\\fontsize{5}{1}\selectfont\n')
                for t2 in range(0, 2):
                    i = 2 * t1 + t2
                    f.write(years[i] + ":\n")
                    f.write('\\begin{figure}\n')
                    f.write('\\centering\n')

                    for j in range(4):
                        f.write('\\begin{subfigure}[b]{0.24\\textwidth}\n')
                        f.write('\\includegraphics[width=\\textwidth]{%s}\n' %
                                filedict[sys + "_" + channels[j] + "_" +
                                         years[i] + "_" + process])
                        if t2 == 1:
                            f.write('\\captionsetup{font=tiny}\n')
                            f.write('\\caption{%s}\n' %
                                    channels[j].replace("_", ", "))
                        f.write('\\end{subfigure}\n')
                    f.write('\\end{figure}\n')
                f.write('\\end{frame}\n')
                f.write('\n')
        #f.write('\\end{document}\n')'''
    f.close()

def position(process: str) -> int:
    processes = ["ttbar", "DYJets", "STop", "WJets"]
    if process not in processes:
        return len(processes)
    else:
        return processes.index(process)

def sort_dict_by_key(input_dict):
    sorted_items = sorted(input_dict.items())
    sorted_dict = {k: v for k, v in sorted_items}
    return sorted_dict

def sys_add(dir: str, filename: str) -> None:
    pdfdict, sys_noms = list_files(dir)
    title = {"sys": "\\section{Distributions of the systematics}\n",
             "pdf_l50": "\\section{Distributions of the pdfs before 50}\n",
             "pdf_g50": "\\section{Distributions of the pdfs after 50}\n"}

    for sys_t, sys_nom in sys_noms.items():
        sys_nom_sorted = sort_dict_by_key(sys_nom)

        with open(filename, 'a', encoding='utf-8') as f:
            f.write('\n')
            f.write(title[sys_t])
        f.close()
        for sys, noms in sys_nom_sorted.items():
            nom_list = list(noms)
            nom_list.sort(key=position)
            gentex(filename, pdfdict, sys, nom_list)
