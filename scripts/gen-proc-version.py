import subprocess
import string
from datetime import datetime
from optparse import OptionParser

def run_cmd(cmd):
    res = subprocess.run(cmd, universal_newlines=True,
                         stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT)
    if res.returncode == 0:
        return res.stdout.strip()
    return 'unknown'

def git_commit():
    cmd = [ 'git', 'log', '--pretty=format:%h', '-n', '1' ]
    return run_cmd(cmd)

def git_branch():
    cmd = [ 'git', 'symbolic-ref', '--short', 'HEAD' ]
    return run_cmd(cmd)

def cc_version(cc):
    cmd = [ cc, '--version' ]
    return run_cmd(cmd).splitlines()[0]

parser = OptionParser()

parser.add_option("-n", "--name", action="store", dest="name")
parser.add_option("-a", "--arch", action="store", dest="arch")
parser.add_option("-c", "--cpu", action="store", dest="cpu")
parser.add_option("-u", "--user", action="store", dest="user", default="unknown")
parser.add_option(      "--host", action="store", dest="host", default="unknown")
parser.add_option(      "--major", action="store", dest="major", default=0)
parser.add_option(      "--minor", action="store", dest="minor", default=0)
parser.add_option(      "--micro", action="store", dest="micro", default=0)
parser.add_option(      "--cc-version", action="store_true", dest="cc_version")
parser.add_option(      "--git-commit", action="store_true", dest="git_commit")
parser.add_option(      "--git-branch", action="store_true", dest="git_branch")

(opts, args) = parser.parse_args()

version = '.'.join(map(lambda x: str(x), [opts.major, opts.minor, opts.micro]))
if opts.git_commit:
    version = '{}-{}'.format(version, git_commit())
if opts.git_branch:
    version = '{}/{}'.format(version, git_branch())
hostname = '@'.join([opts.user, opts.host])
target = ', '.join(filter(lambda x: x is not None, [opts.arch, opts.cpu]))

print('{} version {} ({}) ({}) #{}'.format(opts.name, version, hostname, target,
                                           datetime.now().strftime("%c")))
if (opts.cc_version):
    print(cc_version('arm-none-eabi-gcc'))
