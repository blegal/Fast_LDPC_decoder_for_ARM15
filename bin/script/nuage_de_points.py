#!/usr/bin/python
import subprocess
import os

C_LIST = []
for root, dirs, files in os.walk("../src/Constantes/"):
  for code in dirs:
    C_LIST.append( code )
  break
  
GCC_LIST = ['.45']
TARGET   = 'a15'

for C in C_LIST:
	subprocess.call('echo "CODE ' + C + '"  >> results_article',shell=True)
	for COMPILER in GCC_LIST:
		subprocess.call('echo "COMPILATION OF LDPC DECODER FOR CODE [' + C + ']"',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -max 0.00 | grep Code >> r.txt', shell=True)

for C in C_LIST:
	for COMPILER in GCC_LIST:
		subprocess.call('echo "COMPILATION OF LDPC DECODER FOR CODE [' + C + ']"',shell=True)
		subprocess.call('echo "COMPILATION OF LDPC DECODER FOR CODE [' + C + ']" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter 10 -timer 30 -fer 10000000 -OMS2 1 -thread 1 | grep "Total Kernel throughput" >> r.txt',shell=True)

for C in C_LIST:
	for COMPILER in GCC_LIST:
		subprocess.call('echo "COMPILATION OF LDPC DECODER FOR CODE [' + C + ']"',shell=True)
		subprocess.call('echo "COMPILATION OF LDPC DECODER FOR CODE [' + C + ']" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter 10 -timer 30 -fer 10000000 -OMS2 1 -thread 4 | grep "Total Kernel throughput" >> r.txt',shell=True)
