#include <cassandra.h>
#include <stdio.h>


CassCluster* cluster;
CassSession* session;
CassFuture* connect_future;
CassError rc;
CassFuture* query_future;
char* cassandra_serv_ip = "155.230.91.212"; // contact points


/* Setup and Connect Cassandra*/
void setup_cassandra_driver() {
    /* Setup and connect to cluster */
    cluster = cass_cluster_new();
    session = cass_session_new();

    /* Add contact points */
    cass_cluster_set_contact_points(cluster, cassandra_serv_ip);

    /* Provide the cluster object as configuration to connect the session */
    connect_future = cass_session_connect(session, cluster);

    /* This operation will block until the result is ready */
    rc = cass_future_error_code(connect_future);
    printf("Connect result: %s\n", cass_error_desc(rc));

}



/* Run queries...insert */
void execute_cassandra_insert(char* query_string) {

    /* Create a statement with zero parameters */
    CassStatement* statement = cass_statement_new(query_string, 0);
    query_future = cass_session_execute(session, statement);

    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);

    /* This will block until the query has finished */
    rc = cass_future_error_code(query_future);
    printf("Query result: %s\n", cass_error_desc(rc));

}


/* Terminate Cassandra Driver */
void terminate_cassandra_driver() {
    cass_future_free(query_future);
    cass_future_free(connect_future);
    cass_session_free(session);
    cass_cluster_free(cluster);
}


int main() {

    /* Setup and Connect Cassandra*/
    setup_cassandra_driver();
    /* Run queries... */
    execute_cassandra_insert("insert into university.student(rollno, name, dept) values(3, 'third', 'Math' )");
    /* Terminate Cassandra Driver */
    terminate_cassandra_driver();


    return 0;
}

