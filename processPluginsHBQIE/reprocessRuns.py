import os
import sys

runs = sys.argv[1].split(',')
for run in runs:
    os.system('./reprocessRun.sh {0}'.format(run))
