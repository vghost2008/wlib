/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "tiffwriter.h"
#include <toolkit.h>
WTiffWriter::WTiffWriter()
:image_width_(0)
,image_type_(TIT_GRAYSCALE)
,pure_data_size_(0)
{
}
int WTiffWriter::setOption(unsigned image_width,TiffImageType image_type)
{
	image_width_   =   image_width;
	image_type_    =   image_type;
	return 0;
}
int WTiffWriter::open(const QString& file_path)
{
	TiffHead       ifh                = {0x4949,0x2a,sizeof(ifh)};
	const uint32_t offset_to_next_ifd = 0;
	const uint16_t dn_count           = IFDI_NR;
	TiffIFD        ifd;

	file_name_     =   file_path;

	if(0 == image_width_) return -TW_ERROR_IMAGE_WIDTH;


	file_.setFileName(file_name_);

	if(!file_.open(QIODevice::WriteOnly)) {
		return -TW_OPEN_FILE_FAILD;
	}

	file_.write((const char*)&ifh,sizeof(TiffHead));
	file_.write((const char*)&dn_count,sizeof(uint16_t));

	memset(&ifd,0,sizeof(TiffIFD));
	for(uint16_t i=0; i<IFDI_NR; ++i) {
		file_.write((const char*)&ifd,sizeof(ifd));
	}

	ifd.tag               =   IFD_NEW_SUB_FILE_TYPE;
	ifd.type              =   TDT_LONG;
	ifd.count             =   1;
	ifd.value_or_offset   =   0;
	writeDirectory(ifd,IFDI_NEW_SUB_FILE_TYPE);

	ifd.tag               =   IFD_IMAGE_WIDTH;
	ifd.type              =   TDT_LONG;
	ifd.count             =   1;
	ifd.value_or_offset   =   image_width_;//number of columns in the image.
	writeDirectory(ifd,IFDI_IMAGE_WIDTH);

	ifd.tag               =   IFD_COMPRESSION;
	ifd.type              =   TDT_SHORT;
	ifd.count             =   1;
	ifd.value_or_offset   =   1;//No compression
	writeDirectory(ifd,IFDI_COMPRESSION);

	ifd.tag               =   IFD_PHOTOMETRIC_INTERPRETATION;
	ifd.type              =   TDT_SHORT;
	ifd.count             =   1;
	ifd.value_or_offset   =   image_type_==TIT_GRAYSCALE?1:2;
	writeDirectory(ifd,IFDI_PHOTOMETRIC_INTERPRETATION);

	ifd.tag               =   IFD_SCRIP_BYTE_COUNTS;
	ifd.type              =   TDT_LONG;
	ifd.count             =   1;
	ifd.value_or_offset   =   0;
	writeDirectory(ifd,IFDI_SCRIP_BYTE_COUNTS);


	ifd.tag               =   IFD_RESOLUTION_UNIT;
	ifd.type              =   TDT_SHORT;
	ifd.count             =   1;
	ifd.value_or_offset   =   1; // No absolute unit of measurement
	writeDirectory(ifd,IFDI_RESOLUTION_UNIT);

	file_.write((const char*)&offset_to_next_ifd,sizeof(offset_to_next_ifd));
	/*>------------------------------------------*/
	writeResolution();
	writeBitsPerSample();
	writeSamplePerPixel();
	/*>------------------------------------------*/
	writeScripOffset();

	file_.flush();
	pure_data_size_ = 0;
	return 0;
}
 size_t WTiffWriter::write(const char* data,size_t size)
{
	if(!isOpen()) return 0;
	pure_data_size_ += size;
	return file_.write(data,size);
}
int WTiffWriter::close()
{
	if(!isOpen() || 0==image_width_) return 0;

	TiffIFD   ifd;
	int       ret          = 0;
	const int pixel_size   = image_type_==TIT_GRAYSCALE?1:3;
	const int image_height = pure_data_size_/(pixel_size *image_width_);


	ifd.tag               =   IFD_IMAGE_LENGTH;
	ifd.count             =   1;
	ifd.type              =   TDT_LONG;
	ifd.value_or_offset   =   image_height;//number of rows(scanlines) in the image.

	if((ret = writeDirectory(ifd,IFDI_IMAGE_LENGTH)) !=0) {
		goto out0;
	}

	ifd.tag               =   IFD_ROWS_PER_STRIP;
	ifd.count             =   1;
	ifd.type              =   TDT_LONG;
	ifd.value_or_offset   =   image_height;
	ret                   =   writeDirectory(ifd,IFDI_ROWS_PER_STRIP);
out0:
	file_.close();
	return ret;
}
int WTiffWriter::writeDirectory(const TiffIFD& ifd,const unsigned index)
{
	const unsigned offset = sizeof(TiffHead) + sizeof(uint16_t) + sizeof(TiffIFD)*index;

	if(!file_.seek(offset)) {
		ERR("Seek to %x faild",offset);
		return -TW_SEEK_FAILD;
	}
	file_.write((const char*)&ifd,sizeof(TiffIFD));
	file_.seek(file_.size());
	return 0;
}
int WTiffWriter::writeResolution()
{
	struct Resolution {
		uint32_t low;
		uint32_t high;
	};
	TiffIFD    ifd;
	Resolution res = {1,0};

	ifd.tag               =   IFD_X_RESOLUTION;
	ifd.type              =   TDT_RATIONAL;
	ifd.count             =   1;
	ifd.value_or_offset   =   file_.size();
	writeDirectory(ifd,IFDI_X_RESOLUTION);

	ifd.tag               =   IFD_Y_RESOLUTION;
	ifd.type              =   TDT_RATIONAL;
	ifd.count             =   1;
	ifd.value_or_offset   =   file_.size()+sizeof(res);
	writeDirectory(ifd,IFDI_Y_RESOLUTION);

	file_.write((const char*)&res,sizeof(res));
	file_.write((const char*)&res,sizeof(res));
	return 0;
}
int WTiffWriter::writeScripOffset()
{
	TiffIFD ifd;

	ifd.tag               =   IFD_SCRIP_OFFSET;
	ifd.type              =   TDT_LONG;
	ifd.count             =   1;
	ifd.value_or_offset   =   file_.size();//真实数据开始偏移地址
	return writeDirectory(ifd,IFDI_SCRIP_OFFSET);
}
int WTiffWriter::writeBitsPerSample()
{
	TiffIFD  ifd;
	uint16_t bit = 8;

	switch(image_type_) {
		case TIT_GRAYSCALE:
			ifd.count             =   1;
			ifd.value_or_offset   =   8;
			break;
		case TIT_RGB_FULL_COLOR:
		default:
			ifd.count             =   3;
			ifd.value_or_offset   =   file_.size();
			file_.seek(file_.size());
			for(int i=0; i<ifd.count; ++i) {
				file_.write((const char*)&bit,sizeof(bit));
			}
			break;
	}
	ifd.tag     =   IFD_BITS_PER_SAMPLE;
	ifd.type    =   TDT_SHORT;

	return writeDirectory(ifd,IFDI_BITS_PER_SAMPLE);
}
int WTiffWriter::writeSamplePerPixel()
{
	const uint16_t samples = TIT_GRAYSCALE==image_type_?1:3;
	TiffIFD        ifd;

	ifd.tag               =   IFD_SAMPLES_PER_PIXEL;
	ifd.type              =   TDT_SHORT;
	ifd.count             =   1;
	ifd.value_or_offset   =   samples;
	return writeDirectory(ifd,IFDI_SAMPLES_PER_PIXEL);
}
