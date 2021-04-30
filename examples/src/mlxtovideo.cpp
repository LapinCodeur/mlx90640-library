
#include "mlxtovideo.hpp"

unsigned char temptopix(float x, unsigned char in_min, unsigned char in_max, unsigned char out_min, unsigned char out_max)
{
        return (unsigned char)(x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main(int argc, char** argv) {

  static uint16_t eeMLX90640[832];
  float emissivity = 0.8;
  uint16_t frame[834];
  static char image[IMAGE_SIZE];
  static float mlx90640To[IMAGE_SIZE];
  unsigned char alldata[IMAGE_SIZE];
  float eTa;
  static int fps = FPS;
  static long frame_time_micros = FRAME_TIME_MICROS;
  char *p;

  if(argc > 1){
      fps = strtol(argv[1], &p, 0);
      if (errno !=0 || *p != '\0') {
          fprintf(stderr, "Invalid framerate\n");
          return 1;
      }
      frame_time_micros = 1000000/fps;
  }

  auto frame_time = std::chrono::microseconds(frame_time_micros + OFFSET_MICROS);

  MLX90640_SetDeviceMode(MLX_I2C_ADDR, 0);
  MLX90640_SetSubPageRepeat(MLX_I2C_ADDR, 0);
  switch(fps){
      case 1:
          MLX90640_SetRefreshRate(MLX_I2C_ADDR, 0b001);
          break;
      case 2:
          MLX90640_SetRefreshRate(MLX_I2C_ADDR, 0b010);
          break;
      case 4:
          MLX90640_SetRefreshRate(MLX_I2C_ADDR, 0b011);
          break;
      case 8:
          MLX90640_SetRefreshRate(MLX_I2C_ADDR, 0b100);
          break;
      case 16:
          MLX90640_SetRefreshRate(MLX_I2C_ADDR, 0b101);
          break;
      case 32:
          MLX90640_SetRefreshRate(MLX_I2C_ADDR, 0b110);
          break;
      case 64:
          MLX90640_SetRefreshRate(MLX_I2C_ADDR, 0b111);
          break;
      default:
          fprintf(stderr, "Unsupported framerate: %d\n", fps);
          return 1;
  }
  MLX90640_SetChessMode(MLX_I2C_ADDR);

  paramsMLX90640 mlx90640;
  MLX90640_DumpEE(MLX_I2C_ADDR, eeMLX90640);
  MLX90640_SetResolution(MLX_I2C_ADDR, 0x03);
  MLX90640_ExtractParameters(eeMLX90640, &mlx90640);

  int width = 32;
  int height = 24;
  int vidsendsiz = 0;

  int v4l2lo = open("/dev/video3", O_WRONLY);
  if(v4l2lo < 0) {
      std::cout << "Error opening v4l2l device: " << strerror(errno);
      exit(-2);
  }
  {
      struct v4l2_format v;
      int t;
      v.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
      t = ioctl(v4l2lo, VIDIOC_G_FMT, &v);
      if( t < 0 ) {
          exit(t);
      }
      v.fmt.pix.width = width;
      v.fmt.pix.height = height;
      v.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
      vidsendsiz = width * height;
      v.fmt.pix.sizeimage = vidsendsiz;
      t = ioctl(v4l2lo, VIDIOC_S_FMT, &v);
      if( t < 0 ) {
          exit(t);
      }
  }

  while (1){
      auto start = std::chrono::system_clock::now();
      MLX90640_GetFrameData(MLX_I2C_ADDR, frame);
      MLX90640_InterpolateOutliers(frame, eeMLX90640);

      eTa = MLX90640_GetTa(frame, &mlx90640); // Sensor ambient temprature
      MLX90640_CalculateTo(frame, &mlx90640, emissivity, eTa, mlx90640To); //calculate temprature of all pixels, base on emissivity of object

      for (int i = 0; i < IMAGE_SIZE; i++) {
        alldata[i] = temptopix(mlx90640To[i], 0, 50, 0, 255);
      }

      size_t written = write(v4l2lo, alldata, IMAGE_SIZE);
      if (written < 0) {
          std::cout << "Error writing v4l2l device";
          close(v4l2lo);
          return 1;
      }

      auto end = std::chrono::system_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::this_thread::sleep_for(std::chrono::microseconds(frame_time - elapsed));
      std::cout << "Updated" << std::endl;
  }

  return 0;
}
