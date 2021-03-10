# This script builds the OSDU image for ingestion/delivery

from __future__ import print_function

import os
import sys
import getopt
import subprocess

# defaults
use_buildkit = False
dockerfile = 'docker/osdu.Dockerfile'
openvdsrepo = 'https://community.opengroup.org/osdu/platform/domain-data-mgmt-services/seismic/open-vds.git'
repotag = 'master'
containerreglogin = 'docker login community.opengroup.org:5555'
containerregistry = 'community.opengroup.org:5555/osdu/platform/domain-data-mgmt-services/seismic/open-vds'
containername = '/openvds-ingestion'

def runBuildProcess(arg):
    global use_buildkit, dockerfile, openvdsrepo, repotag, containerreglogin, containerregistry
    return_code = None

    # clean up args, so we don't have empty arguments
    while('' in arg) :
        arg.remove('')

    # add 'DOCKER_BUILDKIT=1' to environment variables, if we enable the new docker build system
    my_env = os.environ.copy()
    if use_buildkit:
        my_env['DOCKER_BUILDKIT'] = '1'

    print('build command:', " ".join(arg))
    process = subprocess.Popen(arg, stdout=subprocess.PIPE, env=my_env, universal_newlines=True)

    # output stdout in real-time, but also store the output internally, in case we need to parse it later
    output_cache = ""
    while True:
        output = process.stdout.readline()
        output_cache += output
        if output:
            print(output.strip())
        return_code = process.poll()
        if return_code is not None:
            for output in process.stdout.readlines():
                output_cache += output
                if output:
                    print(output.strip())
            break
    return (return_code, output_cache)

def main(argv):
    global use_buildkit, dockerfile, openvdsrepo, repotag, containerreglogin, containerregistry
    try:
        opts, args = getopt.getopt(argv, "hbnt:r:", ["help", "use-buildkit", "no-cache", "tag=", "repo="])
    except getopt.GetoptError as e:
        print("abort:", e.msg)
        sys.exit(2)

    buildkit_arg = ''
    nocache_arg = ''
    for opt, arg in opts:
        if opt in ('-h', '--help'):
            print('build_osdu_image.py [-b, --use-buildkit] [-n, --no-cache] [-t <tag>, --tag=<tag>] [-r <repo url>, --repo <url>]')
            sys.exit()
        elif opt in ("-b", "--use-buildkit"):
            use_buildkit = True
            buildkit_arg = '--progress=plain'
        elif opt in ("-n", "--no-cache"):
            nocache_arg = '--no-cache'
        elif opt in ("-t", "--tag"):
            repotag = arg
        elif opt in ("-r", "--repo"):
            openvdsrepo = arg

    (return_code, output) = runBuildProcess(['docker', 'build',
                                  buildkit_arg,
                                  nocache_arg,
                                  '--build-arg', 'repo='+openvdsrepo,
                                  '--build-arg', 'tag='+repotag,
                                  '-t', containerregistry + containername + ':' + repotag,
                                  '-f', dockerfile, '.'])
    print('build status:', return_code)
    runBuildProcess(['docker', 'tag',
                    containerregistry + containername + ':' + repotag,
                    containerregistry + containername + ':latest'])
    print('\npush new image with:')
    print('  docker push ' + containerregistry + containername + ':' + repotag)
    print('  docker push ' + containerregistry + containername + ':latest')

if __name__ == "__main__":
    main(sys.argv[1:])
