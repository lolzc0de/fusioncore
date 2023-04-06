#ifndef __FONT_H_
#define __FONT_H_

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

typedef struct {
	unsigned char Magic[2];
	unsigned char Mode;
	unsigned char CharSize;
} PSF1_HEADER;

typedef struct {
	PSF1_HEADER *Header;
	void *GlyphBuffer;
} PSF1_FONT;


#endif /* __FONT_H_ */