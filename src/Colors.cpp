#include "Colors.h"

ColorRGBA get_indexed_color(int index)
{
	
	if (index == 0) return color_int_to_float(88, 196, 103, 255); // green
	if (index == 1) return color_int_to_float(236, 80, 103, 255); // red
	if (index == 2) return color_int_to_float(101, 129, 245, 255); // blue
	if (index == 3) return color_int_to_float(170, 163, 186, 255); // black
	if (index == 4) return color_int_to_float(237, 200, 48, 255); // yellow
	if (index == 5) return color_int_to_float(190, 83, 195, 255); // pink
	if (index == 6) return color_int_to_float(81, 222, 213, 255); // cyan
	if (index == 7) return color_int_to_float(142, 78, 45, 255); // brown
	//if (index == 7) return color_int_to_float(238, 245, 245, 255); // white	
	return ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f);
	
}


ColorRGBA get_indexed_bright_color(int index)
{
	
	if (index == 0) return color_int_to_float(42, 201, 36, 255); // green
	if (index == 1) return color_int_to_float(236, 36, 39, 255); // red
	if (index == 2) return color_int_to_float(44, 80, 245, 255); // blue
	if (index == 3) return color_int_to_float(41, 38, 46, 255); // black
	if (index == 4) return color_int_to_float(255, 207, 0, 255); // yellow
	if (index == 5) return color_int_to_float(235, 52, 246, 255); // pink
	if (index == 6) return color_int_to_float(0, 255, 210, 255); // cyan
	if (index == 7) return color_int_to_float(200, 108, 60, 255); // brown
	//if (index == 7) return color_int_to_float(245, 245, 245, 255); // white
	return WHITE;
	//return get_indexed_color(index);
	
}
