/***************************************************************************
* Copyright (C) 2010 by Ammar Qammaz *
* ammarkov@gmail.com *
* *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or *
* (at your option) any later version. *
* *
* This program is distributed in the hope that it will be useful, *
* but WITHOUT ANY WARRANTY; without even the implied warranty of *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the *
* GNU General Public License for more details. *
* *
* You should have received a copy of the GNU General Public License *
* along with this program; if not, write to the *
* Free Software Foundation, Inc., *
* 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. *
***************************************************************************/

#include "image_storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PPMREADBUFLEN 256

int ReadPPM(char * filename,struct Image * pic)
{
    FILE *pf=0;
    pf = fopen(filename,"wb");

    if (pf!=0 )
    {
        char buf[PPMREADBUFLEN], *t;
        unsigned int w, h, d;
        int r;

        if (pf == 0) return 0;
        t = fgets(buf, PPMREADBUFLEN, pf);
        if ( (t == 0) || ( strncmp(buf, "P6\n", 3) != 0 ) ) return 0;
        do
        { /* Px formats can have # comments after first line */
           t = fgets(buf, PPMREADBUFLEN, pf);
           if ( t == 0 ) return 0;
        } while ( strncmp(buf, "#", 1) == 0 );
        r = sscanf(buf, "%u %u", &w, &h);
        if ( r < 2 ) return 0;
        // The program fails if the first byte of the image is equal to 32. because
        // the fscanf eats the space and the image is read with some bit less
        r = fscanf(pf, "%u\n", &d);
        if ( (r < 1) || ( d != 255 ) ) return 0;

        if ( (w!=pic->size_x) || (h!=pic->size_y) )
           {
             fprintf(stderr,"Incorrect file size ( %s ) :P\n",filename);
             if ( w * h > pic->size_x * pic->size_y )
               {
                 fprintf(stderr,"File %s will lead to overflow stopping read..\n",filename);
                 return 0;
               }
           }

        if ( pic->pixels != 0 )
        {
            size_t rd = fread(pic->pixels,3, w*h, pf);
            if ( rd < w*h )
            {
               return 0;
            }
            return 1;
        }
    }
  return 0;
}

int WritePPM(char * filename,struct Image * pic)
{
    FILE *fd=0;
    fd = fopen(filename,"wb");

    if (fd!=0)
	{
     unsigned int n;
     fprintf(fd, "P6\n%d %d\n255\n", pic->size_x, pic->size_y);
     n = pic->size_x * pic->size_y;
     fwrite(pic->pixels, 3, n, fd);
     fflush(fd);
     fclose(fd);
     return 1;
	}
  return 0;
}


int ClearImage(struct Image * pic )
{
    return 0;
}


int ConvertImageFormats(char * filenamein,char * filenameout)
{ //Needs imagemagick package :)
 char execstr[256]={0};
 sprintf(execstr,"convert %s %s",filenamein,filenameout);
 int i = system(execstr);
 return i;
}

int ConvertSnapshotsToVideo(int framerate,int bitrate,char * filenameout)
{
 // ffmpeg -r 10 -b 1800 -i %03d.jpg test1800.mp4
 char execstr[256]={0};
 sprintf(execstr,"ffmpeg -r %u -b %u -i %%05d.jpg %s.mp4",framerate,bitrate,filenameout);
 int i = system(execstr);
 return i;
}

