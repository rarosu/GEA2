#define ZLIB_WINAPI

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unzip.h>
#include <string>

#define dir_delimter '/'
#define MAX_FILENAME 512
#define READ_SIZE 100

int test()
{
	unzFile zipfile = unzOpen("C:\\temp\\asd\\zlibSample\\zlibSample\\asd.zip");
	if ( zipfile == NULL )
    {
        printf( "%s: not found\n" );
        return -1;
    }

	// Get info about the zip file
    unz_global_info global_info;
    if ( unzGetGlobalInfo( zipfile, &global_info ) != UNZ_OK )
    {
        printf( "could not read file global info\n" );
        unzClose( zipfile );
        return -1;
    }

	// Buffer to hold data read from the zip file.
    char read_buffer[ READ_SIZE ];

    // Loop to extract all files
    uLong i;
    for ( i = 0; i < global_info.number_entry; ++i )
    {
        // Get info about current file.
        unz_file_info file_info;
        char filename[ MAX_FILENAME ];
        if ( unzGetCurrentFileInfo(
            zipfile,
            &file_info,
            filename,
            MAX_FILENAME,
            NULL, 0, NULL, 0 ) != UNZ_OK )
        {
            printf( "could not read file info\n" );
            unzClose( zipfile );
            return -1;
		}

        // Check if this entry is a directory or file.
        const size_t filename_length = strlen( filename );
        if ( filename[ filename_length-1 ] == dir_delimter )
        {
            // Entry is a directory, so create it.
            printf( "dir:%s\n", filename );
        }
        else
        {
            // Entry is a file, so extract it.
            printf( "file:%s\n", filename );
            if ( unzOpenCurrentFile( zipfile ) != UNZ_OK )
            {
                printf( "could not open file\n" );
                unzClose( zipfile );
                return -1;
            }

            int error = UNZ_OK;
            do    
            {
                error = unzReadCurrentFile( zipfile, read_buffer, READ_SIZE );
                if ( error < 0 )
                {
                    printf( "error %d\n", error );
                    unzCloseCurrentFile( zipfile );
                    unzClose( zipfile );
                    return -1;
                }

				printf("%s\n", read_buffer);

            } while ( error > 0 );
        }

        unzCloseCurrentFile( zipfile );

        // Go the the next entry listed in the zip file.
        if ( ( i+1 ) < global_info.number_entry )
        {
            if ( unzGoToNextFile( zipfile ) != UNZ_OK )
            {
                printf( "cound not read next file\n" );
                unzClose( zipfile );
                return -1;
            }
        }
    }

    unzClose( zipfile );

	return 0;
}

int main()
{
	test();

	std::cin.get();
	return 1;
}
