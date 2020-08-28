# TinyHashView

![Window screenshot](screen.png)

### Description:

**TinyHashView is a small window utility built for KolibriOS.**

- Support for three checksum algorithms: MD5, SHA1, SHA256.
- Shows checksums of the file.
- It is possible to compare checksums.(NEW)

### Compilation:

**This program is almost entirely created in KolibriOS (I am a supporter of software development using the tools of the OS itself). So use TCC to compile or just run the "build_kex.sh" script to build it automatically.**

    tcc algorithms/md5.c algorithms/sha1.c algorithms/sha256.c thashview.c -o thashview -lck
        
### Usage example:

**Open the file with "thashview" in EOLITE or run it in "SHELL**
    
    thashview filename

### Whats is done

- Changed and corrected interface.
- Added copy to clipboard function.
- Added the function of pasting from the clipboard.
- Implemented input field(Not full functionality).
- Added "Compare" button to compare checksums.
- Display notifications when copying, pasting, etc.

### My plans
 
- Expand the functionality of the input field.
- Add a dialog box to open a file. 
- Fix still undetected bugs. 
- Integration with EOLITE (questionable).

### I thank
- Maxim Kuzmitski (maxcodehack) for the drawn badge, help with design and tests.
- Bopann Kostoev (Boppan) for finding bugs.
- Rustem Gimadutdinov (rgmad) for testing.





   
