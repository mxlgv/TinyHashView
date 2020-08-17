# TinyHashView

![Window screenshot](screen.png)

### Description:

**TinyHashView is a small window utility built for KolibriOS.**

- Support for three checksum algorithms: MD5, SHA1, SHA256.
- Shows checksums of the file.

**Warning! The program is in active development. It has limited functionality and may not work correctly!**

### Compilation:

**To compile under Kolibri OS, use TCC or run the script: "build_kex.sh" to auto-build.**

    tcc algorithms/md5.c algorithms/sha1.c algorithms/sha256.c thashview.c -o thashview -lck
        
### Usage example:

**Open the file with "thashview" or run in "SHELL":**
    
    thashview filename

### Whats is done

- The interface has been changed and corrected. 
- Added copy function.

### My plans
 
- Add an input line to compare checksums. 





   
