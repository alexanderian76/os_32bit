This is 32 bit OS. To compile run 'make usb' command (you should run it twice to calculate OS.bin size, maybe will fix it later).
</br></br>Now realised:</br>
</br>loading to protected mode from usb
</br>keyboard interrupts
</br>paging (4GB RAM allocated)
</br>PCI devices scanning
</br>virual filesystem(in RAM only for now)
</br>Writing and readin from booted disk (hdd, usb, floppy)
</br></br>
OS working in text mode (no drivers for video, no vesa)
