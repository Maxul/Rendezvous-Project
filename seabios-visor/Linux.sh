sudo \
    /home/lhl/research/qemu-sgx/x86_64-softmmu/qemu-system-x86_64 \
    -m 1G -sgx epc=20MB \
    -enable-kvm -cpu host -smp 1,sockets=1,cores=1 -machine kernel_irqchip=split \
    -hda /home/lhl/SVM/images/ubuntu16-server.qcow2 \
    -vga cirrus -nographic -vnc :1 \
    -fsdev local,security_model=passthrough,id=fsdev0,path=/home/lhl/ \
    -device virtio-9p-pci,id=fs0,fsdev=fsdev0,mount_tag=hostshare \
    -net nic,model=pcnet,macaddr='00:00:00:01:01:01' \
    -net tap,ifname=vport0,script=no,downscript=no
