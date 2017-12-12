#Ben Beckvold
#Computer Systems: HW #2

#----GRADERS------FYI-----
# i missed the second layer of pointers to the data.
# this allowed all the helper functions to work but when
# the sorting function starts there is a missing step, the sort should
# work with the data i have but none of the other helper functions work.
# i realised this too late to fix. If you comment out the sort call, it runs 
# and you can see the printed arrays.

.globl main

	.data 
#strings used
initialtext: .asciiz "The initial array is: \n"
finaltext: .asciiz "The insertion sort is finished!\n"
openbkt: .asciiz "["
closebkt: .asciiz "] "
routinefirst: .asciiz "Insertion sort has executed!"
routinelast: .asciiz "The final array is: \n"
routinex: .asciiz ", x="
crtext: .asciiz "\n"
	
#array variables
size: .word 16

		.align 5
dataName:	.asciiz "Joe"
		.align 5
		.asciiz "Jenny"
		.align 5
		.asciiz "Jill"
		.align 5
		.asciiz "John"
		.align 5
		.asciiz "Jeff"
		.align 5
		.asciiz "Joyce"
		.align 5
		.asciiz "Jerry"
		.align 5
		.asciiz "Janice"
		.align 5
		.asciiz "Jake"
		.align 5
		.asciiz "Jonna"
		.align 5
		.asciiz "Jack"
		.align 5
		.asciiz "Jocelyn"
		.align 5
		.asciiz "Jessie"
		.align 5
		.asciiz "Jess"
		.align 5
		.asciiz "Janet"
		.align 5
		.asciiz "Jane"
		
		.align 2
dataAddr:	.space 64
	
	.text
main:
	lw $s7, size		#load size=16 into reg
	la $a2, dataName	#load address of names array
#initialize pointer array from data array
	jal initArrAddr
	add $s1, $zero, $v0	#move returned value into local register
#print initial text
	la $a0, initialtext
	addi $v0, $zero, 4	#print string
	syscall
#print array	
	add $a0, $zero, $s7	#move size=16 to arg
	add $a1, $zero, $s1	#move local address of pointer array to argument reg
	jal printarray		#prints array to screen, $a0 is 'size=16, $a1 is pointer array
#sort array	
	add $a0, $zero, $s7	#move size=16 to arg	
	jal sortarray		#insertion sort, $a0 is 'size=16
#print 'completed" text	
	la $a0, finaltext
	addi $v0, $zero, 4	#print string
	syscall	
#print sorted array		
	lw $a0, size		#load size=16 into argument reg
	la $a1, dataAddr
	jal printarray		#prints array to screen
	j endmain		#ends program
	
printarray:	# $a0 is 'size'=16 and $a1 is dataAddr
#print array
	add $s0, $zero, $a0	#load 'size'=16 into local register
	add $s1, $zero, $a1	#load pointer array into local reg
	add $t0, $zero, $s0	#decremeter
	la $t1, openbkt 	#load '['
	la $t2, closebkt 	#load ']'
	la $t3, crtext		#load '\n'
				
readloop:
	lw $s2, ($s1)
	addi $s1, $s1, 4
	add $a0, $zero, $t1	#move '[' into arg
	addi $v0, $zero, 4 	#print string
	syscall
	add $a0, $zero, $s2 	#load data @ pointer
	addi $v0, $zero, 4  	#print name string
	syscall	
	add $a0, $zero, $t2	#move ']' into arg
	addi $v0, $zero, 4 	#print  string
	syscall	
	
	addi $t0, $t0, -1	#decrement counter
	bnez $t0, readloop	#loop control logic (if >0)
	
	add $a0, $zero, $t3	#move '\n' into arg
	addi $v0, $zero, 4 	#print string
	syscall	
	
	jr $ra		#return

