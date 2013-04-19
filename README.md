Qt imageformat plugin for Webp
===============================

It's forked from cor3ntin/qt_webp.

Building:

1. copy all the source code files to %QTDIR%\src\plugins\imageformats\webp
2. go to libwebp folder, run "nmake Makefile.vc CFG=release-static"
2. back to up folder, run  "qmake -tp vc"
3. build the qwebp.vcproj

Using:

make sure that the qwebp4.dll is in the Qt plugin folder.
any image files with .webp ext will work fine with QPixmap QImage

for example:

* reading:
QPixmap p(":/images/demo.webp");

* writing:
QImage p(":/images/demo.png");

p.save("c:/temp/demo.webp", "webp", 100);
// quality >= 100 means lossless 

Known Issues:
there are some webp source code file in enc and dec folder with same file name but different contents.
I have no idea to spec obj file name for each source file, the obj files will overwrites each other, so 
I can't integrate webp code to qt project.  

Steven Yao
