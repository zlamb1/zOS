./build.sh

if [[ "$1" == "debug" ]]; then
    qemu-system-i386 \
        -drive id=disk,file=build/boot.bin,format=raw,index=0,media=disk,if=none \
        -device ide-hd,drive=disk,bus=ide.0 \
        -nic none -m 256M -s -S &
    gdb -x gdb/gdb-cmd
else
    qemu-system-i386 \
        -drive id=disk,file=build/boot.bin,format=raw,index=0,media=disk,if=none \
        -device ide-hd,drive=disk,bus=ide.0 \
        -nic none -m 256M
fi