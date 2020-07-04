# Supplementary-Materials-For-S2BR-Project

This repo omits [coreboot](https://github.com/coreboot/coreboot). Please follow [instructions](https://www.coreboot.org/SeaBIOS) to build corresponding seabios as customized BIOS.

#### S2BR

- SGX Mailman

```sh
cd S2BR/enclave-mailman && make
```

- SMM Leech

```sh
cd S2BR/seabios-leech && make
```

#### SMBR

```sh
cd SMBR/seabios-network
make
```

#### VMBR

```sh
cd VMBR/qemu-vmbr && make clean && make
sh ./run.sh
```
