#!/bin/bash

st-flash erase
st-flash write build/SDIO_FAT32.bin 0x8000000