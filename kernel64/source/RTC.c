#include "RTC.h"

// Read current time from CMOS memory
void kReadRTCTime( BYTE * pbHour, BYTE * pbMinute, BYTE * pbSecond )
{
	BYTE bData;
	
	// Select register which store hour in CMOS memory address register(port 0x70)
	kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_HOUR );
	// Read time from CMOS data register(port 0x71)
	bData = kInPortByte( RTC_CMOSDATA );
	*pbHour = RTC_BCDTOBINARY( bData );
		
	// Select register which store minute in CMOS memory address register(port 0x70)
	kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_MINUTE );
	// Read time from CMOS data register(port 0x71)
	bData = kInPortByte( RTC_CMOSDATA );
	*pbMinute = RTC_BCDTOBINARY( bData );
	
	
	// Select register which store second in CMOS memory address register(port 0x70)
	kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_SECOND );
	// Read time from CMOS data register(port 0x71)
	bData = kInPortByte( RTC_CMOSDATA );
	*pbSecond = RTC_BCDTOBINARY( bData );
}

// Read current date from CMOS memory
void kReadRTCDate( WORD * pwYear, BYTE * pbMonth, BYTE * pbDayOfMonth, BYTE * pbDayOfWeek )
{
	BYTE bData;
	
	// Select register which store year in CMOS memory address register(port 0x70)
	kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_YEAR );
	// Read time from CMOS data register(port 0x71)
	bData = kInPortByte( RTC_CMOSDATA );
	*pwYear = RTC_BCDTOBINARY( bData );
		
	// Select register which store month in CMOS memory address register(port 0x70)
	kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_MONTH );
	// Read time from CMOS data register(port 0x71)
	bData = kInPortByte( RTC_CMOSDATA );
	*pbMonth = RTC_BCDTOBINARY( bData );
	
	
	// Select register which store day of month in CMOS memory address register(port 0x70)
	kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_DAYOFMONTH );
	// Read time from CMOS data register(port 0x71)
	bData = kInPortByte( RTC_CMOSDATA );
	*pbDayOfMonth = RTC_BCDTOBINARY( bData );

	// Select register which store day of week in CMOS memory address register(port 0x70)
	kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_DAYOFWEEK );
	// Read time from CMOS data register(port 0x71)
	bData = kInPortByte( RTC_CMOSDATA );
	*pbDayOfWeek = RTC_BCDTOBINARY( bData );
}

// Return the string of day of week 
char * kConvertDayOfWeekToString( BYTE bDayOfWeek )
{
	static char * vpcDayOfWeekString[8] = { "Error", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
	
	// If day is over 8
	if ( bDayOfWeek >= 8 )
	{
		return vpcDayOfWeekString[0];
	}
	
	// Translate
	return vpcDayOfWeekString[bDayOfWeek];
}

