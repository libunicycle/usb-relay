/* Linker script for the STM32F40xxG chip (1024K flash, 128K RAM). */

/* Define memory regions. */
MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
	ccm (rwx) : ORIGIN = 0x10000000, LENGTH = 64K
}

/* Include the common ld script. */
INCLUDE cortex-m-generic.ld
