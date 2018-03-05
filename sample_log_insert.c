#include <cassandra.h>
#include <stdio.h>
#include <string.h>
#define MAX_LOG_SIZE 2000


CassCluster* cluster;
CassSession* session;
CassFuture* connect_future;
CassError rc;
CassFuture* query_future;
char* cassandra_serv_ip = "155.230.91.212"; // contact points

void split_log(char* log, char log_split_list[][MAX_LOG_SIZE]) {
    char log_cp[MAX_LOG_SIZE];
    char * split;     
    int count = 0;

    strcpy(log_cp, log);  // copy string
    strcat(log_split_list[12],"");
    strcat(log_split_list[15],"");

    split = strtok(log_cp, " ");
    while (split != NULL)               // ìë¥¸ ë¬¸ìì´ì´ ëì¤ì§ ìì ëê¹ì§ ë°ë³µ
    {
        count ++;

        if(count >= 10 && strcmp(log_split_list[10], "NON_SOCKET")==0) { // socket write/read ê° ìë ë -> 12 ì´ì íëì ë´ê¸°

            if(count >= 12) { // ì´ì  ë¤ì ë¤ ëí´ì£¼ì.
                strcat(log_split_list[12], split); // ë·ë¬¸ìì´ ëí´ì£¼ê¸°
                strcat(log_split_list[12], " "); // ê³µë°± ë¤ì ë£ì´ì¤ì¼í¨
            }else {
                strcpy(log_split_list[count], split); // ìë¥¸ ë¬¸ìì´ì ììëë¡ ë°°ì´ì ì ì¥.
            }

        }else { // socket ì¼ ë -> 15 ì´ì íëì ë´ê¸°

            if(count >= 15) { // ì´ì  ë¤ì ë¤ ëí´ì£¼ì.
                strcat(log_split_list[15], split); // ë·ë¬¸ìì´ ëí´ì£¼ê¸°
                strcat(log_split_list[15], " "); // ê³µë°± ë¤ì ë£ì´ì¤ì¼í¨
            }else {
                strcpy(log_split_list[count], split); // ìë¥¸ ë¬¸ìì´ì ììëë¡ ë°°ì´ì ì ì¥.
            }
        }

        split = strtok(NULL, " ");      // ë¤ì ë¬¸ìì´ì ìë¼ì í¬ì¸í°ë¥¼ ë°í
    }// end of while



}


char* get_insert_query_string(char* log, char* insert_query_string) {
    /*
       1 - ìììê°(start_time),
       2 - ìììê°(turnaround_time),
       3 - íë¡ì¸ì¤ì´ë¦(proc_name),
       4 - (host_name),
       5 - proc_id,
       6 - thread_id,
       7 - call_name,
       8 - size, 150(ë°ì´í¸ ì (ìëì í¨ìì ë¦¬í´ê°. readì ê²½ì° ì½ì ë°ì´í°ì í¬ê¸°))
       9 - fd, 8( read() ì¸ìë¡ ëì´ì¨ file descriptor)
       10 - sock_type : NON_SOCKET ì¸ì§ AF_INET ì¸ì§
       if(NON_SOCKET) {
       11 - fd_path : (/proc/10007/auxv íì)
       12 - content : ëë¨¸ì§ ë´ì© ì ë¶... ì ì¥íì.
       }else {
       11 - send_ip :
       12 - send_port :
       13 - recv_ip :
       14 - recv_port :
       15 - ë¶í°ë ëª¨ë content ì ì ì¥
       }
     */


    //    char insert_query_string[MAX_LOG_SIZE]="";
    char split_list[16][MAX_LOG_SIZE]; // 1~15 ê¹ì§ ì¬ì©
    char pre[MAX_LOG_SIZE] = "insert into mongodb.write(";
    char col_name_string[MAX_LOG_SIZE] = "start_time, turnaround_time, proc_name, host_name, proc_id, thread_id, sys_call, buffer_size, fd, sock_type, "; // 1 ~ 10ë² ê¹ì§ ëì¼, ë¤ì ë ì¶ê°í´ì¤ì¼í¨!

    char mid[MAX_LOG_SIZE]  = ") values(";
    char value_string[MAX_LOG_SIZE] = "";
    char post[MAX_LOG_SIZE]  = ")";
    int i;

    split_log(log, split_list); // ë°°ì´ì ë¨ì´ ë¨ìë¡ ì ì¥ëìë¤.

    /* "insert into university.student(rollno, name, dept) values(2, 'woong', 'Computer' )" ì ê°ì ííì ë¬¸ìì´ì´ ìì±ëì´ì¼ íë¤.*/
    for(i=1; i<=15; i++) {

        if(i==1 || i==2 || i==5 || i==8 || i==9){ // 1ë², 2ë², 5ë², 8ë², 9ë² ì ì«ì, ëë¨¸ì§ ë¬¸ìì´ë¡ ì ì¥
            strcat(value_string, split_list[i]);
        }else {
            //ë¬¸ìì´ì ' ë¡ ê°ì¸ì¼í¨.
            strcat(value_string, "'");
            strcat(value_string, split_list[i]);
            strcat(value_string, "'");
        }

        if( i>=12 && strcmp(split_list[10], "NON_SOCKET") == 0  ) {
            break;

        }else if ( i<15){
            strcat(value_string, ", ");
        }

    }


    if( strcmp(split_list[10], "NON_SOCKET") == 0  ) {
        strcat(col_name_string, "fd_path, content");

    } else {
        strcat(col_name_string, "send_ip, send_port, recv_ip, recv_port, content");
    }



    strcat(insert_query_string, pre);
    strcat(insert_query_string, col_name_string);
    strcat(insert_query_string, mid);
    strcat(insert_query_string, value_string);
    strcat(insert_query_string, post);


    return insert_query_string;
}


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

    char query[MAX_LOG_SIZE] = "";
    char log[MAX_LOG_SIZE] = "1511836300.278119858 0.000003861 mongod triton8 10007 7fdc20afed00 read 16 5 NON_SOCKET /proc/10007/auxv  !............... other other other...";
        
    char log2[MAX_LOG_SIZE] = "1511836318.267066916 0.000004774 mongod triton8 10007 7fdc20afd700 recv 249 30 AF_INET 127.0.0.1 27017 127.0.0.1 58940 ....admin.$cmd..............isMaster......client......application......name.....MongoDB Shell...driver.:....name.....MongoDB Internal Client..version.....3.4.9...os.R....type.....Linux..name.....Ubuntu..architecture.....x86_64..version.....16.04....";

    /* Setup and Connect Cassandra*/
    setup_cassandra_driver();
    /* Run queries... */
   // execute_cassandra_insert("insert into university.student(rollno, name, dept) values(3, 'third', 'Math' )");
    execute_cassandra_insert(get_insert_query_string(log2, query));
    //execute_cassandra_insert(get_insert_query_string(log, query));
    
    /* Terminate Cassandra Driver */
    terminate_cassandra_driver();


    return 0;
}

