/*********************************************************************************************\
|	
|	Mark M. Lanning
|	Starlight Software Co.
|	
|	TextureObjects.cpp
|		these class sets up what is needed to open a bmp texture and load it into OpenGL
|
\*********************************************************************************************/

#include <iostream>								// Include the standard input output stream
#include <stdarg.h>
#include "bmp.h"
#include <GL/freeglut.h>
//#include <stdio.h>			// Header File For Standard Input/Output
#include "TextureObjects.h"						// Include our header file for Fmod prototypes
using namespace std;
//Define a new structure to hold the image information
/*	
typedef struct _ImageDataRec {
	int sizeX, 
	int sizeY;
	unsigned char *data;
} ImageDataRec

//Old Structure information
typedef struct _AUX_RGBImageRec {
	GLint sizeX, 
	GLint sizeY;
	unsigned char *data;
} AUX_RGBImageRec;

struct tImageJPG
{
	int rowSpan;
	int sizeX;
	int sizeY;
	unsigned char *data;
};
*/	


///////////////////////////////// DECODE JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////	This decodes the jpeg and fills in the tImageJPG structure
///////////////////////////////// DECODE JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void DecodeJPG(jpeg_decompress_struct* cinfo, ImageDataRec *pImageData)
{
	// Read in the header of the jpeg file
	jpeg_read_header(cinfo, TRUE);
	
	// Start to decompress the jpeg file with our compression info
	jpeg_start_decompress(cinfo);

	// Get the image dimensions and row span to read in the pixel data
	pImageData->rowSpan = cinfo->image_width * cinfo->num_components;
	pImageData->sizeX   = cinfo->image_width;
	pImageData->sizeY   = cinfo->image_height;
	
	// Allocate memory for the pixel buffer
	pImageData->data = new unsigned char[pImageData->rowSpan * pImageData->sizeY];
		
	// Here we use the library's state variable cinfo.output_scanline as the
	// loop counter, so that we don't have to keep track ourselves.
	
	// Create an array of row pointers
	unsigned char** rowPtr = new unsigned char*[pImageData->sizeY];
	for (int i = 0; i < pImageData->sizeY; i++)
		rowPtr[i] = &(pImageData->data[i*pImageData->rowSpan]);

	// Now comes the juice of our work, here we extract all the pixel data
	int rowsRead = 0;
	while (cinfo->output_scanline < cinfo->output_height) 
	{
		// Read in the current row of pixels and increase the rowsRead count
		rowsRead += jpeg_read_scanlines(cinfo, &rowPtr[rowsRead], cinfo->output_height - rowsRead);
	}
	
	// Delete the temporary row pointers
	delete [] rowPtr;

	// Finish decompressing the data
	jpeg_finish_decompress(cinfo);
}


///////////////////////////////// LOAD BMP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////	This loads the BMP file and returns it's data in a AUX_RGBImageRec struct
///////////////////////////////// LOAD BMP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
ImageDataRec	*LoadJPG(const char *filename)
{
	struct jpeg_decompress_struct cinfo;
	ImageDataRec *pImageData = NULL;
	FILE *pFile;
	
	// This is the only function you should care about.  You don't need to
	// really know what all of this does (since you can't cause it's a library!) :)
	// Just know that you need to pass in the jpeg file name, and get a pointer
	// to a tImageJPG structure which contains the width, height and pixel data.
	// Be sure to free the data after you are done with it, just like a bitmap.
	
	// Open a file pointer to the jpeg file and check if it was found and opened 
	if((pFile = fopen(filename, "rb")) == NULL) 
	{
		// Display an error message saying the file was not found, then return NULL
		cout << "Unable to load JPG file" << endl;
		return NULL;
	}
	
	// Create an error handler
	jpeg_error_mgr jerr;

	// Have our compression info object point to the error handler address
	cinfo.err = jpeg_std_error(&jerr);
	
	// Initialize the decompression object
	jpeg_create_decompress(&cinfo);
	
	// Specify the data source (Our file pointer)	
	jpeg_stdio_src(&cinfo, pFile);
	
	// Allocate the structure that will hold our eventual jpeg data (must free it!)
	pImageData = (ImageDataRec*)malloc(sizeof(ImageDataRec));

	// Decode the jpeg file and fill in the image data structure to pass back
	DecodeJPG(&cinfo, pImageData);
	
	// This releases all the stored memory for reading and decoding the jpeg
	jpeg_destroy_decompress(&cinfo);
	
	// Close the file pointer that opened the file
	fclose(pFile);

	// Return the jpeg data (remember, you must free this data after you are done)
	return pImageData;
}






