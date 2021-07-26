#pragma once

namespace logger
{
	const char* const JOURNAL_TRACE = "Trace";
	const char* const JOURNAL_SYSTEM = "System";
	const char* const JOURNAL_SECURITY = "Security";
	const char* const JOURNAL_APPLICATION = "Application";

////////////// Default column names

	const char* const COLUMN_CompName =		"cmp";
	const char* const COLUMN_UserName =		"usr";
	const char* const COLUMN_NameOfSource =	"src";
	const char* const COLUMN_Body =			"body";
	const char* const COLUMN_Extended =		"ext";
	const char* const COLUMN_Flag =			"flag";
	const char* const COLUMN_LogName =			"log";

// key - ID для классификации сообщений
	enum MsgId
	{
		msgEXCEPTION			= 1,
		msgSTART_APPLICATION	= 2,
		msgSTOP_APPLICATION		= 3
	};

}
