gcc -fPIC -c ./elevate.c
python ./shellme.py -n elevate.o -a elf64
