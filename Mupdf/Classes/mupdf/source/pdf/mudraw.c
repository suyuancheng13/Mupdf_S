/*
 * mudraw -- command line tool for drawing pdf/xps/cbz documents
 */

#include "mupdf/fitz.h"
#include "mupdf/pdf.h" /* for mujstest */
#include"pdf-sound.h"
#ifdef _MSC_VER
#include <winsock2.h>
#define main main_utf8
#else
#include <sys/time.h>
#endif

enum { TEXT_PLAIN = 1, TEXT_HTML = 2, TEXT_XML = 3 };

enum { OUT_PNG, OUT_PPM, OUT_PNM, OUT_PAM, OUT_PGM, OUT_PBM, OUT_SVG, OUT_PWG, OUT_PCL, OUT_PDF };

enum { CS_INVALID, CS_UNSET, CS_MONO, CS_GRAY, CS_GRAYALPHA, CS_RGB, CS_RGBA };

typedef struct
{
	char *suffix;
	int format;
} suffix_t;

static const suffix_t suffix_table[] =
{
	{ ".png", OUT_PNG },
	{ ".pgm", OUT_PGM },
	{ ".ppm", OUT_PPM },
	{ ".pnm", OUT_PNM },
	{ ".pam", OUT_PAM },
	{ ".pbm", OUT_PBM },
	{ ".svg", OUT_SVG },
	{ ".pwg", OUT_PWG },
	{ ".pcl", OUT_PCL },
	{ ".pdf", OUT_PDF },
};

typedef struct
{
	char *name;
	int colorspace;
} cs_name_t;

static const cs_name_t cs_name_table[] =
{
	{ "m", CS_MONO },
	{ "mono", CS_MONO },
	{ "g", CS_GRAY },
	{ "gray", CS_GRAY },
	{ "grey", CS_GRAY },
	{ "ga", CS_GRAYALPHA },
	{ "grayalpha", CS_GRAYALPHA },
	{ "greyalpha", CS_GRAYALPHA },
	{ "rgb", CS_RGB },
	{ "rgba", CS_RGBA },
	{ "rgbalpha", CS_RGBA }
};

typedef struct
{
	int format;
	int default_cs;
	int permitted_cs[6];
} format_cs_table_t;

static const format_cs_table_t format_cs_table[] =
{
	{ OUT_PNG, CS_RGB, { CS_GRAY, CS_GRAYALPHA, CS_RGB, CS_RGBA } },
	{ OUT_PPM, CS_RGB, { CS_GRAY, CS_RGB } },
	{ OUT_PNM, CS_GRAY, { CS_GRAY, CS_RGB } },
	{ OUT_PAM, CS_RGBA, { CS_RGBA } },
	{ OUT_PGM, CS_GRAY, { CS_GRAY, CS_RGB } },
	{ OUT_PBM, CS_MONO, { CS_MONO } },
	{ OUT_SVG, CS_RGB, { CS_RGB } },
	{ OUT_PWG, CS_RGB, { CS_MONO, CS_GRAY, CS_RGB } },
	{ OUT_PCL, CS_MONO, { CS_MONO } },
	{ OUT_PDF, CS_RGB, { CS_RGB } }
};

/*
	A useful bit of bash script to call this to generate mjs files:
	for f in tests_private/pdf/forms/v1.3/ *.pdf ; do g=${f%.*} ; echo $g ; ../mupdf.git/win32/debug/mudraw.exe -j $g.mjs $g.pdf ; done

	Remove the space from "/ *.pdf" before running - can't leave that
	in here, as it causes a warning about a possibly malformed comment.
*/


static char *output = NULL;

static float rotation = 0;

static int showxml = 0;
static int showtext = 0;
static int showtime = 0;
static int showmd5 = 0;
static pdf_document *pdfout = NULL;
static int showoutline = 0;
static int uselist = 1;
static int alphabits = 8;

static int errored = 0;
static int ignore_errors = 0;
static int output_format;

static int out_cs = CS_UNSET;


static fz_colorspace *colorspace;
static char *filename;
static int files = 0;
fz_output *out = NULL;

static char *mujstest_filename = NULL;
static FILE *mujstest_file = NULL;


static struct {
	int count, total;
	int min, max;
	
int minpage, maxpage;
	char *minfilename;
	char *maxfilename;
} timing;

