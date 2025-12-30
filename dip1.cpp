#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#pragma pack(1)
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int LONG;
typedef struct _BMPFH
{
	BYTE bftype1;
	BYTE bftype2;
	DWORD bfsize;
	WORD bfreserved1;
	WORD bfreserved2;
	DWORD bfOffbits;
} BMPFH;
typedef struct _BMPIH
{
	DWORD bisize;
	LONG  biw;
	LONG bih;
	WORD biplane;
	WORD bibitcount;
	DWORD biComp;
	DWORD bisizeimage;
	LONG bix;
	LONG biy;
	DWORD biclused;
 	DWORD biclimp;
}  BMPIH;
typedef struct _PALET
{
	BYTE rgbblue;
	BYTE rgbgreen;
	BYTE rgbred;
	BYTE rgbreserved;
} PALET;
typedef struct _IMAGE
{
	BMPFH   bmpfh;
	BMPIH   bmpih;
	PALET   *palet;
	BYTE    *data;
}  IMAGE;
IMAGE *ImageRead(IMAGE *image,char *filename)
{
	BMPFH bmpfh;
	BMPIH bmpih;
	FILE *fp;
	DWORD r,rowsize,size;
	fp=fopen(filename,"rb");
	if(fp==NULL) {printf("File is not found..");exit(1);}
	fread(&bmpfh,sizeof(BMPFH),1,fp);
	if(bmpfh.bftype1!='B' || bmpfh.bftype2!='M')
	{
		printf("This is not a bitmap file.");
		exit(1);
	}
    fread(&bmpih,sizeof(BMPIH),1,fp);//reads bitmap info header from the file and set to bmpih
	image=(IMAGE *) malloc(bmpfh.bfsize);//allocates memory for image
	if(image==NULL) {printf("There is no enough memory for this operation");exit(1);}
	image->bmpfh=bmpfh;//sets bmpfh to image 
	image->bmpih=bmpih;//sets bmpih to image
	
	r=0;//r is set to 0 in case 24 bits per pixel or more (this kind of images does not have color palette)
    if(bmpih.bibitcount==1) r=2;
	if(bmpih.bibitcount==4) r=16;
	if(bmpih.bibitcount==8) r=256;
	{
		image->palet=(PALET *) malloc(4*r);
		fread(image->palet,sizeof(BYTE),4*r,fp);
	}
	rowsize=(image->bmpih.biw*image->bmpih.bibitcount+31)/32*4;
	size=rowsize*image->bmpih.bih;
	image->data=(BYTE *)malloc(size);
	fread(image->data,size,1,fp);
	fclose(fp); 
	return image;
void ImageWrite(IMAGE *image,char *filename)
{
	FILE *fp;
	int r,rowsize,size;
	
	fp=fopen(filename,"wb");
	if(fp==NULL) {printf("Fie opening error!");exit(1);}
	image->bmpfh.bftype2='M';
	fwrite(&image->bmpfh,sizeof(BMPFH),1,fp);
	fwrite(&image->bmpih,sizeof(BMPIH),1,fp);		
	r=0;
    if(image->bmpih.bibitcount==1) r=2;
	if(image->bmpih.bibitcount==4) r=16;
	if(image->bmpih.bibitcount==8) r=256;
	if(r!=0) fwrite(image->palet,4*r,1,fp); to the file
	rowsize=(image->bmpih.biw*image->bmpih.bibitcount+31)/32*4;
	size=rowsize*image->bmpih.bih;
	fwrite(image->data,size,1,fp);
	fclose(fp); 
}
void writeInfo(IMAGE *image,char *fname)
{
	printf("--------Info about %s  image file\n",fname);
	printf("bfType value		:%c%c\n",image->bmpfh.bftype1,image->bmpfh.bftype2);
	printf("bfsize			:%d\n",image->bmpfh.bfsize);
	printf("bfreserved1		:%d\n",image->bmpfh.bfreserved1);
	printf("bfreserved2		:%d\n",image->bmpfh.bfreserved2);
	printf("bfOffbits		:%d\n",image->bmpfh.bfOffbits);
	printf("bisize			:%d\n",image->bmpih.bisize);
	printf("Width			:%d\n",image->bmpih.biw);
	printf("Height			:%d\n",image->bmpih.bih);
	printf("biplane			:%d\n",image->bmpih.biplane);
	printf("bibitcount		:%d\n",image->bmpih.bibitcount);
	printf("Compression		:%d\n",image->bmpih.biComp);
	printf("bisizeimage		:%d\n",image->bmpih.bisizeimage);
	printf("bix			:%d\n",image->bmpih.bix);
	printf("biy			:%d\n",image->bmpih.biy);
	printf("bi color used		:%d\n",image->bmpih.biclused);
	printf("bi color imp.		:%d\n",image->bmpih.biclimp);
}
void negativeImage(IMAGE *image)
{
	DWORD w,h,rowsize,i,j;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	rowsize=(image->bmpih.bibitcount*w+31)/32*4;
   for(i=0;i<h*rowsize;i++) image->data[i]=pow(2,image->bmpih.bibitcount)-1-image->data[i];
    return;
}
void changePalet(IMAGE *image)
{
	int r,i;
	r=pow(2,image->bmpih.bibitcount);
	for(i=0;i<r;i++)
	{
		image->palet[i].rgbblue=r-1-i;
		
		
		
		image->palet[i].rgbgreen=r-1-i;
		image->palet[i].rgbred=r-1-i;
	}
	return;
}
void histogram(IMAGE *image)
{
	FILE *fp;
	int hist[256]={0};
    DWORD w,h,rowsize,i,j;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	rowsize=(image->bmpih.bibitcount*w+31)/32*4;
	for(i=0;i<h*rowsize;i++) hist[image->data[i]]++;
	fp=fopen("hist.txt","w");
	if(fp==NULL) exit(1);
	for(i=0;i<256;i++)
	   fprintf(fp,"%d\t%d\t%lf\n",i,hist[i],(double)hist[i]/(h*rowsize));
	fclose(fp);
	return;	
}
int main()
{
	IMAGE *image=(IMAGE*)malloc(sizeof(IMAGE));
	image=ImageRead(image,"C:\\iemiroglu\\ImageProcessing\\images\\kelebek.bmp");
	
	histogram(image);
	
	
	free(image);   
	return 0;
}
