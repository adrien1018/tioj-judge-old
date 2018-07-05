#!/usr/bin/env python3

import argparse, subprocess, sys, os

# TODO: MySQL config

parser = argparse.ArgumentParser(description = '')
parser.add_argument('--boxpath', default = '/tmp/tioj-box', help = 'Directory to make sandboxes (default: /tmp/tioj-box)')
parser.add_argument('--datapath', default = '/var/lib/tioj-judge', help = 'Directory to store judge data (default: /var/lib/tioj-judge)')
args = parser.parse_args()

def c_string(s):
    return '"' + s.translate(str.maketrans({'\\': r'\\', '"': r'\"'})) + '"'

def c_string_list(lst):
    ret = '{'
    for i in lst: ret += c_string(i) + ', '
    return ret[:-2] + '}'

def get_path(name):
    ps = subprocess.Popen(['which', name], stdout = subprocess.PIPE, stderr = subprocess.PIPE)
    return ps.communicate()[0][:-1].decode()

def get_stats(cmd, input = ''):
    ps = subprocess.Popen(['strace', '-fc'] + cmd, stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.PIPE)
    result = ps.communicate(input = input.encode())[1].decode()
    lns = result.split('\n')
    # print(result)
    pos = [i for i in range(len(lns)) if lns[i].find('----') != -1][0]
    return [ln.split()[-1] for ln in result.split('\n')[pos+1:-3]]

def combine_stats(*res):
    ret = []
    for i in res: ret += i
    ret = [*{*ret}]
    return ret

prog_list = ['gcc', 'g++', 'ghc', 'python2', 'python3']
prog_path = {}

for prog in prog_list:
    print('Getting path to', prog, '... ', end = '')
    prog_path[prog] = get_path(prog)
    if prog_path[prog] == '':
        print('\nCannot find', prog)
        sys.exit(1)
    print(prog_path[prog])

os.chdir('config')
print('Entering ./config ...')

compile_stats = {}
run_stats = {}

# C/C++
print('Getting system call list of C/C++...')
compile_stats['CCpp'] = combine_stats(
    get_stats(['g++', '-std=c++14', '-static', 'cpp_ce.cpp', '-o', 'cpp_ce']), # compilation error
    get_stats(['g++', '-std=c++14', '-static', 'cpp_1.cpp', '-o', 'cpp_1']), # linking error
    get_stats(['g++', '-std=c++14', '-static', '-c', 'cpp_1.cpp', '-o', 'cpp_1.o']),
    get_stats(['g++', '-std=c++14', '-static', '-c', 'cpp_header.cpp', '-o', 'cpp_header.o']),
    get_stats(['g++', '-o', 'cpp_1', 'cpp_1.o', 'cpp_header.o']))
run_stats['CCpp'] = combine_stats(
    get_stats(['./cpp_1'], '0'),
    get_stats(['./cpp_1'], '1'),
    get_stats(['./cpp_1'], '2'))

# TODO: Add other langs, query dynamic linking libs and processes count

# Python 2/3
print('Getting system call list of Python...')
run_stats['Python'] = combine_stats(
    get_stats(['python3', 'python3_1.py'], '0'),
    get_stats(['python3', 'python3_1.py'], '1'))

os.chdir('../')
print('Leaving directory...')

print('Generating src/config.cpp...')
with open('src/config.cpp', 'w') as f:
    f.write('#include "config.h"\n\n')
    f.write('const std::string kBoxPath = ' + c_string(args.boxpath) + ';\n')
    f.write('const std::string kDataPath = ' + c_string(args.datapath) + ';\n')
    for i in prog_path.items():
        if i[0] == 'g++': i = ('gpp', i[1])
        f.write('const std::string k_' + i[0] + 'Path = ' + c_string(i[1]) + ';\n')
    for i in compile_stats.items():
        f.write('const std::vector<std::string> k' + i[0] + 'CompileSyscalls = ' + c_string_list(i[1]) + ';\n')
    f.write('\n')
    for i in run_stats.items():
        f.write('const std::vector<std::string> k' + i[0] + 'RunSyscalls = ' + c_string_list(i[1]) + ';\n')

# C: gcc
# Cpp: g++
# Haskell: ghc
# Python: python2, python3

