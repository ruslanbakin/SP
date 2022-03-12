#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>


#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

void ListIpAddresses() {
    IP_ADAPTER_ADDRESSES* adapter_addresses(NULL);
    IP_ADAPTER_ADDRESSES* adapter(NULL);

    DWORD adapter_addresses_buffer_size = 16 * 1024;

    for (int attempts = 0; attempts != 3; ++attempts) {
        adapter_addresses = (IP_ADAPTER_ADDRESSES*)malloc(adapter_addresses_buffer_size);

        DWORD error = ::GetAdaptersAddresses(AF_UNSPEC,
            GAA_FLAG_SKIP_ANYCAST |
            GAA_FLAG_SKIP_MULTICAST |
            GAA_FLAG_SKIP_DNS_SERVER |
            GAA_FLAG_SKIP_FRIENDLY_NAME,
            NULL,
            adapter_addresses,
            &adapter_addresses_buffer_size);

        if (ERROR_SUCCESS == error) {
            break;
        }
        else if (ERROR_BUFFER_OVERFLOW == error) {
            free(adapter_addresses);
            adapter_addresses = NULL;
            continue;
        }
        else {
            free(adapter_addresses);
            adapter_addresses = NULL;
            return;
        }
    }

    for (adapter = adapter_addresses; NULL != adapter; adapter = adapter->Next) {
        if (IF_TYPE_SOFTWARE_LOOPBACK == adapter->IfType) continue;

        printf("[ADAPTER]: %S\n", adapter->Description);
        printf("[NAME]:    %S\n", adapter->FriendlyName);

        for (IP_ADAPTER_UNICAST_ADDRESS* address = adapter->FirstUnicastAddress; NULL != address; address = address->Next) {
            auto family = address->Address.lpSockaddr->sa_family;
            if (AF_INET == family) {
                SOCKADDR_IN* ipv4 = reinterpret_cast<SOCKADDR_IN*>(address->Address.lpSockaddr);
                char str_buffer[16] = { 0 };
                inet_ntop(AF_INET, &(ipv4->sin_addr), str_buffer, 16);

                printf("[IP]:      %s\n", str_buffer);
            }
        }
        printf("\n");
    }

    free(adapter_addresses);
    adapter_addresses = NULL;
}

int main() {
    std::string s;
    std::cout << "what is your name?\n";
    std::cin >> s;
    std::cout << "Hello, " + s<<".";
    std::cout << "\ncurrent ip:\n";
    ListIpAddresses();
    return 0;
}
