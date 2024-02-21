�����ѡ��ʹ��socket�⣬����ʹ�����º�����ʵ������ͨ�ţ�
- socket()������һ��socket
- bind()����IP��ַ�Ͷ˿ں�
- listen()�������˿ںţ��ȴ���������
- accept()�����ܿͻ����������󣬽�������
- connect()�����ӵ�ָ���ķ�����IP�Ͷ˿ں�
- send()��������Ϣ
- recv()��������Ϣ
- close()���ر�����
        ʾ�����룺
```c++
#include
#include
#include
#include
#include
#include
int main() {
// ����socket
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        std::cerr << "socket() failed" << std::endl;
        return -1;
    }
// ��IP��ַ�Ͷ˿ں�
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = bind(sock, (sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        std::cerr << "bind() failed" << std::endl;
        close(sock);
        return -1;
    }
// �����˿ں�
    ret = listen(sock, 5);
    if (ret < 0) {
        std::cerr << "listen() failed" << std::endl;
        close(sock);
        return -1;
    }
// �ȴ���������
    sockaddr_in clientAddr{};
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSock = accept(sock, (sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSock < 0) {
        std::cerr << "accept() failed" << std::endl;
        close(sock);
        return -1;
    }
// ������Ϣ
    char buf[1024] = {0};
    ssize_t recvSize = recv(clientSock, buf, sizeof(buf)-1, 0);
    if (recvSize < 0) {
        std::cerr << "recv() failed" << std::endl;
        close(sock);
        close(clientSock);
        return -1;
    }
// ������Ϣ
    const char* msg = "Hello, world!";
    ssize_t sendSize = send(clientSock, msg, strlen(msg), 0);
    if (sendSize < 0) {
        std::cerr << "send() failed" << std::endl;
        close(sock);
        close(clientSock);
        return -1;
    }
// �ر�����
    close(clientSock);
    close(sock);
    return 0;
}
```
�����ѡ��ʹ��boost::asio�⣬����ʹ����������ʵ������ͨ�ţ�
- boost::asio::io_context�������¼��������첽I/O����
- boost::asio::ip::tcp::acceptor�����ڼ����˿ںţ��ȴ���������
- boost::asio::ip::tcp::socket�����ڽ������ӡ�������Ϣ��������Ϣ�Ȳ���
        ʾ�����룺
```c++
#include
#include
void onConnect(const boost::system::error_code& error) {
    if (error) {
        std::cerr << "connect() failed: " << error.message() << std::endl;
        return;
    }
    std::cout << "Connected!" << std::endl;
}
void onSend(const boost::system::error_code& error, size_t bytesTransferred) {
    if (error) {
        std::cerr << "send() failed: " << error.message() << std::endl;
        return;
    }
    std::cout << "Sent " << bytesTransferred << " bytes" << std::endl;
}
void onRecv(const boost::system::error_code& error, size_t bytesTransferred) {
    if (error) {
        std::cerr << "recv() failed: " << error.message() << std::endl;
        return;
    }
    std::cout << "Received " << bytesTransferred << " bytes" << std::endl;
}
int main() {
// ����io_context
    boost::asio::io_context ioContext;
// ����acceptor�������˿ں�
    boost::asio::ip::tcp::acceptor acceptor(ioContext);
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 8888);
    acceptor.open(endpoint.protocol());
    acceptor.bind(endpoint);
    acceptor.listen();
// �ȴ���������
    boost::asio::ip::tcp::socket socket(ioContext);
    acceptor.async_accept(socket, [](const boost::system::error_code& error) {
        if (error) {
            std::cerr << "accept() failed: " << error.message() << std::endl;
            return;
        }
        std::cout << "Accepted!" << std::endl;
// ������Ϣ
        char buf[1024] = {0};
        socket.async_receive(boost::asio::buffer(buf, sizeof(buf)-1), onRecv);
// ������Ϣ
        const char* msg = "Hello, world!";
        socket.async_send(boost::asio::buffer(msg, strlen(msg)), onSend);
    });
// ���ӵ�ָ���ķ�����IP�Ͷ˿ں�
    boost::asio::ip::tcp::socket clientSocket(ioContext);
    boost::asio::ip::tcp::endpoint clientEndpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8888);
    clientSocket.async_connect(clientEndpoint, onConnect);
// ����io_context
    ioContext.run();
    return 0;
}