# Radare2 Cheat sheet
aa / aa / aaa / aaaa    analyse the binary\
fs       show the different flags\
fs [category name]     select a flag category\
f      shows the flags from the selected category\
s [offset / function / symbol]     move to that place in the memory\
pdf     print the function were're placed at\

##### VISUAL MODE
VV        get in the visual mode\
hjkl      move the different blocks\

##### DEBUG MODE\
db [offset / function / symbol]   add a breakpoint\
db    shows all set breakpoints\
db - [offset]     remove the beakpoint\
dc     run the binary until a breakpoint or the end\
V!     enter in the fancy mode\
:      to open the command line input\
dr [register]=[value]   put the value in the register\

##### WRITE MODE\
wx [instruction]   writes the instruction to the line we are\
