#!/bin/bash

st-flash erase
st-flash write build/stm32-sdio-fat32.bin 0x8000000