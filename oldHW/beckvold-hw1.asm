#Ben Beckvold
#Computer Systems: HW #1

.globl main

	.data 
x: .word 0
result: .word 0
prompttext: .asciiz "Positive Integer: "
resultstr1: .asciiz "The value of 'factorial("
resultstr2: .asciiz ") is: "
wronginput: .ascii "Enter an integer value greater than 0.\n"
	
	.text
main:

#print input string
	la $a0, prompttext
	addi $v0, $zero, 4	#print string
	syscall	
#read integer
	addi $v0, $zero, 5	#read word
	syscall
	add $s0, $v0, $zero 	#store x in s0
	
	blez $s0, wrong		#input <= 0, go to end with error msg
	
#load variables result and x
	add $s1, $zero, $s0	#result = x to start
	add $a1, $s0, -1	#store x-1 in a1
	beq $s1, 1, endresult	#if x=1, fact(x)=1, go the end
	
	add $a0, $zero, $s1	#store result in argument register
	addi $sp, $sp, -4	#move stack pointer
	sw $ra, 0($sp)		#save return address
	jal factorial		#function call
	
endresult:
#print results
	la $a0, resultstr1 #load first part of text
	addi $v0, $zero, 4 #print string
	syscall
	add $a0, $s0, $zero #load x
	addi $v0, $zero, 1  #print int
	syscall	
	la $a0, resultstr2 #load second part of text
	addi $v0, $zero, 4 #print string
	syscall	
	add $a0, $s1, $zero #load result
	addi $v0, $zero, 1 #print int
	syscall
	
endmain:
#exit program
	addi $v0, $zero, 10
	syscall
	
factorial:	
	beqz $a1, endfact	
	mult $a1, $a0	#result * x-1
	mflo $s1	#move new result into s1
	mflo $a0	#move new result into arugment
	add $a1, $a1, -1  #x-1
	addi $sp, $sp, -4	#move stack pointer
	sw $ra, 0($sp)		#save return address
	jal factorial		#function call
endfact:
	lw $ra, ($sp)	#load prevous $ra
	addi $sp, $sp 4	#move stack pointer
	jr $ra		#jump back to $ra

wrong:
	la $a0, wronginput #load wrong input warning
	addi $v0, $zero, 4 #print string
	syscall	
	j main	#go back to main prompt
