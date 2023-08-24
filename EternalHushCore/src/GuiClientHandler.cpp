#include <Windows.h>
#include <Handlers.h>
#include <vector>
#include <iostream>

TcpServer server;

typedef struct _TASK_OBJECT {
    std::string name;
    std::vector<std::string> args;

} TASK_OBJECT, * PTASK_OBJECT;

void GuiClientHandler(TcpServer data) {

    int client_sz = sizeof(data.c_sin);
    data.c_socket = accept(data.s_socket, (sockaddr*)&data.c_sin, &client_sz);

    TASK_OBJECT recv_data;

    while (data.terminate != true)
    {

        ULONG recv_size = recv(data.c_socket, (char *)&recv_data, sizeof(recv_data), 0);
       
        if (recv_size > NULL)
        {
            printf("%s\n", recv_data.name);
            for (int i = 0; i < recv_data.args.size(); i++) {
                printf("%s\n", recv_data.args.at(i));

            }
        }
    }
}

void start_gui_handler() {
    server.callback = GuiClientHandler;
    server.setup(4444);
    server.start();
}

void stop_gui_handler() {
    server.terminate = true;
    shutdown(server.c_socket, 2);
    shutdown(server.s_socket, 2);
}