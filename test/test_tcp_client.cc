#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <vector>
#include <thread>

using byte = unsigned char;

int main(int argc, char *argv[]) {

    if(signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal PIPE ignore failed");
        return EXIT_FAILURE;
    }

    std::vector<std::thread> vec;
    for(int i = 0; i < 100; ++i) {
        vec.emplace_back([](){
            int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

            sockaddr_in addr;
            addr.sin_family = AF_INET;
            inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
            addr.sin_port = htons(9876);
            socklen_t addr_len = sizeof(addr);

            int rt = connect(fd, reinterpret_cast<sockaddr*>(&addr), addr_len);
            if(rt == -1) {
                perror("connect server failed");
            }

            while(true) {
                std::vector<byte> data;
                for(int i = 0; i < 1024 * 1024 * 10 - 1; ++i) {
                    data.push_back('A');
                }
                data.push_back('Z');

                int sz = write(fd, data.data(), data.size());
                if(sz == -1 && errno == EPIPE) {
                    std::this_thread::sleep_for(std::chrono::milliseconds (1000));
                    std::cout << "server close socket..." << std::endl;
                    break;
                }
                std::cout << "write size = " << sz << std::endl;

                std::this_thread::sleep_for(std::chrono::milliseconds (1000));


                int rd_sz = 0;
                std::vector<byte> read_data;
                while(true) {
                    std::vector<byte> read_buf;
                    read_buf.resize(524288);

                    sz = read(fd, read_buf.data(), read_buf.size());
                    rd_sz += sz;
                    read_data.insert(read_data.end(), read_buf.begin(), read_buf.end());

                    if(read_buf[sz - 1] == 'Z') {
                        break;
                    }
                }
                std::cout << "read size = " << rd_sz << std::endl;
            }

            close(fd);
        });
    }

    for(auto &t : vec) {
        t.join();
    }

    return EXIT_SUCCESS;
}
