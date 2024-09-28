#define _GNU_SOURCE

#include "../Enthalten/include.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sched.h>
#include <string.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <unistd.h>

// sudo su
// gcc test.c -o ../Ausführung/test
// ../Ausführung/test

#define COMMAND_EXIT "exit"
#define COMMAND_CREATE_NETWORK_NAMESPACE "createns"

#define NETNAMESPACE_DICTIONARY "/var/run/netns"
#define SIGN_SINGLE_SPLIT '-'
#define INFORM_INDEX "%3d"
#define INFORM_NAME "%80s"

#define MAX_COMMAND 2
// // 執行指定的命令
int execute_command(char *command[]) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed.");
        return FAILED;
    }

    if (pid == 0) {
        execvp(command[0], command);
        perror("Execvp failed");
        exit(EXIT_FAILURE);
    } else {
        int status_code;
        waitpid(pid, &status_code, 0);
        return WIFEXITED(status_code) ? WEXITSTATUS(status_code) : FAILED;
    }
}

void print_split_line(char sign){
    struct winsize window_size;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size) == FAILED){
        perror("IOCTL failed.");
        exit(EXIT_FAILURE);
    }

    int width = window_size.ws_col;

    for(int index=0;index<width;index++)
        putchar(sign);
    
    putchar('\n');
}

void print_network_namespaces(int* status_code){
    printf("Network Namespace :\n");
    char *command[] = {"ip", "netns", "list", NULL};    // If remove NULL will happen "Execvp failed: Bad address"
    print_split_line(SIGN_SINGLE_SPLIT);
    *status_code = execute_command(command);
}


// // 設置 veth 連接
// int setup_veth_pair(const char *veth1, const char *veth2) {
//     char *command[] = {"ip", "link", "add", (char *)veth1, "type", "veth", "peer", "name", (char *)veth2, NULL};
//     if (execute_command(command) != 0) return -1;

//     char *up_commands[] = {
//         "ip", "link", "set", (char *)veth1, "up", NULL
//     };
//     if (execute_command(up_commands) != 0) return -1;

//     char *up_commands2[] = {
//         "ip", "link", "set", (char *)veth2, "up", NULL
//     };
//     return execute_command(up_commands2);
// }

// // 設置網路命名空間中的網卡
// int setup_namespace_iface(const char *ns_name, const char *veth, const char *ip_addr) {
//     char *command[] = {
//         "ip", "netns", "exec", (char *)ns_name, "ip", "addr", "add", (char *)ip_addr, "dev", (char *)veth, NULL
//     };
//     return execute_command(command);
// }

// // 配置路由
// int setup_routes(const char *ns_name, const char *veth, const char *gateway) {
//     char *command[] = {
//         "ip", "netns", "exec", (char *)ns_name, "ip", "route", "add", "default", "via", (char *)gateway, "dev", (char *)veth, NULL
//     };
//     return execute_command(command);
// }
#define INFORM_NETWORK_NAMESPACE_CREATED

#define MAX_NET_NAMESPACE 100

