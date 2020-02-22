#include <u.h>
#include <libc.h>

typedef struct DMS DMS;
struct DMS
{
	vlong	d;	/* degrees (°) */
	vlong	m;	/* minutes (') */
	double	s;	/* seconds (") */
};

#pragma varargck type "D" DMS
int
Dfmt(Fmt *f)
{
	DMS dms;

	dms = va_arg(f->args, DMS);
	if(dms.d < 0){
		dms.m = -dms.m;
		dms.s = -dms.s;
	}
	return fmtprint(f, "%lld°%lld'%g\"", dms.d, dms.m, dms.s);
}

DMS
deg2dms(double deg)
{
	DMS dms;

	dms.d = deg;
	deg -= dms.d;
	dms.m = deg = deg*60;
	deg -= dms.m;
	dms.s = deg*60;
	return dms;
}

double
dms2deg(DMS dms)
{
	return dms.d + dms.m/60.0 + dms.s/3600.0;
}

void
usage(void)
{
	fprint(2, "usage: %s deg\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	DMS dms;
	Rune r;
	char *s;
	double degs;

	fmtinstall('D', Dfmt);
	ARGBEGIN{
	default: usage();
	}ARGEND;
	if(argc != 1)
		usage();
	s = argv[0];
	degs = strtod(s, &s);
	if(*s != 0){
		memset(&dms, 0, sizeof(DMS));
		while(*s != 0){
			s += chartorune(&r, s);
			switch(r){
			case L'°':
				dms.d = degs;
				degs = strtod(s, &s);
				break;
			case '\'':
				dms.m = dms.d < 0? -degs: degs;
				degs = strtod(s, &s);
				break;
			case '\"':
				dms.s = dms.d < 0? -degs: degs;
				break;
			default:
				sysfatal("unknown format");
			}
		}
		degs = dms2deg(dms);
		print("%g°\n", degs);
	}else{
		dms = deg2dms(degs);
		print("%D\n", dms);
	}
	exits(0);
}
