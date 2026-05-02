# Vibe-coded-sloppy-proof-of-concept-for-CLAP-plugin-with-C-Assembly
This is a proof of concept I decided to try and see if it's even possible. It is not good, I do not know how it works. But is it minimal? It seems so.
This plugin should multiply your volume by 0.5, I think. Use a limiter to save your ears in case it blows up. Tested in Reaper

# DISCLAIMER!
The author is NOT RESPONCIBLE for your pc BLOWING THE FUCK UP if the slop GPT did anything wrong. USE AT YOUR OWN RISK.
But for me it worked and I am still alive, soo... I guess the show will go on.

# Compilation process
1\. You'll likely need to
```console
sudo apt install nasm gcc git
```

2\. This is the compilation process
```console
nasm -f elf64 -o dsp.o dsp.s
```
```console
gcc -shared -fPIC -o asm_plugin.clap main.c dsp.o -I.
```

Tested and compiled on Linux Mint, x64
