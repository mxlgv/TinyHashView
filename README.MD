# TinyHash-W 

![Window screenshot](screen.png)

### Description:

**TinyHash-W is a small window utility built for KolibriOS.**

- Support for three checksum algorithms: MD5, SHA1, SHA256.
- Shows checksums of the file.

**Warning! The program is in active development. It has limited functionality and may not work correctly!**

### Compilation:

**To compile under Kolibri OS, use TCC or run the script: "build_kex.sh" to auto-build.**

    tcc algorithms/md5.c algorithms/sha1.c algorithms/sha256.c thashw.c -o thashw -lck
        
### Usage example:

**Open the file with "thashw" or run in "SHELL":**
    
    thashw filename

### My plans

- Add copy to clipboard. 
- Add an input line to compare checksums. 
- Fix the window size.
- Increase font size.




   