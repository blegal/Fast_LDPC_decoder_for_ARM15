#!/usr/bin/python
import subprocess
import os

C_LIST = []
for root, dirs, files in os.walk("../src/Constantes/"):
  for code in dirs:
    C_LIST.append( code )
  break

GCC_LIST=['.a15']

subprocess.call('rm main.*',shell=True)
for C in C_LIST:
	subprocess.call('echo "CODE ' + C + '"  >> results_article',shell=True)
	subprocess.call('echo "#include \\"./' + C + '/constantes.h\\""     > ../src/Constantes/constantes.h',shell=True)
	subprocess.call('echo "#include \\"./' + C + '/constantes_sse.h\\"" > ../src/Constantes/constantes_sse.h',shell=True)
	for COMPILER in GCC_LIST:
		subprocess.call('echo "COMPILATION OF LDPC DECODER FOR CODE [' + C + ']"',shell=True)
		output = subprocess.check_output('make -f Makefile clean',shell=True)
		output = subprocess.check_output('make -f Makefile -j 32',shell=True)
		output = subprocess.check_output('mv main.icc main' + COMPILER + '.' + C + '',shell=True)
