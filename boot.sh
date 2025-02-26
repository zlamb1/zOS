make

# emulate IDE controller on PCI bus
EMULATION_ARGS="
    -device piix3-ide,id=ide \
    -drive id=disk,file=build/boot.disk,format=raw,index=0,media=disk,if=none \
    -device ide-hd,drive=disk,bus=ide.0 \
    -nic none -m 256M
"

if [[ "$1" == "debug" ]]; then
    qemu-system-i386 $EMULATION_ARGS -s -S &
    gdb-multiarch -x gdb/gdb-cmd
else
    qemu-system-x86_64 $EMULATION_ARGS
fi