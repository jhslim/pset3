// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure that proper number of arguments are passed in command line
    if (argc != 4)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }
    //set user provided value for scaling to n
    int n = atoi(argv[1]);
    //ensure that value of n is greater than 0 and less than or equal to 100
    if (n < 0 || n >= 100)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }



    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        printf("Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        printf("Unsupported file format.\n");
        return 4;
    }

    //create variables to store outfile's BITMAPFILEHEADER and BITMAPINFOHEADER values
    BITMAPFILEHEADER bf_out = bf;
    BITMAPINFOHEADER bi_out = bi;

    //alter biWidth and biHeight according to n value provided by user for outfile and store in struct variables
    bi_out.biWidth = bi.biWidth * n;
    bi_out.biHeight = bi.biHeight * n;

    //Check biWidth and biHeight values
    printf("Original biWidth: %d\n", bi.biWidth);
    printf("Original biHeight: %d\n", bi.biHeight);
    printf("Resized biWidth: %d\n", bi_out.biWidth);
    printf("Resized biHeight: %d\n", bi_out.biHeight);

    // determine padding for scanlines
    int infile_padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int outfile_padding = (4 - (bi_out.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    printf("Infile padding: %i\n", infile_padding);
    printf("Outfile padding: %i\n", outfile_padding);

    //alter outfile's biSizeImage according to new bi_out.biWidth and bi_out.biHeight value and padding value
    bi_out.biSizeImage = ((sizeof(RGBTRIPLE) * bi_out.biWidth) + outfile_padding) * abs(bi_out.biHeight);

    printf("Original biSizeImage: %d\n", bi.biSizeImage);
    printf("Resized biSizeImage: %d\n", bi_out.biSizeImage);

    //alter outfile's bfSize value according to new biSizeImage value and the size of BITMAPFILEHEADER and BITMAPINFOHEADER
    bf_out.bfSize = bi_out.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    printf("Original bfSize: %d\n", bf.bfSize);
    printf("Resized bfSize: %d\n", bf_out.bfSize);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf_out, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi_out, sizeof(BITMAPINFOHEADER), 1, outptr);



    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        RGBTRIPLE lineofpixels [bi_out.biWidth];

        // iterate over pixels in scanline horizontally
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile at specified pixel in scanline
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            //iterate over user provided n number of times, horizontally
            for (int k = 0; k < n; k++)
            {
                // write RGB triple to outfile
                //fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);

                //place current value of triple into lineofpixels array at position j
                lineofpixels[j] = triple;
            }
        }

        for(int l = 0; l < n; l++)
        {
            fwrite(&lineofpixels, sizeof(RGBTRIPLE), 1, outptr);

            //add padding to outptr at the end of scanline
            for (int m = 0; m < outfile_padding; m++)
            {
                fputc(0x00, outptr);
            }
        }

        // skip over padding, if any in input picture after clearing whole scanline
        fseek(inptr, infile_padding, SEEK_CUR);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
