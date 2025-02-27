#include "protocol.h"



void create_data_packet(struct data_packet *packet, 
                       unsigned char client_id,
                       unsigned short seg_no, 
                       const char *payload) {
    // Set packet identifiers
    packet->start_id = START_PACKET;
    packet->end_id = END_PACKET;
    
    // Set packet metadata
    packet->client_id = client_id;
    packet->type = DATA_PACKET;
    packet->seg_no = seg_no;
    
    // Set payload
    packet->length = strlen(payload);
    if (packet->length > MAX_LENGTH) {
        packet->length = MAX_LENGTH;
    }
    strncpy(packet->payload, payload, packet->length);
    packet->payload[packet->length] = '\0';
}

void create_ack_packet(struct ack_packet *packet,
                      unsigned char client_id,
                      unsigned char seg_no) {
    // Set packet identifiers
    packet->start_id = START_PACKET;
    packet->end_id = END_PACKET;
    
    // Set packet metadata
    packet->client_id = client_id;
    packet->type = ACK_PACKET;
    packet->seg_no = seg_no;
}



void create_access_request(struct access_request_packet *packet,
                          unsigned char client_id,
                          unsigned char seg_no,
                          const char *subscriber_no,
                          unsigned char technology) {
    packet->start_id = START_PACKET;
    packet->client_id = client_id;
    packet->type = Acc_per;
    packet->seg_no = seg_no;
    packet->length = strlen(subscriber_no);
    strncpy(packet->subscriber_no, subscriber_no, 10);
    packet->subscriber_no[10] = '\0';
    packet->technology = technology;
    packet->end_id = END_PACKET;
}

void create_server_response(struct server_response_packet *packet,
                           unsigned char client_id,
                           unsigned char seg_no,
                           unsigned short type,
                           const char *subscriber_no,
                           unsigned char technology) {
    packet->start_id = START_PACKET;
    packet->client_id = client_id;
    packet->type = type;
    packet->seg_no = seg_no;
    packet->length = strlen(subscriber_no);
    strncpy(packet->subscriber_no, subscriber_no, 10);
    packet->subscriber_no[10] = '\0';
    packet->technology = technology;
    packet->end_id = END_PACKET;
}





int validate_packet(const struct data_packet *packet) {
    if (packet->start_id != START_PACKET) {
        printf("Invalid start packet identifier\n");
        return 0;
    }
    
    if (packet->end_id != END_PACKET) {
        printf("Invalid end packet identifier\n");
        return 0;
    }
    
    // Check client ID range
    if (packet->client_id > MAX_CLIENT_ID) {
        printf("Invalid client ID\n");
        return 0;
    }
    
    // Check length field
    if (packet->length > MAX_LENGTH) {
        printf("Invalid packet length\n");
        return 0;
    }
    
    return 1;  // Packet is valid
}

void print_packet_info(const struct data_packet *packet) {
    printf("\n=== Packet Information ===\n");
    printf("Start ID: 0x%X\n", packet->start_id);
    printf("Client ID: %d\n", packet->client_id);
    printf("Type: 0x%X\n", packet->type);
    printf("Segment Number: %d\n", packet->seg_no);
    printf("Length: %d\n", packet->length);
    printf("Payload: %s\n", packet->payload);
    printf("End ID: 0x%X\n", packet->end_id);
    printf("=======================\n\n");
}


void print_access_request_info(const struct access_request_packet *packet) {
    printf("\n=== Access Request Packet Information ===\n");
    printf("Start ID: 0x%X\n", packet->start_id);
    printf("Client ID: %d\n", packet->client_id);
    printf("Type: 0x%X (Access Permission Request)\n", packet->type);
    printf("Segment Number: %d\n", packet->seg_no);
    printf("Length: %d\n", packet->length);
    printf("Subscriber Number: %s\n", packet->subscriber_no);
    printf("Technology: %d%c\n", packet->technology, 'G');
    printf("End ID: 0x%X\n", packet->end_id);
    printf("=====================================\n\n");
}


void print_server_response_info(const struct server_response_packet *packet) {
    printf("\n=== Server Response Packet Information ===\n");
    printf("Start ID: 0x%X\n", packet->start_id);
    printf("Client ID: %d\n", packet->client_id);
    printf("Type: 0x%X (", packet->type);
    
    // Print response type in human-readable form
    if (packet->type == Not_paid)
        printf("Subscriber Not Paid");
    else if (packet->type == Not_exist)
        printf("Subscriber Does Not Exist");
    else if (packet->type == Acc_ok)
        printf("Access OK");
    else
        printf("Unknown");
    
    printf(")\n");
    printf("Segment Number: %d\n", packet->seg_no);
    printf("Length: %d\n", packet->length);
    printf("Subscriber Number: %s\n", packet->subscriber_no);
    printf("Technology: %d%c\n", packet->technology, 'G');
    printf("End ID: 0x%X\n", packet->end_id);
    printf("=====================================\n\n");
}