#---------------------------------
sortarray:	#a0 is size=16
	add $s6, $zero, $a0	#length=16
	lw $s7, dataAddr	#load array address into local reg
	addi $s5, $s7, 32	#load array address into local reg
	addi $t7, $zero, 1	#set outer loop counter i=1
	add $t4, $zero, $zero	#outer loop array reference
	add $t6, $zero, $zero	#inner loop counter j=0
	addi $t5, $zero, 4	#inner loop array reference
	
	la $a0, routinefirst 	#load 'executed' text
	addi $v0, $zero, 4  	#print string
	syscall

outfor:	
	lw $a0, ($s7)		#value=a[i]
	
infor:
	add $t6, $t7, -1	#j=i-1
	lw $a1, ($s5)		#load location of second string a[j]
	
	#str_lt call
	addi $sp, $sp, -4	#move stack pointer
	sw $ra, ($sp)		#store $ra before inner function call
	jal str_lt		#compare strings $a0, $a1 (value, a[j])
	lw $ra, ($sp)		#load $ra back after returning
	addi $sp, $sp, 4	#move stack pointer back
	
	bltz $t6, endinfor	#if (j<0)
	beqz $v0, endinfor	#if (str_lt == 0)			
	
	addi $s1, $a1, 4	#a[j+1]
	add $s1, $zero, $a1	#a[j+1] = a[j]
	
	add $t6, $t6, -1	#j--
	add $s5, $s5, -4	#next array address (4*j)
	b infor

endinfor:
	addi $s1, $a1, 4	#a[j+1]
	add $s1, $zero, $a0	#a[j+1] = value
		
	add $t7, $t7, 1		#increment outer loop i++
	add $s7, $s7, 4		#next array address (4*i)	
	blt $t7, $s6, outfor	#only do it 16 times
main_rtn:		
	jr $ra		#return
#---------------------------------

str_lt:
#function to compare 2 strings passes, return 1 for first < second, or 0 for second < first
#arg passes are for the base addr of the string
	#place used register data onto stack
	addi $sp, $sp, -4
	sw $s0, ($sp)
	addi $sp, $sp, -4
	sw $s1, ($sp)
	
	add $s0, $zero, $a0	#first string
	lb $t0, ($s0)	#set contents of address to temp local
	add $s1, $zero, $a1	#second string
	lb $t1, ($s1)	#set contents of address to temp local
	add $v0, $zero, $zero	#return value default 0

ltloop:				#for loop
	beqz $t0, endlt	#if /0 end
	beqz $t1, endlt	#if /0 end
	beq $t0, $t1, advance	#if same, advance to next character
	blt $t0, $t1, rtnlt		#if less exit w/ return value 1
	b endlt
advance:				#go to next character in string
	add $s0, $s0, 1		#next byte of address
	lb $t0, ($s0)		#load next byte
	add $s1, $s1, 1
	lb $t1, ($s1)
	b ltloop		#go back and compare next char
	
rtnlt:	addi $v0, $zero, 1	#set return value to 1
endlt:
	#pop stored contents off the stack back into their respective registers (previous order they were placed)
	lw $s1, ($sp)
	addi $sp, $sp, 4
	lw $s0, ($sp)
	addi $sp, $sp, 4
	
	jr $ra			#return

initArrAddr:
#function to initialize array of pointers to the data arrray
	add $s0, $zero, $a2	#transfer address from argument to local register
	addi $t2, $zero, 16	#decrementer
	la $s1, dataAddr	#load address of pointer array
	add $t1, $zero, $s1
initloop:	
	sw $s0, ($s1)		#store address of name at pointer array address
	addi $s0, $s0, 32	#get next name address
	addi $s1, $s1, 4	#get next pointer array address
	addi $t2, $t2, -1	#decrement counter
	bnez $t2, initloop		#keep assigning addresses if not yet done all 16
	
	add $v0, $zero, $t1	#moves the initial array address into the returned value
	jr $ra			#return
	
endmain:
#exit program
	addi $v0, $zero, 10
	syscall
