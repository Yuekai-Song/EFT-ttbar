import generate_ew
import generate_sys
import os


# To obtain the str needed in latex file

def obtain_initial(filename):
    lines = []
    with open(filename, 'r', encoding='utf-8') as f:
        for line in f.readlines():
            if line != '\\end{document}':
                lines.append(line)
    f.close()
    return lines

if __name__ == "__main__":
    path = os.path.abspath("")
    dir_sys = path + "/../sys_pdf/sys_kappa_same"
    dir_ew = path + "/../ew_pdf"
    modelfile = obtain_initial('./temp/temp.tex')
    filename = "./ttbar/ttbar.tex"

    with open(filename, 'w', encoding='utf-8') as f:
        for line in modelfile:
            f.write('%s' % line)
    f.close()

    generate_ew.ew_add(dir_ew, filename)
    generate_sys.sys_add(dir_sys, filename)

    with open(filename, 'a', encoding='utf-8') as f:
        f.write('\\end{document}\n')
    f.close()
    print('END')