#!/usr/bin/python
import subprocess

C_LIST=['576x288', '960x480', '1152x576', '1248x624', '1536x768', '1944x972', '2304x1152', '9216x4608', '16200x7560']
I_LIST=['3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13', '14', '15', '16', '17', '18', '19', '20', '21', '22', '23', '24', '25']

for C in C_LIST:
	for ITERS in I_LIST:
		CMD='./main.47.' + C + ' -fixed -sse -OMS2 1    -fer 10000000 -min 0.5 -max 1.0 -pas 0.1 -timer 10 -iter ' + ITERS + ' | grep ", THROUGHPUT =" >> r.txt'
		subprocess.call(CMD,shell=True)
		subprocess.call('tail -n 1 r.txt', shell=True)