///////////////////////////////// LOAD JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////	This loads the JPG file and returns it's data in a AUX_RGBImageRec struct
///////////////////////////////// LOAD JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
ImageDataRec	*LoadBMP(const char *mFileName)
{
	AUX_RGBImageRec *oldStructure = auxDIBImageLoad(mFileName);				// Load the bitmap and store the data in mImage
	ImageDataRec	*newStructure = new ImageDataRec;

	newStructure->data = oldStructure->data;
	newStructure->sizeX = oldStructure->sizeX;
	newStructure->sizeY = oldStructure->sizeY;

	return newStructure;
}




/*****************************************************************************\
|	TEXTURE OBJECT CLASS														  |
\*****************************************************************************/

///////////////////////////////////////  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//	TextureSystem - This is the constructor for the texture system
///////////////////////////////////////  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
CTextures::CTextures()
{
	//nothing
}


///////////////////////////////////////  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//	~TextureSystem - This will clean up memory 
///////////////////////////////////////  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
CTextures::~CTextures()
{
	//nothing	
}


///////////////////////////////////////  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//	FileExists - this checks to see if the file exists before opening it.
///////////////////////////////////////  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool CTextures::FileExist(char *mFileName)
{

	FILE *File = NULL;									// File Handle

	if (!mFileName)										// Make Sure A Filename Was Given
	{	return false;	}								// No file specified
	
	//open the file to see if it does exist
	File = fopen(mFileName,"r");							// Check To See If The File Exists
	if (!File)											// If the file does not exist then exit
	{	return false;	}								
	
	fclose(File);										// Close The Handle

	//return success = file exists
	return true;
}


///////////////////////////////////////  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//	LoadTexture - This load a texture from a file and put it into memory
///////////////////////////////////////  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool CTextures::LoadTexture(char *mFileName, UINT &mTexture)
{
	ImageDataRec *mImage = NULL;
	char tempstring[5] = {0};

	//post some type of debug information
	if ( TEXTURE_DEBUG ) {	cout << "Loading Texture: " << mFileName << "...";   }
	
	//check to see if the file exists	
	if (!FileExist(mFileName))							//check to see if the file exists
	{	
		if ( TEXTURE_DEBUG ) {	cout << "Failed!!! - File does not exist" << endl ;   }
		return false;	
	}	
		
	//get the file extension of the file
	strncpy(tempstring, mFileName + strlen(mFileName) - 4, 4);


	//Load the texture information (load based on if it is a JPG or BMP)
	if((!strcmp(tempstring, ".jpg"))||(!strcmp(tempstring, ".JPG")))
	{	
		mImage = LoadJPG(mFileName);	
	}	
	else if((!strcmp(tempstring, ".bmp"))||(!strcmp(tempstring, ".BMP")))
	{	
		mImage = LoadBMP(mFileName);	
	}
	else
	{	return false;		}
		
			
	if(mImage == NULL)									// If we can't load the file, return false
	{						
		if ( TEXTURE_DEBUG ) {	cout << "Failed!!!" << endl ;   }
		return false;
	}
	
	// Generate a texture with the texture ID stored in the texture item
	glGenTextures(1, &mTexture);

	// Bind the texture to OpenGL and initalize the texture - GL_TEXTURE_2D tells it we are using a 2D texture
	glBindTexture(GL_TEXTURE_2D, mTexture);

	// Build Mipmaps - this builds different versions of the picture for distances to make the image look better at different distances
	// gluBuild2DMipmaps Parameters: (2D texture, 3 channels (RGB), bitmap width, bitmap height, It is an RGB format, data is stored as unsigned bytes, the actuall pixel data);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, mImage->sizeX, mImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, mImage->data);

	// Tell OpenGL the quality of our texture map.  
	//		GL_LINEAR_MIPMAP_LINEAR is the smoothest.  
	//		GL_LINEAR_MIPMAP_NEAREST is faster than GL_LINEAR_MIPMAP_LINEAR, but looks blochy and pixilated.  		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// Free up the bitmap information
	if (mImage)										// If we loaded the bitmap
	{
		if (mImage->data)								// If there is texture data
		{	free(mImage->data);	}					// Free the texture data, we don't need it anymore

		free(mImage);									// Free up the bitmap structure
	}

	if ( TEXTURE_DEBUG ) {	cout << "Done." << endl ;   }

	return true;
}

