// void func(int sockfd)
// {

//     char buff[MAX]; 
//     Json::Value root;
//     Json::Reader reader;

//     len = sizeof(cli); 
//     // Accept the data packet from client and verification 

//     while(true)
//     {

        
//         // // read the message from client and copy it in buffer 
//         int len = read(connfd, buff, sizeof(buff));
//         // // std::cout <<"This is len = "<< len << std::endl;
        
//         if (len == 0) 
//         {
//             close(sockfd);
//             break;
//         }
//         buff[len]= '\0'; 
//         // // print buffer which contains the client contents 
//         // // printf("From client: %s\t To client : ", buff); 
//         // // std::cout<< "From client" << buff << "\t To client: " <<std::endl;
//         if (!strncmp(buff, "PING", 4)) 
//         {
//             write(sockfd, "PONG\n", 5);
//         }
//         if (!strncmp(buff, "6", 1))
//         {
//             send_byte(MSP_EX_GET_STARTUP,1);
//             while(1)
//             {
//                 serial_get_buffer();
//                 if(flag_info)
//                 {
//                     char* data = get_json_update(&sys_info);
//                     if (data)
//                     {
//                         write(sockfd, data, strlen(data));
//                         printf("Update complete\n");
//                         free(data);
//                     }
//                 flag_info = false;
//                 break;
//                 }
//             }
//         }
//      if (!strncmp(buff, "4-manual", 8))
//      {
//          send_byte(MSP_EX_SET_MODE,1);
//          while(1)
//          {
//              serial_get_buffer();
//              if (flag_message)
//              {
//                  if (logcat_buff)
//                  {
//                     write(sockfd,logcat_buff,strlen(logcat_buff));
//                     printf("Update complete\n");
//                     free(logcat_buff);
//                      /* code */
//                  }
//              }
//             flag_message = false;
//             break;
//          }
//      }
        

//         // cout << "Hello" << endl;
//     }
// }