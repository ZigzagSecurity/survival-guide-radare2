# Radare2 Cheat sheet
## To be continued
##### INFORMATION GATHERING
`rabin2 -I [filename]` Show informations about the file (arch, format, securities, ...)\
`aa / aa / aaa / aaaa`  Analyse the binary\

##### NAVIGATION
`fs`       Show the different flags\
`fs [category name]`     Select a flag category\
`f`      Shows the flags from the selected category\
`s [offset / function / symbol]`     Move to that place in the memory\
`pdf`     Print the function were're placed at

##### VISUAL MODE
`VV`        Get in the visual mode\
`hjkl`      Move the different blocks

##### DEBUG MODE
`db [offset / function / symbol]`  Add a breakpoint\
`db`  Shows all set breakpoints\
`db - [offset] `    Remove the beakpoint\
`dc`    Run the binary until a breakpoint or the end\
`V!`    Enter in the fancy mode\
`:`     To open the command line input\
`dr [register]=[value]`   Put the value in the register

##### PRINTING INFORMATIONS
`pxw [length] @[register/offset]` Print the memory content (in hex and ascii) in word format

##### WRITE MODE
`wx [instruction]`   Writes the instruction to the line we are

##### FUZZING FOR BUFFER OVERFLOW
`ragg2 -P [string length] -r` Create a string with the De Bruijn pattern\
`wopO [SEGFAULT's offset]` Show the index of the De Bruijn pattern\
