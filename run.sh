#!/bin/bash
qemu-system-x86_64 -curses -show-cursor  -hda Image/x64BareBonesImage.qcow2 -m 512 -usb -device usb-mouse -device usb-kbd -show-cursor
