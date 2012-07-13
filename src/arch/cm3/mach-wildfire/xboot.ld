OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")

STACK_SIZE 	= 0x2000;

MEMORY
{
	rom (rx)	: org = 0x08000000, len = 512k		/* 512 KB */
	ram (rwx)	: org = 0x20000000, len = 64k		/* 64 KB */
}

SECTIONS
{     
	.text :
	{
		. = ALIGN(8);
		PROVIDE (__text_start = .);
		KEEP(*(.isr_vector))
		*(.text)
		*(.text.*)
		*(.init.text)
    	*(.exit.text)
    	
    	. = ALIGN(8);	
		PROVIDE (__initcall_start = .);
		*(.initcall_0.text)
		*(.initcall_0s.text)
		*(.initcall_1.text)
		*(.initcall_1s.text)
		*(.initcall_2.text)
		*(.initcall_2s.text)
		*(.initcall_3.text)
		*(.initcall_3s.text)
		*(.initcall_4.text)
		*(.initcall_4s.text)
		*(.initcall_5.text)
		*(.initcall_5s.text)
		*(.initcall_6.text)
		*(.initcall_6s.text)
		*(.initcall_7.text)
		*(.initcall_7s.text)
		*(.initcall_8.text)
		*(.initcall_8s.text)
		*(.initcall_9.text)
		*(.initcall_9s.text)
		PROVIDE (__initcall_end = .);
		
		. = ALIGN(8);
		PROVIDE (__exitcall_start = .);
		*(.exitcall_9s.text)
		*(.exitcall_9.text)
		*(.exitcall_8s.text)
		*(.exitcall_8.text)
		*(.exitcall_7s.text)
		*(.exitcall_7.text)
		*(.exitcall_6s.text)
		*(.exitcall_6.text)
		*(.exitcall_5s.text)
		*(.exitcall_5.text)
		*(.exitcall_4s.text)
		*(.exitcall_4.text)
		*(.exitcall_3s.text)
		*(.exitcall_3.text)
		*(.exitcall_2s.text)
		*(.exitcall_2.text)
		*(.exitcall_1s.text)
		*(.exitcall_1.text)
		*(.exitcall_0s.text)
		*(.exitcall_0.text)
		PROVIDE (__exitcall_end = .);
		
		. = ALIGN(8);
		PROVIDE (__romdisk_start = .);
		*(.romdisk)
		PROVIDE (__romdisk_end = .);
		
		. = ALIGN(8);
		*(.rodata);
		*(.rodata.*);
		
		. = ALIGN(8);
		*(.glue_7);
		*(.glue_7t);

		. = ALIGN(8);
		PROVIDE (__text_end = .);
	} > rom
	
	.data_shadow ALIGN(8) :
	{
		PROVIDE (__data_shadow_start = .);
		PROVIDE (_sidata = .);
		PROVIDE (__data_shadow_end = (. + SIZEOF (.data)) );
	} > rom
	
	.data : AT ( ADDR (.data_shadow) )
	{
		PROVIDE (__data_start = .);	
		PROVIDE (_sdata = .);
		*(.data)
		. = ALIGN(8);
  		PROVIDE (__data_end = .);
  		PROVIDE (_edata = .);		
	} > ram
	
	.bss ALIGN(8) (NOLOAD) :
	{
		PROVIDE (__bss_start = .);
		PROVIDE (_sbss = .);
		*(.bss)
		*(.bss.*)
		*(.sbss)
		*(COMMON)
		PROVIDE (__bss_end = .);
		PROVIDE (_ebss = .);
		
		. = ALIGN(8);
		PROVIDE (__heap_start = .);
		*(.heap)
		. = ALIGN(8);
		PROVIDE (__heap_end = .);
		
		. = ALIGN(8);
		PROVIDE (__stack_start = .);
		. += STACK_SIZE;
		. = ALIGN(8);
		PROVIDE (__stack_end = .);
		PROVIDE (_estack = .);
	} > ram

	/*
	 * Stabs debugging sections.
	 */
	.stab 0 : { *(.stab) }
	.stabstr 0 : { *(.stabstr) }
	.stab.excl 0 : { *(.stab.excl) }
	.stab.exclstr 0 : { *(.stab.exclstr) }
	.stab.index 0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment 0 : { *(.comment) }
	.debug_abbrev 0 : { *(.debug_abbrev) }
	.debug_info 0 : { *(.debug_info) }
	.debug_line 0 : { *(.debug_line) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
	.debug_aranges 0 : { *(.debug_aranges) }
}
