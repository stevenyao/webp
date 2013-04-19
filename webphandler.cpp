/* Copyright (c) 2012, Corentin Jabot. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

  * Neither the name of the author nor the names of its contributors may
    be used to endorse or promote products derived from this software
    without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "webphandler.h"
#include "webp/decode.h"
#include "webp/encode.h"
#include <QImage>
#include <QDebug>
#include <QVariant>

static const char* const KErrorMessages[] = {
	"OK",
	"OUT_OF_MEMORY: Out of memory allocating objects",
	"BITSTREAM_OUT_OF_MEMORY: Out of memory re-allocating byte buffer",
	"NULL_PARAMETER: NULL parameter passed to function",
	"INVALID_CONFIGURATION: configuration is invalid",
	"BAD_DIMENSION: Bad picture dimension. Maximum width and height "
	"allowed is 16383 pixels.",
	"PARTITION0_OVERFLOW: Partition #0 is too big to fit 512k.\n"
	"To reduce the size of this partition, try using less segments "
	"with the -segments option, and eventually reduce the number of "
	"header bits using -partition_limit. More details are available "
	"in the manual (`man cwebp`)",
	"PARTITION_OVERFLOW: Partition is too big to fit 16M",
	"BAD_WRITE: Picture writer returned an I/O error",
	"FILE_TOO_BIG: File would be too big to fit in 4G",
	"USER_ABORT: encoding abort requested by user"
};

class WebPHandlerPrivate
{
public:
	WebPHandlerPrivate()
	{
		lossless = false;
		quality = 90;
	}

	bool lossless;
	int quality;
};

WebPHandler::WebPHandler()
: d(new WebPHandlerPrivate())
{
}

WebPHandler::~WebPHandler()
{
	delete d;
}

bool WebPHandler::canRead () const{
    return true;
}

bool WebPHandler::read ( QImage * image ){
    if(!canRead()) {
        return false;
    }

    QByteArray data = device()->readAll();
    int width = 0, height = 0;
	if(WebPGetInfo((const uint8_t*)data.constData(), data.size(), &width, &height)) {
		QImage result(width, height, QImage::Format_ARGB32);
		uint8_t *output = result.bits();
		size_t output_size = result.byteCount();
		if(WebPDecodeBGRAInto((const uint8_t*)data.constData(), data.size(), output, output_size, result.bytesPerLine())) {
			*image = result;
			return true;
		}
	}

    return false;
}

static int pictureWriter(const uint8_t* data, size_t data_size,
					const WebPPicture* const pic)
{
	QIODevice *io = (QIODevice*)pic->custom_ptr;
	
	return data_size ? (io->write((const char*)data, data_size) == data_size) : 1;
}

bool WebPHandler::write( const QImage &image )
{
	QImage srcImage = image;
	if(srcImage.format() != QImage::Format_ARGB32) {
		srcImage = srcImage.convertToFormat(QImage::Format_ARGB32);

		if(srcImage.isNull()) {
			qWarning() << "source image is null.";
			return false;
		}
	}

	WebPPicture picture;
	WebPConfig config;

	if( !WebPPictureInit(&picture) || !WebPConfigInit(&config) ) {
		qWarning() << "failed to init webp picture and config";
		return false;
	}

	picture.width = image.width();
	picture.height = image.height();
	picture.use_argb = 1;
	if( !WebPPictureImportBGRA(&picture, image.bits(), image.bytesPerLine()) ) {
		qWarning() << "failed to import image data to webp picture.";

		WebPPictureFree(&picture);
		return false;
	}

	config.lossless = d->lossless;
	config.quality = d->quality;
	picture.writer = pictureWriter;
	picture.custom_ptr = (void*)device();

	if( !WebPEncode(&config, &picture) ) {
		qWarning() << "failed to encode webp picture, error code: " << picture.error_code << " error message: " << KErrorMessages[picture.error_code];
		WebPPictureFree(&picture);
		return false;
	}
	
	WebPPictureFree(&picture);

	return true;
}

QVariant WebPHandler::option( ImageOption option ) const
{
	switch(option)
	{
	case Quality:
		return d->quality;

	default:
		return QImageIOHandler::option(option);
	}
}

void WebPHandler::setOption( ImageOption option, const QVariant &value )
{
	switch(option)
	{
	case Quality:
		d->quality = value.toInt();
		d->lossless = (d->quality >= 100);
		break;

	default:
		return QImageIOHandler::setOption(option, value);
	}
}

bool WebPHandler::supportsOption( ImageOption option ) const
{
	return option == Quality;
}

QByteArray WebPHandler::name() const
{
	return "webp";
}
