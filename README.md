steg - a small steganography program
====================================

A port of a Python application I toyed around with. This tool will allow you to store any kind of data in a PNG image, virtually undetectable to the human eye. This is because `steg` causes extremely small changes in the RGB values of the pixels in the image.

How it works
------------
Imagine you have a single channel image (for simplicity, the tool uses r, g and
b) that you want to store some information in with values 0-255:

``
123  12  54 121
 70 122   0   7
``

Let's say you want to store the ASCII value for 'T' (0b01010100), the way steg
does this is by setting the last bit of the pixel values to the data you want to
store. Which is the same as nudging the values to be either odd or even.

So, after running steg we would get:

``
122  11  54 121
 70 121   0   6
``

The changes made are very subtle, and notice that in this case half of the
values does not even change at all.

