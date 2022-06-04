# signfinder - Windows process memory crawler

Finds a specified byte array in a given process or file.   
This is useful to find functions in a process/executable using PIC.

## Usage:
```
signfinder - Find specific bytes in a remote process or files
signfinder <options> -- <bytes..>
Options <options>:
        --help,    -h           - show this help screen
        --verbose, -v           - print more info
        --pid,  -p <pid>        - process to search in
        --file, -f <file>       - file to search in
Search bytes <bytes..> in hexadecimal
        i.E.: 0xf4782 0x6723, 90 f7 e5 e6 00 e8
```

## Build
signfinder was built and created in mingw, therefore only this built environment is offically supported.
