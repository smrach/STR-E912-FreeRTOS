/**
 * <b>File:</b> STR91x-ROM.ld
 *
 * <b>Project:</b> STR91x Eclipse demo
 *
 * <b>Description:</b> Linker script to load the application in flash.
 *
 * <b>Created:</b> 27/08/2008
 *
 * <dl>
 * <dt><b>Author</b>:</dt>
 * <dd>Stefano Oliveri</dd>
 * <dt><b>E-mail:</b></dt>
 * <dd>software@stf12.net</dd>
 * </dl>
 */
 
/* _HEAPSIZE = 10; */


MEMORY 
{
	flash (rx)	: ORIGIN = 0x00000000, LENGTH = 512K
	ram (rw) 	: ORIGIN = 0x04000000, LENGTH = 96K
}


SECTIONS 
{
	/* first section is .text which is used for code */
	
	.text :
	{
		KEEP(*(.vectors))
		KEEP(*(.init))
		*(.text .text.*)
		*(.gnu.linkonce.t.*)
		*(.glue_7t .glue_7)
		KEEP(*(.fini))
		*(.gcc_except_table)
	} >flash =0
	. = ALIGN(4);


	/* .rodata section which is used for read-only data (constants) */

	.rodata :
	{
		*(.rodata .rodata.*)
		*(.gnu.linkonce.r.*)
	} >flash
	. = ALIGN(4);

	_etext = .;
	PROVIDE (etext = .);

	/* .data section which is used for initialized data */

	.data : AT (_etext)
	{
		__data_start = .;
		*(.data .data.*)
		*(.gnu.linkonce.d.*)
		. = ALIGN(4);
		*(.fastrun .fastrun.*)
	} >ram
	. = ALIGN(4);
	
	_edata = .;
	PROVIDE (edata = .);

	/* .bss section which is used for uninitialized data */

	.bss :
	{
		__bss_start = .;
		__bss_start__ = .;
		*(.bss .bss.*)
		*(.gnu.linkonce.b.*)
		*(COMMON)
		. = ALIGN(4);
	} >ram
	. = ALIGN(4);
	__bss_end__ = .;
	
	_end = .;
	PROVIDE(end = .);
	
/*
	.heap (NOLOAD) :
	{
		__heap_start__ = .;
		_heap_start = .;
		*(.heap)
		. = MAX(__heap_start__ + _HEAPSIZE , .);
	} >ram
	__heap_end__ 	= __heap_start__ + SIZEOF(.heap);
	_heap_end 		= __heap_start__ + SIZEOF(.heap);

*/

}
	. = ALIGN(32 / 8);
	_end = .;
	_bss_end__ = . ; __bss_end__ = . ; __end__ = . ;
	PROVIDE (end = .);


