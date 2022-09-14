#pragma once
#ifndef MODBUSPP_MODBUS_H
#define MODBUSPP_MODBUS_H

#ifdef MYMODBUS_EXPORTS
#define MODBUSCLIENT_API __declspec(dllexport)
#else
#define MODBUSCLIENT_API __declspec(dllimport)
#endif


#include <iostream>
#include <cstring>
#include <stdint.h>
#include <string>
#include <sstream>
#include <tchar.h>

#ifdef ENABLE_MODBUSPP_LOGGING
#include <cstdio>
#define LOG(fmt, ...) printf("[ modbuspp ]" fmt, ##__VA_ARGS__)
#else
#define LOG(...) (void)0
#endif

#ifdef _WIN32
// WINDOWS socket
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <winsock.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
using X_SOCKET = SOCKET;
using ssize_t = int;

#define X_ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define X_CLOSE_SOCKET(s) closesocket(s)
#define X_ISCONNECTSUCCEED(s) ((s) != SOCKET_ERROR)

#else
// Berkeley socket
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using X_SOCKET = int;

#define X_ISVALIDSOCKET(s) ((s) >= 0)
#define X_CLOSE_SOCKET(s) close(s)
#define X_ISCONNECTSUCCEED(s) ((s) >= 0)
#endif

using SOCKADDR = struct sockaddr;
using SOCKADDR_IN = struct sockaddr_in;

#define MAX_MSG_LENGTH 260

///Function Code
#define READ_COILS 0x01
#define READ_INPUT_BITS 0x02
#define READ_REGS 0x03
#define READ_INPUT_REGS 0x04
#define WRITE_COIL 0x05
#define WRITE_REG 0x06
#define WRITE_COILS 0x0F
#define WRITE_REGS 0x10

///Exception Codes

#define EX_ILLEGAL_FUNCTION 0x01 // Function Code not Supported
#define EX_ILLEGAL_ADDRESS 0x02  // Output Address not exists
#define EX_ILLEGAL_VALUE 0x03    // Output Value not in Range
#define EX_SERVER_FAILURE 0x04   // Slave Deive Fails to process request
#define EX_ACKNOWLEDGE 0x05      // Service Need Long Time to Execute
#define EX_SERVER_BUSY 0x06      // Server Was Unable to Accept MB Request PDU
#define EX_NEGATIVE_ACK 0x07
#define EX_MEM_PARITY_PROB 0x08
#define EX_GATEWAY_PROBLEMP 0x0A // Gateway Path not Available
#define EX_GATEWYA_PROBLEMF 0x0B // Target Device Failed to Response
#define EX_BAD_DATA 0XFF         // Bad Data lenght or Address

#define BAD_CON -1

class MODBUSCLIENT_API mymodbus
{

public:
    bool err{};
    int err_no{};
   

    mymodbus(std::string host, uint16_t port);
    ~mymodbus();

    bool modbus_connect();
    void modbus_close() const;

    bool is_connected() const { return _connected; }

    void modbus_set_slave_id(int id);

    int modbus_read_coils(uint16_t address, uint16_t amount, bool* buffer);
    int modbus_read_input_bits(uint16_t address, uint16_t amount, bool* buffer);
    int modbus_read_holding_registers(uint16_t address, uint16_t amount, uint16_t* buffer);
    int modbus_read_input_registers(uint16_t address, uint16_t amount, uint16_t* buffer);

    int modbus_write_coil(uint16_t address, const bool& to_write);
    int modbus_write_register(uint16_t address, const uint16_t& value);
    int modbus_write_coils(uint16_t address, uint16_t amount, const bool* value);
    int modbus_write_registers(uint16_t address, uint16_t amount, const uint16_t* value);
    int modbus_read_registers(uint16_t address, uint16_t amount, uint16_t* buffer);
    std::string GetErr(void);

private:
    bool _connected{};
    uint16_t PORT{};
    uint32_t _msg_id{};
    int _slaveid{};


    X_SOCKET _socket{};
    SOCKADDR_IN _server{};

#ifdef _WIN32
    WSADATA wsadata;
#endif

    void modbus_build_request(uint8_t* to_send, uint16_t address, int func) const;

    int modbus_read(uint16_t address, uint16_t amount, int func);
    int modbus_write(uint16_t address, uint16_t amount, int func, const uint16_t* value);

    ssize_t modbus_send(uint8_t* to_send, size_t length);
    ssize_t modbus_receive(uint8_t* buffer) const;

    void modbuserror_handle(const uint8_t* msg, int func);

    void set_bad_con();
    void set_bad_input();
};


#endif
