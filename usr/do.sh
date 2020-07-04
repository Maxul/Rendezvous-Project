sudo mount -t 9p -o trans=virtio,version=9p2000.L hostshare ./host_files
cd host_files/kern
sudo sh ./doit.sh
cd ../apps/ssh
./app
