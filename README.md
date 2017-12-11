
# Radar-Pi

This is a Python module developed for the Fall 2017 senior design showcase at Florida Atlantic University. It runs on a Raspberry Pi and interfaces the low-frequency output of an FMCW radar through a sound card. 


## Implementation Notes

### Sound Recording on Linux 

arecord -f S32_LE -c 2 -r 96000 /tmp/test.wav

To do custom development on the raspberry pi and have the header <alsa/asoundlib.h>, I had to do 
```sudo apt-get install libasound2-dev```

The audioinjector sound card outputs little-endian data, and only supports: 
- S16_LE
- S24_LE
- S32_LE

[Link](http://users.suse.com/~mana/alsa090_howto.html)

### SSH'ing into the PI

This is the command to do so: 
```ssh -R 52698:localhost:52698 pi@10.0.0.195```
[This](https://www.onenorth.com/blog/post/editing-files-on-raspberry-pi-with-local-sublime) tutorial is a good walkthrough for setup with rsub. 


### Writing Python Extensions

Had to run this to get the python 2.7 headers: 
```sudo apt-get install python-dev```

To get the python 3.4 headers: 
```sudo apt-get install python-dev```

If gcc can't find it, add a line to the makefile: 
```C_INC = -I/usr/include/python3.4```

Then use $(C_INC) wherever you have a file that includes Python things. 

This guy has a fantastic [blog post on the subject](http://dfm.io/posts/python-c-extensions/). 
But his blog post is slightly dated - NumPy integration and setup is a little different now in Python 3. 

