#include "protocol.h"

int sockfd;
struct sockaddr_in server_addr, client_addr;

// Subscriber record structure
typedef struct {
    char subscriber_no[11];
    unsigned char technology;
    int paid;
} subscriber_record;

// Database of subscribers
subscriber_record subscribers[100];
int num_subscribers = 0;

// Read the verification database
void read_verification_database(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening database file");
        exit(EXIT_FAILURE);
    }
    
    char line[100];
    while (fgets(line, sizeof(line), file) && num_subscribers < 100) {
        char sub_no[11];
        int tech, paid;
        
        if (sscanf(line, "%s %d %d", sub_no, &tech, &paid) == 3) {
            strcpy(subscribers[num_subscribers].subscriber_no, sub_no);
            subscribers[num_subscribers].technology = tech;
            subscribers[num_subscribers].paid = paid;
            num_subscribers++;
        }
    }
    
    fclose(file);
    printf("Loaded %d subscribers from database\n", num_subscribers);
    
    // Display loaded subscribers for verification
    printf("\n=== Subscriber Database ===\n");
    for (int i = 0; i < num_subscribers; i++) {
        printf("Subscriber: %s, Technology: %d, Paid: %d\n", 
              subscribers[i].subscriber_no, 
              subscribers[i].technology,
              subscribers[i].paid);
    }
    printf("==========================\n\n");
}

// Verify subscriber access
int verify_subscriber(const char *subscriber_no, unsigned char technology) {
    for (int i = 0; i < num_subscribers; i++) {
        if (strcmp(subscribers[i].subscriber_no, subscriber_no) == 0) {
            printf("Found subscriber %s in database\n", subscriber_no);
            
            // Subscriber exists, check technology
            if (subscribers[i].technology != technology) {
                printf("Technology mismatch: Expected %d, got %d\n", 
                      subscribers[i].technology, technology);
                return Not_exist; // Technology mismatch
            }
            
            // Check payment status
            if (subscribers[i].paid == 0) {
                printf("Subscriber has not paid\n");
                return Not_paid; // Not paid
            } else {
                printf("Subscriber authorized\n");
                return Acc_ok; // All good
            }
        }
    }
    
    printf("Subscriber %s not found in database\n", subscriber_no);
    return Not_exist; // Subscriber not found
}

int main() {
    socklen_t client_len = sizeof(client_addr);
    struct access_request_packet request;
    struct server_response_packet response;
    
    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    
    // Bind socket
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server started on port %d...\n", SERVER_PORT);
    
    // Read verification database
    read_verification_database("Verification_Database.txt");
    
    while (1) {
        printf("\nWaiting for access request...\n");
        
        // Receive access request packet
        ssize_t n = recvfrom(sockfd, &request, sizeof(struct access_request_packet), 0,
                           (struct sockaddr*)&client_addr, &client_len);
        
        if (n < 0) {
            perror("Receive failed");
            continue;
        }
        
        // Print received packet info
        print_access_request_info(&request);
        
        // Verify subscriber
        int result = verify_subscriber(request.subscriber_no, request.technology);
        
        // Create and send response
        create_server_response(&response, request.client_id, request.seg_no,
                             result, request.subscriber_no, request.technology);
        
        if (sendto(sockfd, &response, sizeof(struct server_response_packet), 0,
                 (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0) {
            perror("Send response failed");
        }
        
        print_server_response_info(&response);
    }
    
    close(sockfd);
    return 0;
}