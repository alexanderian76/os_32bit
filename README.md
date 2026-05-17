This is 32 bit OS. To compile run 'make usb' command (you should run it twice to calculate OS.bin size, maybe will fix it later).
Now realised 
loading to protected mode from usb
keyboard interrupts
paging (4GB RAM allocated)
PCI devices scanning
virual filesystem(in RAM only for now)
Writing and readin from booted disk (hdd, usb, floppy)

OS working in text mode (no drivers for video, no vesa)