static int gettime(void)
{
	static struct timeval first;
	static int once = 1;
	struct timeval now;
	if (once)
	{
		gettimeofday(&first, NULL);
		once = 0;
	}
	gettimeofday(&now, NULL);
	return (now.tv_sec - first.tv_sec) * 1000 + (now.tv_usec - first.tv_usec) / 1000;
}

static int isrange(char *s)
{
	while (*s)
	{
		if ((*s < '0' || *s > '9') && *s != '-' && *s != ',')
			return 0;
		s++;
	}
	return 1;
}

static void escape_string(FILE *out, int len, const char *string)
{
	while (len-- && *string)
	{
		char c = *string++;
		switch (c)
		{
		case '\n':
			fputc('\\', out);
			fputc('n', out);
			break;
		case '\r':
			fputc('\\', out);
			fputc('r', out);
			break;
		case '\t':
			fputc('\\', out);
			fputc('t', out);
			break;
		default:
			fputc(c, out);
		}
	}
}

static void drawpage(fz_context *ctx, fz_document *doc, int pagenum)
{
	fz_page *page;
	fz_display_list *list = NULL;
	fz_device *dev = NULL;
	int start;
	fz_cookie cookie = { 0 };
	int needshot = 0;

	fz_var(list);
	fz_var(dev);
	fz_try(ctx)
	{
		page = fz_load_page(doc, pagenum - 1);
	}
	fz_catch(ctx)
	{
		fz_rethrow_message(ctx, "cannot load page %d in file '%s'", pagenum, filename);
	}

	
	if (uselist)
	{
		fz_try(ctx)
		{
			list = fz_new_display_list(ctx);
			dev = fz_new_list_device(ctx, list);
			fz_run_page(doc, page, dev, &fz_identity, &cookie);
		}
		fz_always(ctx)
		{
			fz_free_device(dev);
			dev = NULL;
		}
		fz_catch(ctx)
		{
			fz_drop_display_list(ctx, list);
			fz_free_page(doc, page);
			fz_rethrow_message(ctx, "cannot draw page %d in file '%s'", pagenum, filename);
		}
	}


	if (pdfout)
	{
		fz_matrix ctm;
		fz_rect bounds, tbounds;
		pdf_page *newpage;

		fz_bound_page(doc, page, &bounds);
		fz_rotate(&ctm, rotation);
		tbounds = bounds;
		fz_transform_rect(&tbounds, &ctm);

		newpage = pdf_create_page(pdfout, bounds, 72, 0);

		fz_try(ctx)
		{
			dev = pdf_page_write(pdfout, newpage);
			if (list)
				fz_run_display_list(list, dev, &ctm, &tbounds, &cookie);
			else
				fz_run_page(doc, page, dev, &ctm, &cookie);
			fz_free_device(dev);
			dev = NULL;
		}
		fz_always(ctx)
		{
			fz_free_device(dev);
			dev = NULL;
		}
		fz_catch(ctx)
		{
			fz_drop_display_list(ctx, list);
			fz_free_page(doc, page);
			fz_rethrow(ctx);
		}
		pdf_insert_page(pdfout, newpage, INT16_MAX);
		pdf_free_page(pdfout, newpage);
	}

	if (list)
		fz_drop_display_list(ctx, list);

	fz_free_page(doc, page);
	fz_flush_warnings(ctx);


	if (cookie.errors)
		errored = 1;
}

static void drawrange(fz_context *ctx, fz_document *doc, char *range)
{
	int page, spage, epage, pagecount;
	char *spec, *dash;

	pagecount = fz_count_pages(doc);
	spec = fz_strsep(&range, ",");
	while (spec)
	{
		dash = strchr(spec, '-');

		if (dash == spec)
			spage = epage = pagecount;
		else
			spage = epage = atoi(spec);

		if (dash)
		{
			if (strlen(dash) > 1)
				epage = atoi(dash + 1);
			else
				epage = pagecount;
		}

		spage = fz_clampi(spage, 1, pagecount);
		epage = fz_clampi(epage, 1, pagecount);

		if (spage < epage)
			for (page = spage; page <= epage; page++)
				drawpage(ctx, doc, page);
		else
			for (page = spage; page >= epage; page--)
				drawpage(ctx, doc, page);

		spec = fz_strsep(&range, ",");
	}
}

