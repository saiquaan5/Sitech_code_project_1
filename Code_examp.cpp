            if (!notfound){
                if (client_addr.sin_addr.s_addr){
                    string data;
                    // data.begin()
                    std::string data = "s1 " + std::to_string(err_x) + " " + std::to_string(err_y) + '\n';
                    char send_data[data.size()+1]; // Data request control
                    strcpy(send_data, data.c_str());
                    std::cout << send_data << std::endl;
                    sendto(sock, send_data, sizeof(send_data), 0, (struct sockaddr*) &client_addr, sizeof(struct sockaddr));
                }
            } else {
                if (client_addr.sin_addr.s_addr){
                    std::string data = "s0 " + std::to_string(0) + " " + std::to_string(0) + '\n';
                    char send_data[data.size()+1]; // Data request control
                    strcpy(send_data, data.c_str());
                    std::cout << send_data << std::endl;
                    sendto(sock, send_data, sizeof(send_data), 0, (struct sockaddr*) &client_addr, sizeof(struct sockaddr));
                }
            }