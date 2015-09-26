
#pragma once

class Noise3D
{
public:
	Noise3D(int width, int height, int depth)
	: noise_width(width), noise_height(height), noise_depth(depth), noise_buffer(noise_width * noise_height * noise_depth * sizeof(double))
	{
		noise = noise_buffer.get_data<double>();
		for (int x = 0; x < noise_width; x++)
		{
			for (int y = 0; y < noise_height; y++)
			{
				for (int z = 0; z < noise_depth; z++)
				{
					noise[x+y*noise_width] = (rand() % 32768) / 32768.0;
				}
			}
		}
	}

	double smooth_noise(double x, double y, double z)
	{ 
		//get fractional part of x and y
		double fract_x = x - int(x);
		double fract_y = y - int(y);
		double fract_z = z - int(z);   
  
		//wrap around
		int x1 = (int(x) + noise_width) % noise_width;
		int y1 = (int(y) + noise_height) % noise_height;
		int z1 = (int(z) + noise_depth) % noise_depth;
  
		//neighbor values
		int x2 = (x1 + noise_width - 1) % noise_width;
		int y2 = (y1 + noise_height - 1) % noise_height;
		int z2 = (z1 + noise_depth - 1) % noise_depth;

		//smooth the noise with bilinear interpolation
		double value = 0.0;
		value += fract_x       * fract_y       * fract_z       * noise[x1+y1*noise_width+z1*noise_width*noise_height];
		value += fract_x       * (1 - fract_y) * fract_z       * noise[x1+y2*noise_width+z1*noise_width*noise_height];
		value += (1 - fract_x) * fract_y       * fract_z       * noise[x2+y1*noise_width+z1*noise_width*noise_height];
		value += (1 - fract_x) * (1 - fract_y) * fract_z       * noise[x2+y2*noise_width+z1*noise_width*noise_height];

		value += fract_x       * fract_y       * (1 - fract_z) * noise[x1+y1*noise_width+z2*noise_width*noise_height];
		value += fract_x       * (1 - fract_y) * (1 - fract_z) * noise[x1+y2*noise_width+z2*noise_width*noise_height];
		value += (1 - fract_x) * fract_y       * (1 - fract_z) * noise[x2+y1*noise_width+z2*noise_width*noise_height];
		value += (1 - fract_x) * (1 - fract_y) * (1 - fract_z) * noise[x2+y2*noise_width+z2*noise_width*noise_height];

		return value;
	}

	double turbulence(double x, double y, double z, double size)
	{
		double value = 0.0, initial_size = size;
   
		while(size >= 1)
		{
			value += smooth_noise(x / size, y / size, z / size) * size;
			size /= 2.0;
		}
   
		return 0.5 * value / initial_size;
	}

private:
	int noise_width;
	int noise_height;
	int noise_depth;
	uicore::DataBuffer noise_buffer;
	double *noise;
};