static void drawoutline(fz_context *ctx, fz_document *doc)
{
	fz_outline *outline = fz_load_outline(doc);
	fz_output *out = NULL;

	fz_var(out);
	fz_try(ctx)
	{
		out = fz_new_output_with_file(ctx, stdout);
		if (showoutline > 1)
			fz_print_outline_xml(ctx, out, outline);
		else
			fz_print_outline(ctx, out, outline);
	}
	fz_always(ctx)
	{
		fz_close_output(out);
		fz_free_outline(ctx, outline);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

static int
parse_colorspace(const char *name)
{
	int i;

	for (i = 0; i < nelem(cs_name_table); i++)
	{
		if (!strcmp(name, cs_name_table[i].name))
			return cs_name_table[i].colorspace;
	}
	fprintf(stderr, "Unknown colorspace \"%s\"\n", name);
	exit(1);
	return -1;
}

int split(char *outputfile, char *filename,char *passWord ,char *page)
{
	char *password = passWord;
	fz_document *doc = NULL;
	int c;
	fz_context *ctx;

	fz_var(doc);

    output = outputfile;

	if (!output )
	{
		printf("nothing to do\n");
		exit(0);
	}

	ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	if (!ctx)
	{
		fprintf(stderr, "cannot initialise context\n");
		exit(1);
	}

	fz_set_aa_level(ctx, alphabits);

	/* Determine output type */
	output_format = OUT_PDF;
    
	{
		int i, j;
        
		for (i = 0; i < nelem(format_cs_table); i++)
		{
			if (format_cs_table[i].format == output_format)
			{
				if (out_cs == CS_UNSET)
					out_cs = format_cs_table[i].default_cs;
				for (j = 0; j < nelem(format_cs_table[i].permitted_cs); j++)
				{
					if (format_cs_table[i].permitted_cs[j] == out_cs)
						break;
				}
				if (j == nelem(format_cs_table[i].permitted_cs))
				{
					fprintf(stderr, "Unsupported colorspace for this format\n");
					exit(1);
				}
			}
		}
	}

	switch (out_cs)
	{
	case CS_MONO:
	case CS_GRAY:
	case CS_GRAYALPHA:
		colorspace = fz_device_gray(ctx);
		break;
	case CS_RGB:
	case CS_RGBA:
		colorspace = fz_device_rgb(ctx);
		break;
	default:
		fprintf(stderr, "Unknown colorspace!\n");
		exit(1);
		break;
	}

	if (output_format == OUT_PDF)
	{
       
        fz_try(ctx)
        {
             fz_context *ctxx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
		     pdfout = pdf_open_document(ctxx, output);
            /*
             load page tree
             */
            pdf_load_page(pdfout, 0);
        }
        fz_catch(ctx)
        {
            pdfout = pdf_create_document(ctx);
        }
      
//       pdfout = pdf_create_document(ctx);
	}

	fz_try(ctx)
	{

		{
			fz_try(ctx)
			{
				fz_try(ctx)
				{
					doc = fz_open_document(ctx, filename);
				}
				fz_catch(ctx)
				{
					fz_rethrow_message(ctx, "cannot open document: %s", filename);
				}

				if (fz_needs_password(doc))
				{
					if (!fz_authenticate_password(doc, password))
						fz_throw(ctx, FZ_ERROR_GENERIC, "cannot authenticate password: %s", filename);
				}

				if (showoutline)
					drawoutline(ctx, doc);

				if ( output )
				{
					if (!isrange(page))
						drawrange(ctx, doc, "1-");
					if (isrange(page))
						drawrange(ctx, doc, page);
				}
				fz_close_document(doc);
				doc = NULL;
			}
			fz_catch(ctx)
			{
				if (!ignore_errors)
					fz_rethrow(ctx);

				fz_close_document(doc);
				doc = NULL;
				fz_warn(ctx, "ignoring error in '%s'", filename);
			}
		}
	}
	fz_catch(ctx)
	{
		fz_close_document(doc);
		fprintf(stderr, "error: cannot draw '%s'\n", filename);
		errored = 1;
	}

	if (pdfout)
	{
		fz_write_options opts = { 0 };
        opts.do_incremental=1;
		pdf_write_document(pdfout, output, &opts);
		pdf_close_document(pdfout);
	}

	fz_free_context(ctx);
	return (errored != 0);
}

#ifdef _MSC_VER
int wmain(int argc, wchar_t *wargv[])
{
	char **argv = fz_argv_from_wargv(argc, wargv);
	int ret = main(argc, argv);
	fz_free_argv(argc, argv);
	return ret;
}
#endif
