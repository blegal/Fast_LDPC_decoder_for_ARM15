#
# CROS-COMPILATION
#
#CC=arm-linux-gnueabihf-g++-4.9
#CFLAGS=-fsigned-bitfields -I../cpp_src -W -Wall -O3 -fopenmp -march=armv7-a -mtune=cortex-a15 -marm -mfpu=neon -finline -funroll-loops -std=c++11 -I../src -I/opt/local/include/ -mfpu=neon -flax-vector-conversions -static

#
# NATIVE COMPILATION
#
CC=g++-4.8
CFLAGS=-fsigned-bitfields -I../cpp_src -W -Wall -O3 -fopenmp -march=native -mtune=cortex-a15 -marm -mfpu=neon -finline -funroll-loops -std=c++11 -I../src -I/opt/local/include/ -mfpu=neon -flax-vector-conversions -static
LDFLAGS=-L/opt/local/lib
EXEC=main.icc

SRC= \
 ../src/CBitGenerator/CBitGenerator.cpp \
 ../src/CChanel/CChanel.cpp             \
 ../src/CChanel/CChanelAWGN_x86.cpp     \
 ../src/CChanel/CChanelForFixed.cpp     \
 ../src/CDecoder/template/CDecoder.cpp  \
 ../src/CDecoder/template/CDecoder_fixed.cpp     \
 ../src/CDecoder/template/CDecoder_fixed_x86.cpp \
 ../src/CDecoder/template/CDecoder_fixed_SSE.cpp \
 ../src/CDecoder/OMS/CDecoder_OMS_fixed_x86.cpp  \
 ../src/CDecoder/OMS/CDecoder_OMS_fixed_SSE.cpp  \
 ../src/CDecoder/OMS/CDecoder_OMS_fixed_NEON16_v2.cpp \
 ../src/CEncoder/CFakeEncoder.cpp   \
 ../src/CEncoder/Encoder.cpp        \
 ../src/CEncoder/GenericEncoder.cpp \
 ../src/CErrorAnalyzer/CErrorAnalyzer.cpp          \
 ../src/CFixPointConversion/CFastFixConversion.cpp \
 ../src/CFixPointConversion/CFixConversion.cpp     \
 ../src/CTerminal/CTerminal.cpp   \
 ../src/CTimer/CTimer.cpp         \
 ../src/CTrame/CTrame.cpp         \
 ../src/CTools/transpose_neon.cpp \
 ../src/main_p.cpp

OBJ= $(SRC:.cpp=.o)

all: $(EXEC)

main.icc: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	find ../src/ -name "*.o" -exec rm {} \;
	find . -name "main.*"    -exec rm {} \;

mrproper: clean
	rm $(EXEC)
