#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdint.h>

#define internal static
#define global static
#define local static

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef u32 b32;

typedef float r32;

#define SOCKET_PORT 27015
#define ADDRESS_FAMILY_IPV4 AF_INET
#define STREAM_SOCKET SOCK_STREAM
#define IP_PROTOCOL_TCP IPPROTO_TCP

#define DEFAULT_BUFFER_LEN 512

struct request_header
{
    char name[32];
    char value[256];
};

struct request
{
    char method[14];
    char uri[256];
    char protocolVersion[32];
    
    u32 headersCount;
    request_header headers[64];
};

struct response
{
    char protocolVersion[32];
    u16 statusCode;
    char statusReason[64];
};

int StringLength(char *str)
{
    int result = 0;
    while(str[result] != '\0') result++;
    return result;
}

int FirstIndexOf(char *str, char delimiter)
{
    int result = 0;
    int len = StringLength(str);
    while(str[result] != delimiter)
    {
        if(str[result] == '\0' && result == len) return 0;
        result++;
    }
    return result;
}


void mem_set(void *buffer, char v, size_t count)
{
    char *set = (char *)buffer;
    for(u64 index = 0; index < count; ++index)
    {
        *(set + index) = (int)v;
    }
}

void mem_copy(void *to, void *from, size_t count)
{
    for(u64 index = 0; index < count; ++index)
    {
        *((char *)to + index) = *((char *)from + index);
    }
}

int main(void)
{
	WSADATA wsaData;
    if(!WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        SOCKET listener = socket(ADDRESS_FAMILY_IPV4, STREAM_SOCKET, IP_PROTOCOL_TCP);
        if(listener != INVALID_SOCKET)
        {
            sockaddr_in serv;
            serv.sin_family = ADDRESS_FAMILY_IPV4;
            serv.sin_addr.s_addr = inet_addr("127.0.0.1");
            serv.sin_port = htons(SOCKET_PORT);
            if(bind(listener, (SOCKADDR *)&serv, sizeof(serv)) != SOCKET_ERROR)
            {
                if(listen(listener, SOMAXCONN) != SOCKET_ERROR)
                {
                    int receivedResult = 0;
                    char receivedBuffer[DEFAULT_BUFFER_LEN];
                    char *workingBuffer;
                    SOCKET client = accept(listener, 0, 0);
                    if(client != INVALID_SOCKET)
                    {
                        u32 lineRow = 0;
                        request Request = {};
                        b32 resumeRead = false;
                        for(;;)
                        {
                            mem_set(receivedBuffer, 0, DEFAULT_BUFFER_LEN);
                            receivedResult = recv(client, receivedBuffer, DEFAULT_BUFFER_LEN, 0);
                            if(receivedResult > 0)
                            {
                                workingBuffer = receivedBuffer;
                                //note(jarrett): check if the first line of the HTTP header is setup
                                if(StringLength(Request.protocolVersion) == 0)
                                {
                                    int index = FirstIndexOf(receivedBuffer, ' ');
                                    mem_copy(Request.method, workingBuffer, index);
                                    
                                    workingBuffer += (index + 1);
                                    index = FirstIndexOf(workingBuffer, ' ');
                                    
                                    mem_copy(Request.uri, workingBuffer, index);
                                    
                                    workingBuffer += (index + 1);
                                    //todo(jarrett): make this to be able to support multi-chars?
                                    index = FirstIndexOf(workingBuffer, '\r\n');
                                    
                                    mem_copy(Request.protocolVersion, workingBuffer, index);
                                    lineRow++;
                                    workingBuffer += (index + 2);
                                }
                                
#if 1
                                int index = FirstIndexOf(workingBuffer, ':');
                                while(index > 0)
                                {
                                    request_header *header;
                                    if(resumeRead)
                                    {
                                        header = Request.headers + (Request.headersCount - 1);
                                    }
                                    else
                                    {
                                        header = Request.headers + Request.headersCount++;
                                        
                                        mem_copy(header->name, workingBuffer, index);
                                        
                                        workingBuffer += (index + 2);
                                    }
                                    
                                    //index = FirstIndexOf(workingBuffer, '\r\n');
                                    
                                    index = FirstIndexOf(workingBuffer, '\r\n');
                                    lineRow++;
                                    if(index == 0)
                                    {
                                        mem_copy(header->value, workingBuffer, StringLength(workingBuffer));
                                        
                                        resumeRead = true;
                                        break; //todo(jarrett): store line so we can resume from it
                                    }
                                    else if(resumeRead && index > 0)
                                    {
                                        u32 resumePoint = StringLength(header->value);
                                        for(int bufferIndex = 0; bufferIndex < index; ++bufferIndex)
                                        {
                                            header->value[resumePoint + bufferIndex] = workingBuffer[bufferIndex];
                                        }
                                        
                                        workingBuffer += (index + 2);
                                        resumeRead = false;
                                    }
                                    else
                                    {
                                        mem_copy(header->value, workingBuffer, index);
                                        workingBuffer += (index + 2);
                                        resumeRead = false;
                                    }
                                    
                                    //workingBuffer += (index + 2);
                                    
                                    index = FirstIndexOf(workingBuffer, ':');
                                }
#endif
                                
                                int j = 0;
                            }
                            else if(receivedResult == 0)
                            {
                                break;
                            }
                            else
                            {
                                break;
                            }
                            
                            if(receivedResult < DEFAULT_BUFFER_LEN)
                            {
                                
#if 1
                                mem_set(receivedBuffer, 0, DEFAULT_BUFFER_LEN);
                                mem_copy(receivedBuffer, "HTTP/1.1 200 OK \r\n", 64);
                                //first-line (status-line): "HTTP/1.1 200 OK CRLF"
                                
                                int sendResult = send(client, receivedBuffer, StringLength(receivedBuffer), 0);
                                if(sendResult != SOCKET_ERROR)
                                {
                                    int i = 0;
                                }
                                int t = WSAGetLastError();
                                int x = 0;
                                shutdown(client, SD_SEND);
#endif
                            }
                        }
                        
                        
                        
                        closesocket(client);
                    }
                }
                int i = 0;
            }
            
            closesocket(listener);
        }
    }
    else
    {
        //todo(jarrett): logging
    }
    
    WSACleanup();
	return(0);
}
