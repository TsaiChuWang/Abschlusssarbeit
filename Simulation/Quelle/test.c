#define _GNU_SOURCE

#include "../Enthalten/include.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sched.h>
#include <string.h>

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
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }
}

// // 創建網路命名空間
// int create_network_namespace(const char *net_namespace) {
//     char *command[] = {"ip", "netns", "add", (char *)net_namespace, NULL};
//     return execute_command(command);
// }

// int delete_network_namespace(const char *net_namespace){
//     char *command[] = {"ip", "netns", "delete", (char *)net_namespace, NULL};
//     return execute_command(command);
// }

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

// // 刪除網路命名空間
// int delete_network_namespace(const char *ns_name) {
//     char *command[] = {"ip", "netns", "delete", (char *)ns_name, NULL};
//     return execute_command(command);
// }

#define MAX_NET_NAMESPACE 100

struct Netnamespace{
    const char* name;
};

struct Netnamespace create_network_namespace(const char *net_namespace_name, int* status_code){
    char *command[] = {"ip", "netns", "add", (char *)net_namespace_name, NULL};
    *status_code = execute_command(command);
    struct Netnamespace netnamespace;
    netnamespace.name = net_namespace_name;
    if(*status_code != 0){
        fprintf(stderr, "Failed to create network namespaces\n");
        exit(EXIT_FAILURE);
    }
    return netnamespace;
}

void delete_network_namespace(struct Netnamespace netnamespace, int* status_code){
    char *command[] = {"ip", "netns", "delete", (char *)netnamespace.name, NULL};
    *status_code = execute_command(command);
        if(*status_code != 0){
        fprintf(stderr, "Failed to delete network namespaces\n");
        exit(EXIT_FAILURE);
    }
}

void 

// 主函數
int main() {
    const char* net_namespace_1 = "net_namespace_1";

    int status_code = 0;
    struct Netnamespace netnamespace = create_network_namespace(net_namespace_1, &status_code);


    delete_network_namespace(netnamespace, &status_code);

    // if(create_network_namespace(net_namespace_1) != 0){
    //     fprintf(stderr, "Failed to create network namespaces\n");
    //     return EXIT_FAILURE;
    // }
    // // const char* net_namespace_2 = "net_namespace_2";
    // // const char* net_namespace_3 = "net_namespace_3";
    
    // if(delete_network_namespace(net_namespace_1) != 0){
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
