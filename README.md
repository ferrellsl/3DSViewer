# 3DSViewer
3D Studio Max 3DS model viewer

This code was originally written by Mark M. Lanning in the late 90's to view 3DS files.  It originally had dependencies on GLAUX, libjpeg, GLUT32 and GLU.  I have removed most of these dependencies because they have long since been deprecated.  The only remaining dependencies are now freeglut and libjpeg.  It was also a 32-bit app and I have compiled it for 64-bit versions of Windows.  This code is highly portable and should compile on Linux and MacOS without issues provided the programmer links against MacOS or Linux versions of freeglut and libjpeg.  This is a command line app which takes a single argument.  To view a 3DS file, type:  3dsviewer followed by the name of the 3DS model that you want to view.  I.e. 3dsviewer my_file.3ds

Texture files will be loaded automatically provided they have the same base name as the model.  Both BMP and JPEG files are suppoted.  I have included skull.3ds and skull.jpg as examples.  The viewer will search for a JPEG or BMP texture file automatically upon loading a model, so there's no need to specify the texture file name.  Alternatively, just drag-and-drop your 3DS file onto the 3DSViewer.exe file.

Pressing the space bar will toggle the animation.  Pressing the V or v key will toggle mesh or textured mode.  Pressing ESC or Q will quit the program.

There is a Visual Studio 2019 Solution file included.  For coders too lazy to build libjpeg and freeglut, I have included those dependecies as zip files in the Dependencies folder of this repo.

![image](https://user-images.githubusercontent.com/5926287/153332936-1a1984c5-be7d-4f55-9a1a-179d2a25e5f8.png)

![image](https://user-images.githubusercontent.com/5926287/153333040-405954e5-2638-4f49-b0e3-61c370dea204.png)

![image](https://user-images.githubusercontent.com/5926287/153333074-7f95de61-16ec-43ae-a0e2-1f16929a66d9.png)

![image](https://user-images.githubusercontent.com/5926287/153333123-fee12312-f090-4536-ac02-5958c4edf147.png)

![image](https://user-images.githubusercontent.com/5926287/153333167-c7cc6a19-8522-4214-b027-68bed5c26fd1.png)


