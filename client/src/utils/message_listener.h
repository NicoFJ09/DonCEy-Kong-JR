#ifndef MESSAGE_LISTENER_H
#define MESSAGE_LISTENER_H

#include "../network/connection.h"
#include <pthread.h>
#include <stdbool.h>

// se supone que aquí se debe de agregar que se quiere hacer con ese mensaje. Al en otro thread aparte se hixo este 
//message listener para no bloquear el hilo principal al estar esperando mensajes del servidor


pthread_t message_listener_start(Connection* conn);

void message_listener_stop(pthread_t thread_id);

#endif