char** split_commands(const char* input, int* count){
    char* command_buffer = strdup(input);
    char* command;
    char** commands = malloc(MAX_COMMAND*sizeof(char*));
    *count = 0;

    command = strtok(command_buffer, " ");
    while (command != NULL){
        
        if(*count >= MAX_COMMAND)
            break;

        commands[*count] = malloc((strlen(command)+1)*sizeof(char));
        if(commands[*count] ==NULL){
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        strcpy(commands[*count], command);
        (*count)++;

        command = strtok(NULL, " ");
    }

    free(command_buffer);
    return commands;
}

struct Netnamespace{
    const char* name;
};

struct Netnamespace create_network_namespace(const char *network_namespace_name, int* status_code){
    char *command[] = {"ip", "netns", "add", (char *)network_namespace_name, NULL};
    *status_code = execute_command(command);
    struct Netnamespace netnamespace;
    netnamespace.name = network_namespace_name;
    if(*status_code != 0){
        fprintf(stderr, "Failed to create network namespaces\n");
        exit(EXIT_FAILURE);
    }

#ifdef INFORM_NETWORK_NAMESPACE_CREATED
    printf("Network namespace has created : "INFORM_NAME"\n");
    print_network_namespaces(status_code);
    if(*status_code != 0){
        fprintf(stderr, "List network namespace failed\n");
        exit(EXIT_FAILURE);
    }
#endif

    return netnamespace;
}

void append_new_network_namespace(struct Netnamespace** netnamespaces, int* number_netnamespace, char *network_namespace_name, int* status_code){
    (*network
}

void delete_network_namespace(struct Netnamespace netnamespace, int* status_code){
    char *command[] = {"ip", "netns", "delete", (char *)netnamespace.name, NULL};
    *status_code = execute_command(command);
        if(*status_code != 0){
        fprintf(stderr, "Failed to delete network namespaces\n");
        exit(EXIT_FAILURE);
    }
}

// 主函數
int main() {
    // const char* network_namespace_1 = "network_namespace_1";

    int status_code = 0;
    
    int number_netnamespace = 0;
    struct Netnamespace* netnamespaces = (struct Netnamespace*)malloc(sizeof(struct Netnamespace)*MAX_NET_NAMESPACE);


    // struct Netnamespace netnamespace = create_network_namespace(network_namespace_1, &status_code);

    // print_network_namespaces(&status_code);

    // delete_network_namespace(netnamespace, &status_code);
    // print_network_namespaces(&status_code);

    
    
    while (TRUE)
    {
        char command[MAX_COMMAND_LENGTH];

        printf(">> ");
        scanf("%[^\n]", &command);
        int number_commands = 0;
        char** commands = split_commands(command, &number_commands);

        if(strcmp(commands[0], COMMAND_EXIT) == COMPARE_TRUE){
            for(int index=0;index<number_netnamespace;index++)
                delete_network_namespace(*(netnamespaces+index), &status_code);
            
            free(netnamespaces);
            break;
        }
            
        
        if(strcmp(commands[0], COMMAND_CREATE_NETWORK_NAMESPACE) == COMPARE_TRUE)
            if(number_commands<2){
            }
    }
    printf("a\n");
    
    // if(create_network_namespace(network_namespace_1) != 0){
    //     fprintf(stderr, "Failed to create network namespaces\n");
    //     return EXIT_FAILURE;
    // }
    // // const char* network_namespace_2 = "network_namespace_2";
    // // const char* network_namespace_3 = "network_namespace_3";
    
    // if(delete_network_namespace(network_namespace_1) != 0){
    //     fprintf(stderr, "Failed to delete network namespaces\n");
    //     return EXIT_FAILURE;
    // }

    // // 網路命名空間和網卡名稱
    // const char *ns1 = "ns4";
    // const char *ns2 = "ns5";
    // const char *ns3 = "ns6";
    // const char *veth1 = "veth5";
    // const char *veth2 = "veth6";
    // const char *veth3 = "veth7";
    // const char *veth4 = "veth8";

    // // 創建網路命名空間
    // if (create_network_namespace(ns1) != 0 ||
    //     create_network_namespace(ns2) != 0 ||
    //     create_network_namespace(ns3) != 0) {
    //     fprintf(stderr, "Failed to create network namespaces\n");
    //     return EXIT_FAILURE;
    // }

    // // 設置 veth 連接
    // if (setup_veth_pair(veth1, veth2) != 0 ||
    //     setup_veth_pair(veth3, veth4) != 0) {
    //     fprintf(stderr, "Failed to setup veth pair\n");
    //     return EXIT_FAILURE;
    // }

    // // 配置網卡和 IP 地址
    // if (setup_namespace_iface(ns1, veth1, "192.168.1.1/24") != 0 ||
    //     setup_namespace_iface(ns2, veth2, "192.168.1.2/24") != 0 ||
    //     setup_namespace_iface(ns2, veth3, "192.168.2.1/24") != 0 ||
    //     setup_namespace_iface(ns3, veth4, "192.168.2.2/24") != 0) {
    //     fprintf(stderr, "Failed to setup network interfaces\n");
    //     return EXIT_FAILURE;
    // }

    // // 配置路由
    // if (setup_routes(ns1, veth1, "192.168.1.2") != 0 ||
    //     setup_routes(ns2, veth2, "192.168.1.1") != 0 ||
    //     setup_routes(ns2, veth3, "192.168.2.2") != 0 ||
    //     setup_routes(ns3, veth4, "192.168.2.1") != 0) {
    //     fprintf(stderr, "Failed to setup routes\n");
    //     return EXIT_FAILURE;
    // }

    // printf("Network namespaces setup successfully.\n");

    // // 測試連接
    // printf("Testing connectivity from ns1 to ns3...\n");

    // pid_t pid = fork();
    // if (pid == 0) {
    //     // 子進程中在 ns1 執行 ping
    //     char *ping_command[] = {"ip", "netns", "exec", (char *)ns1, "ping", "-c", "4", "192.168.2.2", NULL};
    //     execvp("ip", ping_command);
    //     perror("execvp failed");
    //     exit(EXIT_FAILURE);
    // } else {
    //     int status;
    //     waitpid(pid, &status, 0);
    //     if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    //         fprintf(stderr, "Ping test failed\n");
    //         return EXIT_FAILURE;
    //     }
    // }

    // // 刪除網路命名空間
    // if (delete_network_namespace(ns1) != 0 ||
    //     delete_network_namespace(ns2) != 0 ||
    //     delete_network_namespace(ns3) != 0) {
    //     fprintf(stderr, "Failed to delete network namespaces\n");
    //     return EXIT_FAILURE;
    // }

    // printf("Network namespace operations completed successfully\n");
    // return EXIT_SUCCESS;
}
// gcc ./test.c  -o ../Ausführung/test
// ../Ausführung/test
