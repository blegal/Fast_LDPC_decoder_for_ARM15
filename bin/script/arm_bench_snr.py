#!/usr/bin/python
import subprocess

#C_LIST=['20000x10000']
C_LIST=['1944x972', '2048x384', '2304x1152', '4000x2000', '4896x2448', '8000x4000', '9972x4986', '20000x10000', '64800x21600', '64800x32400']
#C_LIST=['576x288', '1944x972', '2304x1152', '4000x2000', '8000x4000', '20000x10000', '64800x32400']
I_LIST=['5', '10', '15', '20', '25', '30']
SNR_LIST=['0.5', '0.75', '1.00', '1.25', '1.50', '1.75', '2.00', '2.25', '2.50', '2.75', '3.00', '3.25', '3.50', '3.75', '4.00', '4.25', '4.50', '4.75', '5.00', '5.25', '5.50', '5.75', '6.00' , '6.25', '6.50', '6.75', '7.00' , '7.25', '7.50', '7.75', '8.00', '8.25', '8.50', '8.75', '9.00', '9.25', '9.50', '9.75', '10.00' ]

subprocess.call('echo ""  > results_article_snr',shell=True)

for C in C_LIST:
	subprocess.call('echo "CODE ' + C + '"  >> results_article_snr',shell=True)
#	subprocess.call('echo "#include \\"./' + C + '/constantes.h\\""     > ../src/Constantes/constantes.h',shell=True)
#	subprocess.call('echo "#include \\"./' + C + '/constantes_sse.h\\"" > ../src/Constantes/constantes_sse.h',shell=True)
#	subprocess.call('make -f Makefile.gcc.4.6 clean',shell=True)
#	subprocess.call('make -f Makefile.gcc.4.6 -j 8',shell=True)
#	for SNR in SNR_LIST:
#		CMD='./main.gcc.4.6 -float -sse -OMS 0.15 -stop -fer 10000000 -min ' + SNR + ' -max 100.0 -pas 0.1 -timer 20 -iter 20 | grep "(PERF) SNR =" >> results_article_snr'
#		subprocess.call(CMD,shell=True)
#	for SNR in SNR_LIST:
#		CMD='./main.gcc.4.6 -float -sse -OMS 0.15 -stop -fer 10000000 -min ' + SNR + ' -max 100.0 -pas 0.1 -timer 20 -iter 20 -thread 4 | grep "(PERF) SNR =" >> results_article_snr'
#		subprocess.call(CMD,shell=True)
for SNR in SNR_LIST:
	subprocess.call('echo "SNR = ' + SNR + '"  >> results_article_snr',shell=True)
	for C in C_LIST:
		subprocess.call('echo "SNR = ' + SNR + ' -  CODE = ' + C + '"',shell=True)
		CMD='./main.icc.' + C + ' -fixed -avx -OMS 1 -stop -fer 10000000 -min ' + SNR + ' -max 100.0 -pas 0.1 -timer 20 -iter 20 | grep "(PERF) SNR =" | grep THROUGHPUT >> results_article_snr'
		subprocess.call(CMD,shell=True)
#	for SNR in SNR_LIST:
#		CMD='./main.gcc.4.6 -fixed -sse -OMS 1 -stop -fer 10000000 -min ' + SNR + ' -max 100.0 -pas 0.1 -timer 20 -iter 20 -thread 4 | grep "(PERF) SNR =" >> results_article_snr'
#		subprocess.call(CMD,shell=True)
