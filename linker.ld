ENTRY(start)

SECTIONS
{
	. = 2M;

	/* Executable code */
	.text BLOCK(4K) : ALIGN(4K)
	{
	/* IMPORTANT multiboot must be in first 8kb and works only in text somehow 0_o */
	*(.multiboot)
	*(.text)
	}

	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
	*(.rodata*)
	}

	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
	*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
	*(COMMON)
	*(.bss)
	}

    . = ALIGN(4096);
    kernel_end = .;
    heap_start = . + 0x10000;
}
