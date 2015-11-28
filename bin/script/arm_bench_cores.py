#!/usr/bin/python
import subprocess
import os

C_LIST = []
for root, dirs, files in os.walk("../src/Constantes/"):
  for code in dirs:
    C_LIST.append( code )
  break
  
GCC_LIST = ['.45']
C_LIST   = ['64800x32400']
TARGET   = 'a15'

for C in C_LIST:
	subprocess.call('echo "CODE ' + C + '"  >> results_article',shell=True)
	for COMPILER in GCC_LIST:
		subprocess.call('echo "COMPILATION OF LDPC DECODER FOR CODE [' + C + ']"',shell=True)
		subprocess.call('echo "COMPILATION OF LDPC DECODER FOR CODE [' + C + ']" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter  5 -timer 10 -fer 10000000 -OMS2 1 -thread 1 | grep "Total Kernel throughput" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter 10 -timer 10 -fer 10000000 -OMS2 1 -thread 1 | grep "Total Kernel throughput" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter 13 -timer 10 -fer 10000000 -OMS2 1 -thread 1 | grep "Total Kernel throughput" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter 15 -timer 10 -fer 10000000 -OMS2 1 -thread 1 | grep "Total Kernel throughput" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter 20 -timer 10 -fer 10000000 -OMS2 1 -thread 1 | grep "Total Kernel throughput" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter  5 -timer 10 -fer 10000000 -OMS2 1 -thread 2 | grep "Total Kernel throughput" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter 10 -timer 10 -fer 10000000 -OMS2 1 -thread 2 | grep "Total Kernel throughput" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter 13 -timer 10 -fer 10000000 -OMS2 1 -thread 2 | grep "Total Kernel throughput" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter 15 -timer 10 -fer 10000000 -OMS2 1 -thread 2 | grep "Total Kernel throughput" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter 20 -timer 10 -fer 10000000 -OMS2 1 -thread 2 | grep "Total Kernel throughput" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter  5 -timer 10 -fer 10000000 -OMS2 1 -thread 4 | grep "Total Kernel throughput" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter 10 -timer 10 -fer 10000000 -OMS2 1 -thread 4 | grep "Total Kernel throughput" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter 13 -timer 10 -fer 10000000 -OMS2 1 -thread 4 | grep "Total Kernel throughput" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter 15 -timer 10 -fer 10000000 -OMS2 1 -thread 4 | grep "Total Kernel throughput" >> r.txt',shell=True)
		subprocess.call('./main.' + TARGET + '.' + C + ' -fixed -sse -iter 20 -timer 10 -fer 10000000 -OMS2 1 -thread 4 | grep "Total Kernel throughput" >> r.txt',shell=True)
