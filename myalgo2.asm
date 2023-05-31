;storing 3 numbers in memory and retriving it back and adding it

ldc 1 ;B=0,A=1
stl 0 ;memory[sp]=1,A=0
ldc 2 ;B=0,A=2;
stl 1 ;memory[sp+1]=2, A=0
ldl 0 ;B=0,A=memory[sp]=1
sp2a  ;B=1,A=sp
adc 1 ;A=sp+1;
ldnl 0 ;A=memory[sp+1]=2;
add  ;A=B+A=3
