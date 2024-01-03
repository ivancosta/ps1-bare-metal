/*
 * ps1-bare-metal - (C) 2023 spicyjpeg
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
 
/**
 * Print the string to the SIOCON0 (stdout) using the kernel/BIOS A-function  
 * A0 bios function 3F (printf) 
 * Output is available through a separate window in mostly all PSX emulators 
 * 
 * @author Ivan Francisco Coutinho Costa
 * @param msg 
 */
void printToTTY(char *msg) { // arguments are passed in a0..a3, so msg is in a0

      __asm__ volatile(
            ".set noreorder\n"
            "addiu $29, $29, -4\n"   
            "sw	$31,0($29)\n"       // put return address ra=$31 in the stack 

            //"la $4, %0\n"         // $4/a0 already contains string address, put by caller 
            "addiu $10, $0, 0xA0\n" // bios A0
            "addiu $9, $0, 0x3F\n"  // function 3f = printf
          
            "addiu $29, $29, -16\n"  // stack pointer, I'm not certain if space needs to be allocated in the stack before calling the BIOS
            "jalr $10\n"            // Call the BIOS function
            "nop\n"                 // branch delay
            "addiu $29, $29, 16\n" // stack pointer

            "lw	$31, 0($29)\n"      
            "addiu $29, $29, 4\n"   
            ".set reorder\n"
            : // no outputs
            : // input string is already in register a0 when the function is called
            : "$9", "$10"   // $9=t1 $10=t2
        );

}


/**
 * Print the string using the A0 bios function 3F (printf) to the SIOCON0
 * Output is visible in PSX emulators 
 * 
 * @author Ivan Francisco Coutinho Costa
 */

int main(int argc, const char **argv) {

    char *str = "Print by calling a function\n";
    for (int i=0; i<10; i++) {
        printToTTY(str);
    }
  
    for (int j=0; j<10; j++) {

        const char *string_to_print = "Printing without calling a separate function\n";

        __asm__ volatile(
            ".set noreorder\n"
            "addiu $29, $29, -4\n"  // stack pointer
            "sw	$2,0($29)\n"        // put  v0 register in the stack v0=$2 (bios function mess with it and it is used in the for loop )
            "la $4, %0\n"           // $4/a0 
            "addiu $10, $0, 0xA0\n" // bios  
            "addiu $9, $0, 0x3F\n"  // function 3f = printf
          
            "addiu $29, $29, -16\n" // stack pointer, I'm not certain if space needs to be allocated in the stack before calling the BIOS
            "jalr $10\n"            // Call the BIOS function
            "nop\n"                 // branch delay
            "addiu $29, $29, 16\n"  // stack pointer
          
            "lw	$2, 0($29)\n"       // recover v0=$2 (used by the C compiler in the outer for..loop) from stack
            "addiu $29, $29, 4\n"   // stack pointer
            ".set reorder"
            : // no outputs
            : "i"(string_to_print)  // input i=imediate
            : "$4", "$9", "$10"     // $4=a0 $9=t1 $10=t2
        );
    }

    loop:
        goto loop;
	
	// We're not actually going to return. Unless a loader was used to launch
	// the program, returning from main() would crash the console as there would
	// be nothing to return to.
	return 0;
}
