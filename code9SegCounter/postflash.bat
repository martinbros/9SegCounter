mode COM8 BAUD=115200 PARITY=n DATA=8
echo m 5 > COM8
TIMEOUT 1
echo y > COM8
putty -load "busPirate"