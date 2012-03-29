#include "taudb_api.h"
#include "libpq-fe.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

TAUDB_TRIAL* taudb_get_trials(PERFDMF_EXPERIMENT* experiment) {
#ifdef TAUDB_DEBUG
  printf("Calling taudb_get_trials(%p)\n", experiment);
#endif
  PGresult *res;
  int nFields;
  int i, j;

  /*
   * Our test case here involves using a cursor, for which we must be
   * inside a transaction block.  We could do the whole thing with a
   * single PQexec() of "select * from table_name", but that's too
   * trivial to make a good example.
   */

  /* Start a transaction block */
  res = PQexec(_taudb_connection, "BEGIN");
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
  {
    fprintf(stderr, "BEGIN command failed: %s", PQerrorMessage(_taudb_connection));
    PQclear(res);
    taudb_exit_nicely();
  }

  /*
   * Should PQclear PGresult whenever it is no longer needed to avoid
   * memory leaks
   */
  PQclear(res);

  /*
   * Fetch rows from table_name, the system catalog of databases
   */
  char my_query[256];
  sprintf(my_query,"DECLARE myportal CURSOR FOR select * from trial where experiment = %d", experiment->id);
  res = PQexec(_taudb_connection, my_query);
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
  {
    fprintf(stderr, "DECLARE CURSOR failed: %s", PQerrorMessage(_taudb_connection));
    PQclear(res);
    taudb_exit_nicely();
  }
  PQclear(res);

  res = PQexec(_taudb_connection, "FETCH ALL in myportal");
  if (PQresultStatus(res) != PGRES_TUPLES_OK)
  {
    fprintf(stderr, "FETCH ALL failed: %s", PQerrorMessage(_taudb_connection));
    PQclear(res);
    taudb_exit_nicely();
  }

  int nRows = PQntuples(res);
  TAUDB_TRIAL* trials = taudb_create_trials(nRows);
  taudb_numItems = nRows;

  nFields = PQnfields(res);

  /* the rows */
  for (i = 0; i < PQntuples(res); i++)
  {
    int metaIndex = 0;
    trials[i].primary_metadata = taudb_create_primary_metadata(nFields);
    /* the columns */
    for (j = 0; j < nFields; j++) {
	  if (strcmp(PQfname(res, j), "id") == 0) {
	    trials[i].id = atoi(PQgetvalue(res, i, j));
	  } else if (strcmp(PQfname(res, j), "name") == 0) {
	    trials[i].name = PQgetvalue(res, i, j);
	  } else if (strcmp(PQfname(res, j), "date") == 0) {
	    trials[i].collection_date = PQgetvalue(res, i, j);
	  } else if (strcmp(PQfname(res, j), "node_count") == 0) {
	    trials[i].node_count = atoi(PQgetvalue(res, i, j));
	  } else if (strcmp(PQfname(res, j), "contexts_per_node") == 0) {
	    trials[i].contexts_per_node = atoi(PQgetvalue(res, i, j));
	  } else if (strcmp(PQfname(res, j), "threads_per_context") == 0) {
	    trials[i].threads_per_context = atoi(PQgetvalue(res, i, j));
	  } else {
	    trials[i].primary_metadata[metaIndex].name = PQfname(res, j);
	    trials[i].primary_metadata[metaIndex].value = PQgetvalue(res, i, j);
		metaIndex++;
	  }
	} 
	trials[i].primary_metadata_count = metaIndex;
  }

  PQclear(res);

  /* close the portal ... we don't bother to check for errors ... */
  res = PQexec(_taudb_connection, "CLOSE myportal");
  PQclear(res);

  /* end the transaction */
  res = PQexec(_taudb_connection, "END");
  PQclear(res);
  
  return trials;
}

