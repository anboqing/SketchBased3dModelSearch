#encoding=utf-8

import os
import re

def scan_dir(ostream,dirname,names):
    if dirname != '.':
        print 'searching directory : ',dirname,"..."
        m = re.search('(\d+)',dirname)
        label = int(m.group(0).__str__())
        for name in names:
            path = dirname[2:]+'/'+name
            line = path + ' '+str(label)+'\n'
            ostream.write(line)


if __name__ == '__main__':
    with open('file_list.txt','w') as ostream:
        os.path.walk('.',scan_dir,ostream)
        ostream.close()
