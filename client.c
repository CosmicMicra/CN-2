#include "protocol.h"

int sockfd;
struct sockaddr_in server_addr;
unsigned char client_id = 1;

// Send access request with timer and retry mechanism
int send_access_request_with_timer(struct access_request_packet *packet) {
    struct server_response_packet response;
    socklen_t server_len = sizeof(server_addr);
    int retries = 0;
    
    // Set up timer
    struct timeval tv;
    tv.tv_sec = ACK_TIMEOUT;
    tv.tv_usec = 0;
    
    while (retries < MAX_RETRIES) {
        // Set socket timeout
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
            perror("Error setting timeout");
            return -1;
        }
        
        printf("\nSending Access Request (Attempt %d):\n", retries + 1);
        print_access_request_info(packet);
        
        // Send packet
        if (sendto(sockfd, packet, sizeof(struct access_request_packet), 0, 
                 (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("Send failed");
            return -1;
        }
        
        // Wait for response
        int recv_result = recvfrom(sockfd, &response, sizeof(struct server_response_packet), 0, 
                                 (struct sockaddr*)&server_addr, &server_len);
        
        if (recv_result < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                printf("Timeout for access request (Attempt %d)\n", retries + 1);
                retries++;
                
                if (retries == MAX_RETRIES) {
                    printf("Server does not respond\n");
                    return -1;
                }
                continue;
            }
            perror("Receive failed");
            return -1;
        }
        
        // Print received response
        print_server_response_info(&response);
        
        // Check response type
        if (response.type == Acc_ok) {
            printf("\n*** Access granted for subscriber %s ***\n", packet->subscriber_no);
        } else if (response.type == Not_paid) {
            printf("\n*** Access denied: Subscriber %s has not paid ***\n", packet->subscriber_no);
        } else if (response.type == Not_exist) {
            printf("\n*** Access denied: Subscriber %s does not exist or technology mismatch ***\n", 
                  packet->subscriber_no);
        }
        
        return response.type;
    }
    
    return -1;
}

int main() {
    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Test cases
    struct access_request_packet request;
    
    printf("=== Testing Access Verification Protocol ===\n");
    
    // Test Case 1: Valid subscriber with paid status
    printf("\nTest Case 1: Valid subscriber with paid status\n");
    create_access_request(&request, client_id, 1, "4085546805", 4);
    send_access_request_with_timer(&request);
    sleep(1);
    
    // Test Case 2: Valid subscriber with unpaid status
    printf("\nTest Case 2: Valid subscriber with unpaid status\n");
    create_access_request(&request, client_id, 2, "4086668821", 3);
    send_access_request_with_timer(&request);
    sleep(1);
    
    // Test Case 3: Valid subscriber with wrong technology
    printf("\nTest Case 3: Valid subscriber with wrong technology\n");
    create_access_request(&request, client_id, 3, "4085546805", 3); // Technology should be 4
    send_access_request_with_timer(&request);
    sleep(1);
    
    // Test Case 4: Subscriber does not exist
    printf("\nTest Case 4: Subscriber does not exist\n");
    create_access_request(&request, client_id, 4, "4081234567", 5);
    send_access_request_with_timer(&request);
    sleep(1);
    
    
    
    close(sockfd);
    return 0;
}