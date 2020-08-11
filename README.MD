# TinyHash-W 

![Window screenshot](screen.png)

### Description:

**TinyHash-W is a small window utility built for KolibriOS.**

- Support for three checksum algorithms: MD5, SHA1, SHA256.
- Shows checksums of the file.

<span style="color:red">**Warning! The program is under active development and may malfunction or malfunction!**</span>.

### Compilation:

**To compile under Kolibri OS, use TCC or run the script: "build_kex.sh" to auto-build.**

    tcc algorithms/md5.c algorithms/sha1.c algorithms/sha256.c thashw.c -o thashw -lck
        
### Usage example:

*Open the file with "thashw" or run in "SHELL": 
    
    thashw filename
    
   
