 #ifndef HTTP_REQUEST_H
 #define HTTP_REQUEST_H
 
 #include "lwip/apps/http_client.h"
 #include <string.h>
 /*
    enum que define todos tipos de dados e escolhe para qual canal enviar o dado
 */
 typedef enum {
    THINGSPEAK_BREAK=1,
    THINGSPEAK_CURRENT,
    THINGSPEAK_VIBRATION,
    THINGSPEAK_TEMPERATURE,
    THINGSPEAK_HUMIDITY
} thingspeak_type_t;

 /*
    Estrutura que define um dado que deve ser enviado
 */
typedef struct {
    thingspeak_type_t type;
    char value[8];
} queue_entry_t;

 /*
    Função externa que envia o dado
 */
 int thing_send(queue_entry_t data_send);

 #endif
 