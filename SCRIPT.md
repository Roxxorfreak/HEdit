# HEdit 4 Script Documentation

## Introduction

HEdit supports scripts that can be used to visualize data in a certain way.
These scripts are (by default) stored in the user's home directory in a sub-folder called ".hedit-scripts".

That is
* **Windows**: %userprofile%\\.hedit-scripts
* **Linux/MacOS X**: ~/.hedit-scripts

The folder can be changed via HEdit config file (located in the user's home directory).

A script file has the extension ".hs". All files with this extension are loaded by HEdit.

## General script structure

Scripts are of the following structure:

```
main("Script description")
{
    Instruction1;
    .
    .
    .
    return;
}
```

The script description must not exceed 127 characters.
Every instruction line must end with a semi-colon `;`

## Instruction Set

This section documents the script instructions. Please note: All offsets are relative to the current file position.

### Control flow

#### return
Ends the script execution
###### Syntax
    return;
###### Parameters
none

#### loop/until
Executes the code in the loop body until the variable reaches the specified value.
###### Syntax
    loop;
    ...
    until(variable, value);
###### Parameters
Name | Type | Description
 - | - | -
*variable* | variable | The name of the variable to check
*value* | integer | The value to reach

### Output

#### gotoxy
Moves the cursor to the specified position on the virtual screen.
###### Syntax
    gotoxy(x, y);
###### Parameters
Name | Type | Description
 - | - | -
*x* | integer | Cursor column, must be between 1 and 200
*y* | integer | Cursor row, must be between 1 and 90

#### print
Outputs the specified text at the current cursor position.
###### Syntax
    print(text);
###### Parameters
Name | Type | Description
 - | - | -
*text* | string | Any text up to 78 characters

#### cprintb
Outputs the specified text at the current position 
if the value at "offset" equals to "value", compared as byte
###### Syntax
    cprintb(text, offset, value);
###### Parameters
Name | Type | Description
 - | - | -
*text* | string | Any text up to 78 characters
*offset* | integer | The offset of the value to compare (may be a variable)
*value* | integer | The value to compare with (may be a variable)

#### cprintw
Outputs the specified text at the current position
if the value at "offset" equals to "value", compared as word
###### Syntax
    cprintw(text, offset, value);
###### Parameters
Name | Type | Description
 - | - | -
*text* | string | Any text up to 78 characters
*offset* | integer | The offset of the value to compare (may be a variable)
*value* | integer | The value to compare with (may be a variable)

#### cprintd
Outputs the specified text at the current position
if the value at "offset" equals to "value", compared as double word
###### Syntax
    cprintd(text, offset, value);
###### Parameters
Name | Type | Description
 - | - | -
*text* | string | Any text up to 78 characters
*offset* | integer | The offset of the value to compare (may be a variable)
*value* | integer | The value to compare with (may be a variable)

#### printub
Outputs the value at the specified offset as unsigned byte,
with the Prefix left of it and the Suffix right of it at the current position.
###### Syntax
    printub(prefix, suffix, offset);
###### Parameters
Name | Type | Description
 - | - | -
prefix | string | Any text up to 30 characters, printed left of the value
suffix | string | Any text up to 30 characters, printed right of the value
offset | integer | The offset of the value to print as unsigned byte (may be a variable)

#### printuw
Outputs the value at the specified offset as unsigned word,
with the prefix left of it and the suffix right of it at the current position.
###### Syntax
    printuw(prefix, suffix, offset);
###### Parameters
Name | Type | Description
 - | - | -
prefix | string | Any text up to 30 characters, printed left of the value
suffix | string | Any text up to 30 characters, printed right of the value
offset | integer | The offset of the value to print as unsigned word (may be a variable)

#### printud
Outputs the value at the specified offset as unsigned double word,
with the "prefix" left of it and the "suffix" right of it at the current cursor position.
###### Syntax
    printud(prefix, suffix, offset);
###### Parameters
Name | Type | Description
 - | - | -
prefix | string | Any text up to 30 characters, printed left of the value.
suffix | string | Any text up to 30 characters, printed right of the value.
offset | integer | The offset of the value to print as unsigned double word (may be a variable)

#### printhb
Outputs the value at the specified offset as hexdecimal byte,
with the "prefix" left of it and the "suffix" right of it at the current cursor position.
###### Syntax
    printhb(prefix, suffix, offset);
###### Parameters
Name | Type | Description
 - | - | -
prefix | string | Any text up to 30 characters, printed left of the value.
suffix | string | Any text up to 30 characters, printed right of the value.
offset | integer | The offset of the value to print as hexdecimal byte (May be a variable)

#### printhw
Outputs the value at the specified offset as hexdecimal word,
with the "prefix" left of it and the "suffix" right of it at the current cursor position.
###### Syntax
    printhw(prefix, suffix, offset);
###### Parameters
Name | Type | Description
 - | - | -
prefix | string | Any text up to 30 characters, printed left of the value.
suffix | string | Any text up to 30 characters, printed right of the value.
offset | integer | The offset of the value to print as hexdecimal word (May be a variable)

#### printhd
Outputs the value at the specified offset as hexdecimal double word,
with the "prefix" left of it and the "suffix" right of it at the current cursor position.
###### Syntax
    printhd(prefix, suffix, offset);
###### Parameters
Name | Type | Description
 - | - | -
prefix | string | Any text up to 30 characters, printed left of the value.
suffix | string | Any text up to 30 characters, printed right of the value.
offset | integer | The offset of the value to print as hexdecimal double word (May be a variable)

#### printsb
Outputs the value at the specified offset as signed byte,
with the "prefix" left of it and the "suffix" right of it at the current cursor position.
###### Syntax
    printsb(prefix, suffix, offset);
###### Parameters
Name | Type | Description
 - | - | -
prefix | string | Any text up to 30 characters, printed left of the value.
suffix | string | Any text up to 30 characters, printed right of the value.
offset | integer | The offset of the value to print as signed byte (May be a variable)

#### printsw
Outputs the value at the specified offset as signed word,
with the "prefix" left of it and the "suffix" right of it at the current cursor position.
###### Syntax
    printsw(prefix, suffix, offset);
###### Parameters
Name | Type | Description
 - | - | -
prefix | string | Any text up to 30 characters, printed left of the value.
suffix | string | Any text up to 30 characters, printed right of the value.
offset | integer | The offset of the value to print as signed word (May be a variable)

#### printsd
Outputs the value at the specified offset as signed double word,
with the "prefix" left of it and the "suffix" right of it at the current cursor position.
###### Syntax
    printsd(prefix, suffix, offset);
###### Parameters
Name | Type | Description
 - | - | -
Prefix | string | Any text up to 30 characters, printed left of the value
Suffix | string | Any text up to 30 characters, printed right of the value
offset | integer | The offset of the value to print as signed double word (May be a variable)

#### printfd
Outputs the value at the specified offset as floating point number,
with the "prefix" left of it and the "suffix" right of it at the current cursor position.
###### Syntax
    printfd(prefix, suffix, offset);
###### Parameters
Name | Type | Description
 - | - | -
prefix | string | Any text up to 30 characters, printed left of the value
suffix | string | Any text up to 30 characters, printed right of the value
offset | integer | The offset of the value to print as floating point number (may be a variable)

#### printds
Outputs "length" bytes from "offset" as character string at the current cursor position, stopping
if a binary zero is reached.
###### Syntax
    printds(offset, length);
###### Parameters
Name | Type | Description
 - | - | -
offset | integer | The offset of the string to print (may be a variable)
length | integer | The length of the string to print (may be a variable)

#### prinths
Outputs "length" bytes from "offset" as hexdecimal values (e.g. 0F043B).
###### Syntax
    prinths(offset, length);
###### Parameters
Name | Type | Description
 - | - | -
offset | integer | The offset of the string to print (may be a variable)
length | integer | The length of the string to print (may be a variable)

#### printxor
Outputs the XOR'ed checksum of "length" bytes from "offset" as hexdecimal byte value.
###### Syntax
    printxor(offset, length);
###### Parameters
Name | Type | Description
 - | - | -
*offset* | integer | The offset of the string to calculate the checksum of (may be a variable)
*length* | integer | The length of the string to calculate the checksum of (may be a variable)

### Variables

#### declare
Declares a new variable for later use with loadb/loadw/loadd.
All variables are zero-initialized on declaration.
###### Syntax
    declare(variable, signed);
###### Parameters
Name | Type | Description
 - | - | -
*variable* | variable | The name of the variable to declare, must not start with a number.
*signed* | boolean | Specifies if the variable should be "signed", i.e. can be negative.

#### set
Loads a value into the variable.
###### Syntax
    set(variable, value);
###### Parameters
Name | Type | Description
 - | - | -
*variable* | variable | The name of the variable to load the value into
*value* | integer | The value to load (may be a variable)

#### add
Adds a value to the variable.
###### Syntax
    add(variable, value);
###### Parameters
Name | Type | Description
 - | - | -
*variable* | variable | The name of the variable to add the value to
*value* | integer | The value to add (may be a variable)

#### sub
Subtracts a value from the variable.
###### Syntax
    sub(variable, value);
###### Parameters
Name | Type | Description
 - | - | -
*variable* | variable | The name of the variable to subtract the value from
*value* | integer | The value to subtract (may be a variable)

#### incr
Increases the value of the variable by one
###### Syntax
    incr(variable);
###### Parameters
Name | Type | Description
 - | - | -
*variable* | variable | The name of the variable to increase

#### decr
Decreases the value of the variable by one.
###### Syntax
    decr(variable);
###### Parameters
Name | Type | Description
 - | - | -
*variable* | variable | The name of the variable to decrease

#### loadb
Loads the byte value at the specified offset into the variable.
"offset" can be another variable.
###### Syntax
    loadb(variable, offset);
###### Parameters
Name | Type | Description
 - | - | -
*variable* | variable | The name of the variable to load the byte value into
*offset* | integer | The offset of the value to load (may be a variable)

#### loadw
Loads the word value at the specified offset into the variable.
"offset" can be another variable.
###### Syntax
    loadw(variable, offset);
###### Parameters
Name | Type | Description
 - | - | -
*variable* | variable | The name of the variable to load the word value into
*offset* | integer | The offset of the value to load (may be a variable)

#### loadd
Loads the double word value at the specified offset into the variable.
"offset" can be another variable.
###### Syntax
    loadd(variable, offset);
###### Parameters
Name | Type | Description
 - | - | -
*variable* | variable | The name of the variable to load the double word value into
*offset* | integer | The offset of the value to load (may be a variable)
