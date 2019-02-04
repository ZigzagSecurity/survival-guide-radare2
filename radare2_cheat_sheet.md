# Radare2 Cheat sheet
## To be continued
##### INFORMATION GATHERING
`rabin2 -I [filename]` Show informations about the file (arch, format, securities, ...)\
`aa / aa / aaa / aaaa`    analyse the binary\

##### NAVIGATION
`fs`       show the different flags\
`fs [category name]`     select a flag category\
`f`      shows the flags from the selected category\
`s [offset / function / symbol]`     move to that place in the memory\
`pdf`     print the function were're placed at

##### VISUAL MODE
`VV`        get in the visual mode\
`hjkl`      move the different blocks

##### DEBUG MODE
`db [offset / function / symbol]`  add a breakpoint\
`db`  shows all set breakpoints\
`db - [offset] `    remove the beakpoint\
`dc`    run the binary until a breakpoint or the end\
`V!`     enter in the fancy mode\
`:`      to open the command line input\
`dr [register]=[value]`   put the value in the register

##### PRINTING INFORMATIONS
`pxw [length] @[register/offset]` Print the memory content (in hex and ascii) in word format

##### WRITE MODE
`wx [instruction]`   writes the instruction to the line we are

##### FUZZING FOR BUFFER OVERFLOW
`ragg2 -P [string length] -r` Create a string with the De Bruijn pattern\
`wopO [SEGFAULT's offset]` Show the index of the De Bruijn pattern\
