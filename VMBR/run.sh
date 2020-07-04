sudo qemu-system-x86_64 -enable-kvm \
    -cpu host -smp 2,sockets=1,cores=2 -m 2G \
    -vga cirrus -nographic -vnc :0 \
    -hda /var/lib/libvirt/images/Ubuntu-16.04.qcow2 \
    -bios ./seabios-network/out/bios.bin \
    -chardev file,id=seabios,path=/tmp/bios.log \
    -device isa-debugcon,iobase=0x402,chardev=seabios \
    -fsdev local,security_model=passthrough,id=fsdev0,path=./ \
    -device virtio-9p-pci,id=fs0,fsdev=fsdev0,mount_tag=hostshare -net nic,model=pcnet
