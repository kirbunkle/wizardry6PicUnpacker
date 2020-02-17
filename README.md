# wizardry6PicUnpacker
Simple utility for compressing/decompressing wizardy 6 .PIC format files

*** 
About the program
***

This program will decrompress the .PIC files from a Wizardry 6 installation.

Honestly, it is not very useful right now. I was going to add functionality to convert to bitmap, but decided to cut it short.

You can still unpack the .PIC files into the raw data (which is not very useful, but this is how the game reads it), or into
a "pseudo-bitmap" format with the pixel data organized in each half byte. This may be useful if you wanted to change the pixel data,
then re-compress your modification back into a .PIC file, as I have done.

***
About the edited .PIC files
***

My main goal for making this program was to censor some of the nudity in the game.

I have already "modestized" 3 .PIC files:

MON30, Amazulu

MON38, Siren

MON45, Rebecca

You can see samples of what these would look like in the EditedPics/SamplesOfEditedPics directory

Installing these files is very simple, download them and move them to your Wizardry 6 installation directory.
For example, I downloaded my game from GOG.com, and my path is C:\Program Files (x86)\GOG Galaxy\Games\Wizardry 6\
These files are named the same as the files they will replace, so make a back-up if you don't want them overwritten.

Enjoy.
