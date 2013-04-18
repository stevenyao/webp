Qt imageformat plugin for Webp
===============================

It's forked from cor3ntin/qt_webp.

the plugin just supports read-only, writing is on-going.

Building:

1. copy all the source code files to %QTDIR%\src\plugins\imageformats\webp
2. qmake -tp vc
3. build the qwebp.vcproj

Using:

make sure that the qwebp4.dll is in the Qt plugin folder.
any image files with .webp ext will work fine with QPixmap QImage

for example:

QPixmap p(":/images/demo.webp");

Steven Yao
