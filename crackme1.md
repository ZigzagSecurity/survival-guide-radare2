# Survival guide to radare2 // PART 1

For the first guide I will use a challenge from the picoCTF 2018. 
It's really simple and easy to understand.

[Here you can get the binary](//github.com/ZigzagSecurity/survival-guide-radare2/raw/master/PART1/be-quick-or-be-dead-1)


First of all you have to load the binary in r2, that is easily done with :\
`radare2 ./be-quick-or-be-dead-1`

A prerequisite is to know that you can manually search for any command using the **?** alone or with command to see what you can append.\
Now our work can begin.

## The analysis part 
The first thing to do is to analyse the binary. We have several options possible, some are good to inspect precisely the binary and others are more general and do the work pretty well.\
So here, we will use the command:\
`aaa`\
This command means "analyse all & autoname". 

(//github.com/ZigzagSecurity/survival-guide-radare2/blob/master/PART1/aaa.png)

As I said before, you can see all other analysis function by taping a?, aa? or any combination or letters.
After this r2 should know everything (at least enougth for us) about the binary. 

## The discovering part
This part consist of discovering what our binary is made of.\
Let's enter the command:\
`fs`

```console
[0x004005a0]> fs
0    6 * strings
1   43 * symbols
2   31 * sections
3   10 * segments
4    8 * relocs
5    8 * imports
6    1 * functions
[0x004005a0]> 
```
This will show what r2 analysed before like the strings, functions, imports, symbols, etc.
Then we can select one of these categories and print what's inside with the commands :
```
fs [category name]
f 
```

We can see in the strings category, all the strings that are printed when starting our binary.\
In the **sym** category we can find the differents functions that are called. 

The most interesting at the moment, **the main** function.

## The reading code part
So now we want to print the code to actually see what is done in there. The easiest way (but not the only one)to do it is to go at that place in the memory.\
We can do it with the command **s [offset / function / symbol]**

`s main`

Note that our cursor, get moved to the **sym.main offset**.

Then we can print the function where we currently are thanks to the "disassemble function" command,
**pdf** (Print Disassemble Function)

At this point we have a little overview of the binary.\
Four functions are called before leaving the main.

### Let's explore the first one
First, we move at the begining and then print the code
```
s sym.header
pdf 
```

As we can read, this function print the string **be quick or be dead 1** and then loop to print the "=".\
To see it a little bit better we should enter the Visual Mode:\
`VV`
That shows us some block of code with the different jump and conditions.\
In this view you can navigate using the keys "hjkl" to respectively move to left, down, up and right.

To get back to the normal view, just hit **q** until getting there.

### The program routine
I will skip the detailed inspection of the others functions because it's not the goal of this tuto. I will let you explore them with the commands we have seen earlier.

To be brief, set_timer set a timer (thanks captain obvious) and quit the program when it reach his end.
The function get_key calculate the key and put it in an object called "key".
Finally, the print_flag function will decrypt the flag with the key and print it to the terminal.

## How to get the flag ?
At this step we can think of two easy way exploit that binary :
- either we skip the timer
- either we skip the calculating part

## Let's exploit it !

### let's skip the calculating
so first thing is to open radare in debug mode with:
```
radare2 -d ./binary
aaaa
db sym.calculate_key
dc
```
So here we open the binary in debug mode, then we analyse it to permit r2 to interact with.
Finally we put a breakpoint at the beginning of the calculating function and get the execution to it with the `dc` command.\
Now we have to start the debug work, so let's switch in the Fancy Mode, it will be much easier.\
`V!`

Here you see our different windows with all informations we need on our running binary.
You can switch from panels with tabs and move informations with arrow keys. It's also possible to close windows, open new ones, etc but it will be subject for a next tuto. :)

Back to our business, we can see where we are in the list of instructions with the rip cursor. We can execute instructions with s or S (it does not get in function calls). So let's try to move a little to verify that we were right about that calculating loop.

You can see as you hit s how the registers, stack and instructions change.
Now we want to get out of that loop, so we can just hope over it.
We can change the instruction pointer to go directly to the next line after the end of our loop. To write command while in visual mode, just hit ":" and then type your command.
To modify a register use the command "dr".
```
dr rip=[offset of the line to go to]
hit enter to get back to the debug visual
```
So normally our cursor just moved right after the loop. Now we still need to put the value we were supposed to calculate in **eax**. Again we use **dr**.
```
dr eax=0xdfa8fc78  (That's the value that was compared with the local variable)
```
Now everything is prepared, we just have to run the binary and reach the end.\
The final **dc**.


### let's kill the timer
Here we are going to really kill the timer. We could have done the same trick to modify the instruction pointer but we would have to do it each time we run the binary. A definitive solution would be way better.
Let's start radare2 in writing mode :\
```
radare2 -w ./binary
```
Then we need to move to the main and remove the call to the timer's initialisation function. To do so, let's use :
```
s [offset of the calling instruction]
wx 9090909090
pdf
```

What we've done here is to move to the line we want to write to and then overwrite nope (0x90) instructions to it. Finally, we print again the function to verify the changes.
So normally you should see a nop instruction inplace of the timer call.
To get your flag, just quit radare2 and run the binary.
