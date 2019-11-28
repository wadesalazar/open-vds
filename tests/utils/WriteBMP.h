#include <cstdio>
#include <cstdint>

void writeBMP(int out_width, int out_height, FILE *file, const void *data, size_t size)
{
  uint32_t filesize = 54 + size;
  uint8_t bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
  uint8_t bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
  uint8_t bmppad[3] = {0,0,0};

  bmpfileheader[ 2] = (uint8_t)(filesize    );
  bmpfileheader[ 3] = (uint8_t)(filesize>> 8);
  bmpfileheader[ 4] = (uint8_t)(filesize>>16);
  bmpfileheader[ 5] = (uint8_t)(filesize>>24);

  bmpinfoheader[ 4] = (uint8_t)(out_width    );
  bmpinfoheader[ 5] = (uint8_t)(out_width>> 8);
  bmpinfoheader[ 6] = (uint8_t)(out_width>>16);
  bmpinfoheader[ 7] = (uint8_t)(out_width>>24);
  bmpinfoheader[ 8] = (uint8_t)(out_height    );
  bmpinfoheader[ 9] = (uint8_t)(out_height>> 8);
  bmpinfoheader[10] = (uint8_t)(out_height>>16);
  bmpinfoheader[11] = (uint8_t)(out_height>>24);
  fwrite(bmpfileheader,1,14,file);
  fwrite(bmpinfoheader,1,40,file);

  for (int32_t i = 0; i < out_height; i++)
  {
    fwrite(reinterpret_cast<const uint8_t *>(data) + (out_width*(out_height-i-1)*3),3,size_t(out_width),file);
    fwrite(bmppad,1,(-3 * out_width) & 3, file);
  }
  fflush(file);
}
