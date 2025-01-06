make

if [[ "$1" == "debug" ]]; then
    qemu-system-i386 -drive file=build/boot.disk,format=raw,index=0,media=disk -nic none -s -S &
    gdb -x gdb/gdb-real-mode
else
    qemu-system-x86_64 -drive file=build/boot.disk,format=raw,index=0,media=disk -nic none
fi