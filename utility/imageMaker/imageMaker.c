#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define BYTESOFSECTOR 512

// Function define
int adjustInSectorSize( int iFd, int iSourceSize );
void writeKernelInFormation( int iTargetFd, int iTotalKernelSectorCount,
	int iKernel32SectorCount );
int copyFile( int iSourceFd, int iTargetFd );

// Main function
int main( int argc, char* argv[] )
{
	int iSourceFd;
	int iTargetFd;
	int iBootLoaderSize;
	int iKernel32SectorCount;
	int iKernel64SectorCount;
	int iSourceSize;

	// Check option for command line
	if ( argv < 4 )
	{
		fprintf( stderr, "[Error] ImageMaker BootLoader.bin kernel32.bin kernel64.bin\n" );
		exit( -1 );
	}

	// Generate disk.img
	if ( ( iTargetFd = open( "disk.img", O_RDWR | O_CREAT | O_TRUNC
		, S_IREAD | S_IWRITE ) ) == -1 )
	{
		fprintf( stderr, "[Error] %s open fail\n", argv[1] );
		exit( -1 );
	}

	// Open bootloader file and copy it to disk.img
	printf( "[INFO] Copy boot loader to image file\n");
	if ( ( iSourceFd = open( argv[1], O_RDONLY ) ) == -1 )
	{
		fprintf( stderr, "[ERROR] %s open fail\n", argv[1] );
		exit( -1 );
	}

	iSourceSize = copyFile( iSourceFd, iTargetFd );
	close( iSourceFd );

	// To fill rest of memory to 512, set 0x00
	iBootLoaderSize == adjustInSectorSize( iTargetFd, iSourceSize );
	printf( "[INFO] %s size = [%d] and sector count = [%d]\n",
		argv[1], iSourceSize, iBootLoaderSize );

	/*
	 * Copy 32bit kernel to disk image
	 */
	printf( "[INFO] Copy protected mode kernel to image file\n" );
	
	if ( ( iSourceFd = open(argv[2], O_RDONLY ) ) == -1 )
	{
		fprintf( stderr, "[ERROR] %s open fail\n", argv[2] );
		exit( -1 );
	}
	
	iSourceSize = copyFile( iSourceFd, iTargetFd );
	close( iSourceFd );
	
	// To fill rest of memory to 512, set 0x00
	iKernel32SectorCount = adjustInSectorSize( iTargetFd, iSourceSize );
	printf( "[INFO] %s size = [%d] and sector count = [%d]\n",
		argv[2], iSourceSize, iKernel32SectorCount );

	/*
	 * Copy 64bit kernel to disk image
	 */
	printf( "[INFO] Copy IA-32e mode kernel to image file\n" );
	
	if ( ( iSourceFd = open(argv[3], O_RDONLY ) ) == -1 )
	{
		fprintf( stderr, "[ERROR] %s open fail\n", argv[3] );
		exit( -1 );
	}
	
	iSourceSize = copyFile( iSourceFd, iTargetFd );
	close( iSourceFd );
	
	// To fill rest of memory to 512, set 0x00
	iKernel64SectorCount = adjustInSectorSize( iTargetFd, iSourceSize );
	printf( "[INFO] %s size = [%d] and sector count = [%d]\n",
		argv[3], iSourceSize, iKernel64SectorCount );
	
	/*
	 * Update kernel information to disk.img
	 */
	printf( "[INFO] Start to write kernel information\n" );
	// Insert kernel information to 5th section in boot sector
	writeKernelInformation( iTargetFd, iKernel32SectorCount + iKernel64SectorCount , iKernel32SectorCount );
	printf( "[INFO] Imgae file create complete\n" );

	close( iTargetFd );
	return 0;
}


// To fill rest of memory to 512, set 0x00
int adjustInSectorSize( int iFd, int iSourceSize )
{
	int i;
	int iAdjustSizeToSector;
	char cCh;
	int iSectorCount;

	iAdjustSizeToSector = iSourceSize % BYTESOFSECTOR;
	cCh = 0x00;

	if (iAdjustSizeToSector != 0)
	{
		iAdjustSizeToSector = 512 - iAdjustSizeToSector;
		printf( "[INFO] File size [%lu] and fill [%u] byte\n", iSourceSize,
			iAdjustSizeToSector );
		for ( i=0; i < iAdjustSizeToSector; i++)
		{
			write( iFd, &cCh, 1 );
		}
	}
	else 
	{
		printf( "[INFO] File size is aligned 512 byte\n" );
	}

	// Return the number of sector
	iSectorCount = ( iSourceSize + iAdjustSizeToSector ) / BYTESOFSECTOR;
	return iSectorCount;
}

// Insert kernel information to BootLoader
int writeKernelInformation( int iTargetFd, int iTotalKernelSectorCount,
	int iKernel32SectorCount )
{
	unsigned short usData;
	long lPosition;

	// The number of sector is at 5 bytes after from file start
	lPosition = lseek( iTargetFd, 5, SEEK_SET );
	if ( lPosition == -1 )
	{
		fprintf( stderr, "lseek fail. Return value = %d, errno = %d, %d\n",
			lPosition, errno, SEEK_SET );
		exit( -1 );
	}

	// Except for boot loader,
	// Store total sector number & protect sector
	usData = ( unsigned short ) iTotalKernelSectorCount;
	write( iTargetFd, &usData, 2 );
	usData = ( unsigned short ) iKernel32SectorCount;
	write( iTargetFd, &usData, 2 );
	
	printf( "[INFO] Total sector count except boot loader [%d]\n",
		iTotalKernelSectorCount );
	printf( "[INFO] Total sector count protected mode kernel [%d]\n",
		iKernel32SectorCount );
}

// Copy Source FD to Target FD and return the size
int copyFile( int iSourceFd, int iTargetFd )
{
	int iSourceFileSize;
	int iRead;
	int iWrite;
	char vcBuffer[ BYTESOFSECTOR ];

	iSourceFileSize = 0;

	while ( 1 ) 
	{
		iRead = read( iSourceFd, vcBuffer, sizeof ( vcBuffer ) );
		iWrite = write( iTargetFd, vcBuffer, iRead );
		
		if ( iRead != iWrite )
		{
			fprintf( stderr, "[ERROR] iRead != iWrite.. \n" );
			exit( -1 );
		}
		iSourceFileSize += iRead;

		if ( iRead != sizeof ( vcBuffer ) )
		{
			break;
		}
	}
	
	return iSourceFileSize;
}
 
