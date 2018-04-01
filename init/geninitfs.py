import errno, os.path, sys
from optparse import OptionParser
from os import listdir, remove, stat
from stat import *

def main():
    parser = OptionParser()
    parser.add_option("-b", "--bfdarch", action="store", dest="bfdarch")
    parser.add_option("-d", "--dir", action="store", dest="dirpath")
    parser.add_option("-o", "--output", action="store", dest="outfile")
    parser.add_option("-v", "--verbose", action="store_true", dest="verbose")
    (opts, args) = parser.parse_args()

    bfdendian = {'arm': 'little'}

    try:
        os.remove(opts.outfile)
    except OSError as e:
        if e.errno != errno.ENOENT:
            raise

    for f in listdir(opts.dirpath):
        if opts.verbose:
            print("Adding %s" % f)
        pathname = os.path.join(opts.dirpath, f)
        if stat(pathname).st_mode:
            # write filesize
            with open(opts.outfile, 'ab') as of:
                of.write(os.path.getsize(pathname).to_bytes(2, byteorder=bfdendian[opts.bfdarch]))
            with open(opts.outfile, 'a') as of:
                # write filename
                of.write("%s\0" % f)
                # align to next 4-byte boundary
                if of.tell() % 4:
                    for i in range(of.tell() % 4, 4):
                        of.write('\0')
                # concat input file
                with open(pathname) as infile:
                    for line in infile:
                        of.write(line)
                # align to next 4-byte boundary
                if of.tell() % 4:
                    for i in range(of.tell() % 4, 4):
                        of.write('\0')
        elif opts.verbose:
            print('Skipping %s' % pathname)


if __name__ == '__main__':
    main()
