This is 32 bit OS. To compile run 'make usb' command (you should run it twice to calculate OS.bin size, maybe will fix it later).\n
Now realised \n
loading to protected mode from usb\n
keyboard interrupts\n
paging (4GB RAM allocated)\n
PCI devices scanning\n
virual filesystem(in RAM only for now)\n
Writing and readin from booted disk (hdd, usb, floppy)\n\n

OS working in text mode (no drivers for video, no vesa)
