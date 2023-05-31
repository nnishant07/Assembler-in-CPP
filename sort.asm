

ldc 0x1000 	;B=A=0,A=0x1000
a2sp 		;sp=0x1000 , A=0
adj -1 		;sp=sp-1
ldc 11 		;B=0,A=11
stl 0 		;memory[sp+0]=11,A=0
ldc array	;B=0,A=array
call sort	;B=array, A=pc,pc=pc+sort
HALT 		


; A will contain the return address 
; B will contain the address of elements 
; SP+0 represents number of elements sort

sort: adj -1		;sp=sp-1
stl 0			;memory[sp]=A,A=B
;saving return address in stack
stl 2			;memory[sp+2]=A,A=B
;save address of elements
adj -3			;sp=sp-3
ldc 0			;B=A,A=0
stl 0			;memory[sp]=A,A=B;
;saving temp to swap		
ldc 1			;B=A,A=1
stl 1			;memory[sp+1]=A,A=B
;saving b			
ldc 0			;B=A,A=0
stl 2			;memory[sp+2]=A,A=B
;saving a
loop_out: ldl 4		;B=A,A=memory[sp+4]
;load count	
ldl 2			;B=A,A=memory[sp+2]
;load x
sub			;A=B-A
;count - x
ldc 1			;B=A,A=1
sub			;A=B-A
;count-1-x		
brz done		;if(A==0){PC+done}
ldc 1			;B=A,A=1	
stl 1			;memory[sp+1]=A,A=B
;b = 1			
loop_in: ldl 4		;B=A,A=memory[sp+4]
;load count
ldl 2			;B=A,A=memory[sp+2]
;load x			
sub			;A=B-A
;count - x		
ldl 1			;B=A,A=memory[sp+1]
;load y 		
sub 			;A=B-A
;count - x -y
brz addx		;if(A==0){PC+addx}
ldl 5			;B=A,A=memory[sp+5]
;load address of elements
ldl 1			;B=A,A=memory[sp+1]
ldc 1			;B=A,A=1
sub			;A=B-A
ldl 5			;B=A,A=memory[sp+5]
add			;A=B+A;
stl 6			;memory[sp+6]=A,A=B
ldl 1			;B=A,A=memory[sp+1]
ldl 5			;B=A,A=memory[sp+5]
add			;A=B+A
stl 7			;memory[sp+6]=A,A=B
ldl 7			;B=A,A=memory[sp+7]
ldnl 0			;A=memory[A]
ldl 6			;B=A,A=memory[sp+6]
ldnl 0			;A=memory[A]
sub			;A=B-A
;array[y-1]-array[y]
brlz swap		;if(A<0) pc=pc+offset
br addy			;pc=pc+offset
swap: 		
ldl 6			;B=A,A=memory[sp+6]
;load address of elements
ldnl 0			;A=memory[A]
;array[y-1]
stl 0			;memory[sp]=A,A=B
;temp = array[y-1]
ldl 7			;B=A,A=memory[sp+7]
ldnl 0			;A=memory[A]
;array[y]
ldl 6			;B=A,A=memory[sp+6]
stnl 0			;memory[A]=B
;array[y-1] = array[y]	
ldl 0			;B=A,A=memory[sp]
;load temp
ldl 7			;B=A,A=memory[sp+7]
stnl 0			;memory[A]=B
;array[y] = temp
;increment
addy: ldc 1		;B=A,A=1
ldl 1			;B=A,A=memory[sp+1]
;load y
add			;A=B+A
;y++
stl 1			;memory[sp+1]=A,A=B
;save y++
br loop_in		;pc=pc+loop_in
addx: ldc 1		;B=A,A=1
ldl 2			;B=A,A=memory[sp+2]
;load x 
add			;A=B+A
;x++
stl 2			;memory[sp+2]=A,A=B
ldc 1			;B=A,A=1
ldl 5			;B=A,A=memory[sp+2]
add			;A=B+A
;array++
br loop_out		;pc=pc+loop_in
done: ldl 3		;B=A,A=memory[sp+3]
;load return address
adj 5			;sp=sp+5
;sp = array
return			
;ok, 
array: data 9 
data 8 
data 7 
data 6 
data 5 
data 4 
data 3 
data 2
data 1 
data 0
data 0



; int* sort(int* in,int count)
;{
;   int x;
;   int y;
;   int temp;
;   for(x=0;x < count-1;x++)
;   {
;       for(y=1;y<count-x;y++)
;       {
;           if(((in+y))>((in+y-1)))
;           {
;               temp=(*(in+y-1));
;               ((in+y-1))=((in+y));
;               (*(in+y))=temp;
;           }
;       }
;   }
;   exit:return;
;} 
