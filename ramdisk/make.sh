cd ramdisk
find . | cpio -o -H newc -O ../initramfs.cpio
