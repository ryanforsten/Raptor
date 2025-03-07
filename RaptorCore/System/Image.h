/***************************************************************************/
/*                                                                         */
/*  Image.h                                                                */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
#define AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CTextureFactoryConfig;


class RAPTOR_API CImage  
{
public:
	//!	Image data type (pixels type)
	typedef enum
	{
		CGL_COLOR24,
		CGL_COLOR24_ALPHA,
		CGL_COLOR_FLOAT16,
		CGL_COLOR_FLOAT16_ALPHA,
		CGL_COLOR_FLOAT32,
		CGL_COLOR_FLOAT32_ALPHA
	} PIXEL_TYPE;

    //! This class defines an Image I/O service. The user can extend Raptor texture loading capabilities
    //! by implementing and registering a custom IO class into the TextureFactoryConfig.
    //! TextureFactories will dynamically request for an available ImageIO to load a texture.
    class IImageIO
    {
    public:
		//!	Virtual destructor
		virtual ~IImageIO() {};

		//!	Returns the kind of image managed ( common extension used for image type: e.g. jpg)
		virtual bool isOfKind(const std::string &kind) const = 0;

		//! Returns the list of extension kind handled by this imageIO.
		virtual std::vector<std::string> getImageKind(void) const = 0;

		//! Method prototype for image loading 'from file'
		//!	@param fname : full filename, with path and file extensions
		//! @param I : a valid image object.
		//! @eturn true if loading is successfull.
		virtual bool loadImageFile(const std::string& fname, CImage* const I) const = 0;

        //! Method prototype for texture storing 'to file'
	    //!	@param fname : full filename, with path and file extensions
	    //! @param I : a valid image object, the image pixels must be accesible through I->getPixels or I->getFloatPixels.
	    //! @return true if storing is successfull.
		virtual bool storeImageFile(const std::string& fname, CImage* const I) const = 0;

    protected:
        IImageIO() {};
        IImageIO(const IImageIO&) {};
		IImageIO& operator=(const IImageIO&) { return *this; };
    };

    //! This class defines an Image Operation service. The user can extend Raptor texture loading capabilities
    //! by implementing and registering a custom Operator class into the TextureFactoryConfig. 
    //! Such an operator will be used during texture loading or/and generation phases.
    class RAPTOR_API IImageOP
    {
    public:
        //! This enum defines the standard operations available
        typedef enum
        {
            BUMPMAP_LOADER,
            IMAGE_SCALER,
			ALPHA_TRANSPARENCY,
			MIPMAP_BUILDER,
			OTHER_OP,
			NB_OP_KIND
        } OP_KIND;

		//!	Virtual destructor
		virtual ~IImageOP() {};

        //!	Returns the kind of operation managed ( extension )
        virtual OP_KIND getKind(void) const = 0;

		//! Apply the specific operator to the image ( it must be valid and have been loaded )
		//! @param src : a valid image object, defined as the source of pixels
		//! @return true if no error, false otherwise.
		virtual bool apply(CImage* const src) const = 0;
						   //,const operation_param_t& param) const = 0;

    protected:
        IImageOP() {};
        IImageOP(const IImageOP&) {}
		IImageOP& operator=(const IImageOP&) { return *this; };
    };



public:
	//!	Constructor.
	CImage();

	//!	Destructor.
	virtual ~CImage();

	//!	Create a new image initialised with the subpart of this image defined by the parameters.
	//!	@param x: horizontal position of ref point of subimage in this.
	//!	@param y: vertical position of ref point of subimage in this.
	//!	@param w: width of subimage in this.
	//!	@param h: height of subimage in this.
	//!	@return the subimage or NULL if incorrect parameters of uninitialised this.
	CImage* createSubImage(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

	//!	Loads an image file named filename. The file type is used to
	//!	Determine the appropriate image loader, if any.
	//! @return false if the an error is encountered when trying to access filename of no loader found.
	bool loadImage(const std::string &filename,
				   const CVaArray<CImage::IImageOP*>& ops);

	//! texture name ( default is the source filename )
	const std::string & getName(void) const { return m_name; };

	//! @return image width
	uint32_t	getWidth(void) const { return m_width; };

	//! @return image height
	uint32_t	getHeight(void) const { return m_height; };

	//! @return image height
	uint32_t	getLayers(void) const { return m_layers; };


	//!	Returns the sized format of buffer in host memory to upload or download texels from server.
	//!	(buffer is allocated with allocateTexels)
	unsigned int getBufferFormat(void) const;

	//! Returns the type of a texel componant in host buffer
	//!	(buffer is allocated with allocateTexels)
	unsigned int getBufferType(void) const;

	//! Returns the type of a texel componant in host buffer
	//!	(buffer is allocated with allocateTexels)
	CImage::PIXEL_TYPE getPixelType(void) const
	{
		return m_pixelType;
	};


	//! Allocates a bloc of texels to pass data to server. Memory allocator must be in library.
	//! The size is dependent on texture attributes, it might be reallocated or not.
	//! @param width : image width in pixels.
	//!	@param height : image height in pixels.
	//! @param type : the type of pixels to allocate, it cannot be changed later on.
	//! @return true if allocation is successfull.
	bool allocatePixels(uint32_t width, uint32_t height, PIXEL_TYPE type = CGL_COLOR24_ALPHA);

	//! Allocates a bloc of texels for several image layers.
	//! @param width : image width in pixels.
	//!	@param height : image height in pixels.
	//! @param layers : the number of layers for this image.
	//! @param type : the type of pixels to allocate, it cannot be changed later on.
	//! @return true if allocation is successfull.
	bool allocatePixels(uint32_t width, uint32_t height, uint32_t layers, PIXEL_TYPE type = CGL_COLOR24_ALPHA);

	//! Releases any allocated texel blocs.
	void releasePixels(void);

	//!	Obtain image pixels.
	//!	@return pointer to pixels array or NULL if image do not match
	uint8_t* getPixels(uint32_t layer = 0) const;

	//!	Obtain image pixels.
	//!	@return pointer to pixels array or NULL if image do not match
	float* getFloatPixels(uint32_t layer = 0) const;

	//!	Callbacks management :
    //!
	//!	Adds a loader/storer class for a specific kind of image (used mainly by LoadTexture ).
	//!	The different imagers are choosen by the file extension when LoadTexture is called. 
    //! If there is already an imager for an extension, the one given here replaces the existing one.
	//!	By default, there is only a buffer loader set. Some basic loaders are provided by CRaptorToolBox
	static void setImageKindIO(IImageIO *imager);

	//!	Returns a loader given a file name or file extension.
	//! The loader is one defined above.
	static IImageIO* const getImageKindIO(const std::string &extension);




private:
	//!	Forbidden assignment operator
	CImage& operator=(const CImage&) { return *this; };

	//!	Object name ( default is filename )
	std::string	m_name;

	//! Image dimensions
	uint32_t	m_width;
	uint32_t	m_height;
	uint32_t	m_layers;

	//!	The buffer pixel type for loading image data
	PIXEL_TYPE		m_pixelType;

	//!	Actual pixels storage
	void			*pixels;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)

