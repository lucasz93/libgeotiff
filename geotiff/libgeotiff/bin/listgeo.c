/*
 * listgeo.c -- example client code for LIBGEO geographic
 *     TIFF tag support. Dumps info to GeoTIFF metadata file.
 *
 *  Author: Niles D. Ritter
 *
 */

#include "geotiff.h"
#include "xtiffio.h"
#include "geo_normalize.h"
#include <stdio.h>

static const char *CSVFileOverride( const char * );
static const char *CSVDirName = NULL;

int main(int argc, char *argv[])
{
    char	*fname = "newgeo.tif";
    TIFF 	*tif=(TIFF*)0;  /* TIFF-level descriptor */
    GTIF	*gtif=(GTIF*)0; /* GeoKey-level descriptor */
    int		i, norm_print_flag = 0;

    /*
     * Handle command line options.
     */
    for( i = 1; i < argc; i++ )
    {
        if( strcmp(argv[i],"-norm") == 0 )
            norm_print_flag = 1;
        else if( strcmp(argv[i],"-t") == 0 )
        {
            CSVDirName = argv[++i];
            SetCSVFilenameHook( CSVFileOverride );
        }
        else if( strcmp(fname,"newgeo.tif") == 0 )
            fname = argv[i];
        else
        {
            printf( "Usage: listgeo [-norm] [-t tabledir] [filename]\n" );
            exit( 1 );
        }
    }

    /*
     * Open the file, read the GeoTIFF information, and print to stdout. 
     */

    tif=XTIFFOpen(fname,"r");
    if (!tif) goto failure;
	
    gtif = GTIFNew(tif);
    if (!gtif)
    {
        fprintf(stderr,"failed in GTIFNew\n");
        goto failure;
    }
	
    /* dump the GeoTIFF metadata to std out */

    GTIFPrint(gtif,0,0);

    /*
     * Capture, and report normalized information if requested.
     */

    if( norm_print_flag )
    {
        GTIFDefn	defn;
        
        if( GTIFGetDefn( gtif, &defn ) )
        {
            printf( "\n" );
            GTIFPrintDefn( &defn, stdout );

#ifdef notdef
            printf( "PROJ.4 String = `%s'\n",
                    GTIFGetProj4Defn( &sDefn ) );
#endif
        }
    }
    
    GTIFFree(gtif);
    XTIFFClose(tif);
    return 0;
		
  failure:
    fprintf(stderr,"failure in listgeo\n");
    if (tif) TIFFClose(tif);
    if (gtif) GTIFFree(gtif);
    return 1;
}

static const char *CSVFileOverride( const char * pszInput )

{
    static char		szPath[1024];

#ifdef WIN32
    sprintf( szPath, "%s\\%s", CSVDirName, pszInput );
#else    
    sprintf( szPath, "%s/%s", CSVDirName, pszInput );
#endif    

    return( szPath );
}