#include "common.h"

/*
 * Test originally written by John K. Hohm
 * (cfr "Warning return as copy of last result row (was: Warning: Null value
 * is eliminated by an aggregate or other SET operation.)" July 15th 2006)
 *
 * Contains also similar test by Jeff Dahl
 * (cfr "Warning: Null value is eliminated by an aggregate or other SET 
 * operation." March 24th 2006
 *
 * This test wrong SQLFetch results with warning inside select
 * Is different from raiserror test cause in raiserror error is not
 * inside recordset
 * Sybase do not return warning but test works the same
 */
static char software_version[] = "$Id: warning.c,v 1.9 2008/12/03 12:55:52 freddy77 Exp $";
static void *no_unused_var_warn[] = { software_version, no_unused_var_warn };

static const char one_null_with_warning[] = "select max(a) as foo from (select convert(int, null) as a) as test";

#ifdef TDS_NO_DM
static const int tds_no_dm = 1;
#else
static const int tds_no_dm = 0;
#endif

static void
Test(const char *query)
{
	CHKPrepare((SQLCHAR *) query, SQL_NTS, "S");

	CHKExecute("S");

	CHKFetch("SI");

	CHKFetch("No");

	/*
	 * Microsoft SQL Server 2000 provides a diagnostic record
	 * associated with the second SQLFetch (which returns
	 * SQL_NO_DATA) saying "Warning: Null value is eliminated by
	 * an aggregate or other SET operation."
	 * We check for "NO DM" cause unixODBC till 2.2.11 do not read
	 * errors on SQL_NO_DATA
	 */
	if (db_is_microsoft() && tds_no_dm) {
		SQLCHAR output[256];

		CHKGetDiagRec(SQL_HANDLE_STMT, Statement, 1, NULL, NULL, output, sizeof(output), NULL, "SI");
		printf("Message: %s\n", (char *) output);
	}

	ResetStatement();
}

int
main(void)
{
	Connect();

	Command("CREATE TABLE #warning(name varchar(20), value int null)");
	Command("INSERT INTO #warning VALUES('a', NULL)");

	Test(one_null_with_warning);
	Test("SELECT SUM(value) FROM #warning");

	Disconnect();

	printf("Done.\n");
	return 0;
}

