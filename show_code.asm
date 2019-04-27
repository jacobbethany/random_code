;
; Created on 2019-04-27 by Jacob Bethany
; Purpose: To see if it would be possible to make a program dump its own opcode values.
;
; Note: The final opcode in this program isn't really completely detectable, with the way that I'm doing this.
; A few opcodes at the bottom of the script are missing (as well as some functions below the label that is used).
;
;Write a program to show the opcode values of the program as it's running.
;Todo:
;[X] Get the EIP value in a global variable as the first instruction of the code.
;[X] Jump to the bottom of the code by a label.
;[X] Store the EIP value in another variable to use as an offset from the first.
;[X] Jump back to the opcode after the first jump at the start of the program.
;[X] Have the program copy all instructions between the two pointers in memory into a buffer.
;[X] Convert each of the buffer's bytes into its ascii value (0-255).
;[X] Display each of the converted values of the buffer.
;[X] Terminate the process.

format elf executable
use32;
entry start;

segment readable writeable executable

sz_line_break db 10 ;;\n

ul_code_start dd 0
ul_code_end dd 0
ul_code_size dd 0

uc_buffer rb 4096
uc_ascii_number rb 12

;;4294967296
;;1234567890 (10 bytes + 1 for null termination; +1 again if signed and negative)

start:

;;Store the location in memory of the first offset.
 call get_eip
 mov [ul_code_start], eax
 jmp end_of_the_code


functional_start_of_code:


;;working code for converting a number to ascii and displaying it.
 mov eax, [ul_code_size]
;; mov eax, [ul_code_start]
;; mov eax, [ul_code_end]

 call number_to_ascii
 mov eax, 4
 mov ebx, 1
 int 0x80
;; jmp exit_the_process




;;read from ul_code_start to ul_code_end (ul_code_size bytes) into the buffer, uc_buffer.
 mov esi, ul_code_start
 mov edi, uc_buffer
 mov ecx, [ul_code_size]
 mov edx, ecx ;;store this for later.

 rep movsb ;;continually copy from the executable code into the buffer, until ecx reaches zero.

;;convert and display every byte of the buffer until we've reached ul_code_size bytes.

 mov ecx, edx ;;ul_code_size (stored for quick access).
 mov edx, uc_buffer ;;point to the start of the buffer.

convert_and_display_character:

  xor eax, eax
  mov al, byte [edx] ;;extract a byte from the opcode buffer to examine.

mov esi, edx  ;;push edx ;;preserve the pointer to the current position in the array.
mov edi, ecx  ;;push ecx ;;preserve the counter variable.

  ;;This will store a converted ascii representation in the back end of a buffer and give us a pointer to the start of it's string in ECX.
  call number_to_ascii ;;this will convert the number in eax to ascii and display it. (eax must be preserved within this function)
  mov eax, 4 ;;sys_write
  ;;mov ecx, message to write... (done already with call number_to_ascii)
  mov ebx, 1  ;;file descriptor (STDOUT)
  ;;mov edx, ?? ;;length of the string. (done already with call number_to_ascii)
  int 0x80

  mov eax, 4
  mov ebx, 1
  mov ecx, sz_line_break ;;\n
  mov edx, 1 ;;1 byte to write
  int 0x80

mov edx, esi  ;;pop ecx ;;restore the current value of the counter variable.
mov ecx, edi  ;;pop edx ;;restore the pointer to the current position within the array.

  add edx, 1     ;;go to the next byte of the opcode buffer.
  loop convert_and_display_character ;;if ( --ecx ) goto convert_and_display_characters;

exit_the_process:
;;exit the process
 mov ebx, 0 ;;exit code for the process (0 = no error)
 mov eax, 1 ;;sys_exit
 int 0x80   ;;call the kernel




;;functions:
;;in:  eax = the value to display as ascii
;;out: ecx = the buffer to hold the new ascii string
;;out: edx = the length of the string.
number_to_ascii:
 ;;mov esi, eax ;;store the value in eax for later.

 mov ecx, uc_ascii_number
 add ecx, 12 ;;the max length of a 4 byte unsigned value is 11 characters.
 mov [ecx], byte 0
 sub ecx, 1
 mov ebx, 10 ;;we'll continually divide by the base (10).

 cmp eax, 10
 jl finish_conversion

continue_converting_and_displaying:
 xor edx, edx ;;set edx to zero for dividing, since we divide edx:eax by some immediate value or register or value in memory.
 div ebx

 add dl, '0'   ;;add the offset between the real zero and ascii zero to the remainder.
 mov [ecx], dl ;;store the remainder in the next available position in the ascii string buffer.
 sub ecx, 1    ;;move backwards so that the number isn't in reverse.

 cmp eax, 10
 jge continue_converting_and_displaying

;;We skip here if the number was < 10 with which to begin.
finish_conversion:

 add al, '0'   ;;add the offset of ascii '0' to the real remaining value in the number that we're converting.
 mov [ecx], al ;;store the number in the current position.

 ;;Store the offset of the start of the number and the begining of the buffer in EAX.
 mov eax, ecx
 sub eax, uc_ascii_number

 ;;Store the total length of the string (buffer size - string start position offset from start of the buffer) in EDX.
 mov edx, 12 ;;(size of the buffer)
 sub edx, eax

 ;;mov eax, esi  ;;restore the value of eax.

ret


get_eip:
 mov eax, [esp]
ret






end_of_the_code:

;;Store the location in memory of the last few offsets.

 call get_eip ;;doesn't work here, for some reason.
 ;;mov eax, $
 ;;add eax, [ul_code_start]
 mov [ul_code_end], eax

;;Calculate the size of the code by the offset of the end of the code from the start of the code.
 sub eax, [ul_code_start]
 mov [ul_code_size], eax

 jmp functional_start_of_code
