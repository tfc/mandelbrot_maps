
#ifndef __COLORS_H__
#define __COLORS_H__

class HsvColor
{
	public:
		unsigned char h;
		unsigned char s;
		unsigned char v;

		HsvColor(unsigned char _h, unsigned char _s, unsigned char _v)
			: h(_h), s(_s), v(_v)
		{}
};

class RgbColor
{
	public:
		unsigned char r;
		unsigned char g;
		unsigned char b;

		RgbColor(unsigned char _r, unsigned char _g, unsigned char _b)
			: r(_r), g(_g), b(_b)
		{}

		RgbColor(const HsvColor &hsv)
		{
			unsigned char region, remainder, p, q, t;

			if (hsv.s == 0) {
				r = hsv.v;
				g = hsv.v;
				b = hsv.v;
				return;
			}

			region = hsv.h / 43;
			remainder = (hsv.h - (region * 43)) * 6; 

			p = (hsv.v * (255 - hsv.s)) >> 8;
			q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
			t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

			switch (region)
			{
				case 0:
					r = hsv.v; 
					g = t; 
					b = p;
					break;
				case 1:
					r = q; 
					g = hsv.v; 
					b = p;
					break;
				case 2:
					r = p; 
					g = hsv.v; 
					b = t;
					break;
				case 3:
					r = p; 
					g = q; 
					b = hsv.v;
					break;
				case 4:
					r = t; 
					g = p; 
					b = hsv.v;
					break;
				default:
					r = hsv.v; 
					g = p; 
					b = q;
					break;
			}

			return;
		}
};

#endif

