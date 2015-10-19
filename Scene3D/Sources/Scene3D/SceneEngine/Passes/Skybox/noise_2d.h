
#pragma once

class Noise2D
{
public:
	Noise2D(int width, int height)
	: noise_width(width), noise_height(height), noise_buffer(uicore::DataBuffer::create(noise_width * noise_height * sizeof(double)))
	{
		noise = noise_buffer->data<double>();
		for (int x = 0; x < noise_width; x++)
		{
			for (int y = 0; y < noise_height; y++)
			{
				noise[x+y*noise_width] = (rand() % 32768) / 32768.0;
			}
		}
	}

	double smooth_noise(double x, double y)
	{  
	   // get fractional part of x and y
	   double fract_x = x - int(x);
	   double fract_y = y - int(y);
   
	   // wrap around
	   int x1 = (int(x) + noise_width) % noise_width;
	   int y1 = (int(y) + noise_height) % noise_height;
   
	   // neighbor values
	   int x2 = (x1 + noise_width - 1) % noise_width;
	   int y2 = (y1 + noise_height - 1) % noise_height;

	   // smooth the noise with bilinear interpolation
	   double value = 0.0;
	   value += fract_x       * fract_y       * noise[x1+y1*noise_width];
	   value += fract_x       * (1 - fract_y) * noise[x1+y2*noise_width];
	   value += (1 - fract_x) * fract_y       * noise[x2+y1*noise_width];
	   value += (1 - fract_x) * (1 - fract_y) * noise[x2+y2*noise_width];

	   return value;
	}

	double turbulence(double x, double y, double size)
	{
		double value = 0.0, initial_size = size;
    
		while(size >= 1)
		{
			value += smooth_noise(x / size, y / size) * size;
			size /= 2.0;
		}
    
		return 0.5 * value / initial_size;
	}

private:
	int noise_width;
	int noise_height;
	uicore::DataBufferPtr noise_buffer;
	double *noise;
};
