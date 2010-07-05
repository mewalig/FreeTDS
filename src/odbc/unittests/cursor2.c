#include "common.h"

/*
 * 1) Test cursor do not give error for statement that do not return rows
 * 2) Test cursor returns results on language RPCs
 */

static char software_version[] = "$Id: cursor2.c,v 1.10 2010/07/05 09:20:32 freddy77 Exp $";
static void *no_unused_var_warn[] = { software_version, no_unused_var_warn };

int
main(int argc, char *argv[])
{
	unsigned char sqlstate[6];
	unsigned char msg[256];

	odbc_connect();

	odbc_command("CREATE TABLE #cursor2_test (i INT)");

	odbc_check_cursor();

	odbc_reset_statement();
	CHKSetStmtAttr(SQL_ATTR_CURSOR_TYPE, (SQLPOINTER) SQL_CURSOR_DYNAMIC, SQL_IS_INTEGER, "S");

	/* this should not fail or return warnings */
	odbc_command("DROP TABLE #cursor2_test");

	CHKGetDiagRec(SQL_HANDLE_STMT, odbc_stmt, 1, sqlstate, NULL, msg, sizeof(msg), NULL, "No");


	odbc_reset_statement();
	odbc_command_with_result(odbc_stmt, "drop proc sp_test");
	odbc_command("create proc sp_test @name varchar(30) as select 0 as pippo select 1 as 'test', @name as 'nome'");

	odbc_reset_statement();
	CHKSetStmtAttr(SQL_ATTR_CURSOR_TYPE, (SQLPOINTER) SQL_CURSOR_STATIC, SQL_IS_INTEGER, "S");

	odbc_command(" exec sp_test 'ciao'");

	CHKFetch("S");

	odbc_reset_statement();
	odbc_command("drop proc sp_test");

	odbc_disconnect();
	
	return 0;
}
