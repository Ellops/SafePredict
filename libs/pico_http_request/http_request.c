/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/async_context.h"
#include "lwip/altcp.h"
#include "lwip/altcp_tls.h"
#include "http_request.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "lwip/init.h"

 #define DEBUG_MODE_HTTP 1

// Send Data to ThingSpeak through http

struct tcp_pcb *tcp_client_pcb;
ip_addr_t server_ip;

#define THINGSPEAK_PORT 80
#define THINGSPEAK_HOST "api.thingspeak.com"

// Callback quando recebe resposta do ThingSpeak
static err_t http_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }
    printf("Resposta do ThingSpeak: %.*s\n", p->len, (char *)p->payload);
    pbuf_free(p);
    return ERR_OK;
}


static err_t http_connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) {
        printf("Erro na conexão TCP\n");
        free(arg);  // Free on error if allocated memory is no longer needed
        return err;
    }

    printf("Conectado ao ThingSpeak!\n");

    queue_entry_t *req = (queue_entry_t *)arg;
    if (req == NULL) {
        printf("Erro: Dados inválidos\n");
        return ERR_ARG;
    }

    char request[256];
    snprintf(request, sizeof(request),
        "GET /update?api_key=%s&field%u=%.3f HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n",
        THINGSPEAK_API_KEY, req->type, atof(req->value), THINGSPEAK_HOST);

    tcp_write(tpcb, request, strlen(request), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    tcp_recv(tpcb, http_recv_callback);

    free(req);  // Free the allocated memory after use

    return ERR_OK;
}


// Resolver DNS e conectar ao servidor
static void dns_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    if (ipaddr) {
        printf("Endereço IP do ThingSpeak: %s\n", ipaddr_ntoa(ipaddr));
        tcp_client_pcb = tcp_new();
        // Set the callback argument so that http_connected_callback receives our data
        tcp_arg(tcp_client_pcb, callback_arg);
        tcp_connect(tcp_client_pcb, ipaddr, THINGSPEAK_PORT, http_connected_callback);
    } else {
        printf("Falha na resolução de DNS\n");
        free(callback_arg);  // Free the allocated memory on DNS failure
    }
}


int thing_send(queue_entry_t data_send) {
    // Allocate memory for the request data to ensure it stays valid
    queue_entry_t *req = malloc(sizeof(queue_entry_t));
    if (!req) {
        printf("Erro: Falha ao alocar memória para a requisição\n");
        return -1;
    }
    *req = data_send;  // Copy the data into our allocated structure

    /* 
     * Initiate DNS resolution of THINGSPEAK_HOST.
     * dns_gethostbyname() will either return:
     *  - ERR_OK: if the IP is already cached/resolved (call dns_callback immediately)
     *  - ERR_INPROGRESS: if the lookup is in progress (dns_callback will be called later)
     *  - an error code otherwise.
     */
    err_t err = dns_gethostbyname(THINGSPEAK_HOST, &server_ip, dns_callback, req);
    if (err == ERR_OK) {
        // Host already resolved, so call the callback directly.
        dns_callback(THINGSPEAK_HOST, &server_ip, req);
    } else if (err != ERR_INPROGRESS) {
        printf("Erro ao resolver DNS: %d\n", err);
        free(req);  // Free the allocated memory since we won't be using it
        return -1;
    }

    return 0;
}