#include <cassandra.h>
#include <stdio.h>

int main() {
      /* Setup and connect to cluster */
      CassCluster* cluster = cass_cluster_new();
        CassSession* session = cass_session_new();

          /* Add contact points */
          cass_cluster_set_contact_points(cluster, "155.230.91.212");

            /* Provide the cluster object as configuration to connect the session */
            CassFuture* connect_future = cass_session_connect(session, cluster);

              /* This operation will block until the result is ready */
              CassError rc = cass_future_error_code(connect_future);

                printf("Connect result: %s\n", cass_error_desc(rc));

                  /* Run queries... */
            
                /* Create a statement with zero parameters */
                CassStatement* statement
                      = cass_statement_new("insert into university.student(rollno, name, dept) values(2, 'woong', 'Computer' )", 0);

                CassFuture* query_future = cass_session_execute(session, statement);

                /* Statement objects can be freed immediately after being executed */
                cass_statement_free(statement);

                /* This will block until the query has finished */
               rc = cass_future_error_code(query_future);

                printf("Query result: %s\n", cass_error_desc(rc));

                cass_future_free(query_future);
                    



                  cass_future_free(connect_future);
                    cass_session_free(session);
                      cass_cluster_free(cluster);

                        return 0;
}


