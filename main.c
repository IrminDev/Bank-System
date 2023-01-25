#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/file.h>
#include <string.h>
#include "uuid4.h"
#include <windows.h>

//Estructura para los clientes
struct cliente{
    int idCli;
    char nombre[20];
    char apellido[25];
};

//Estructura para las cuentas
struct cuenta{
    int idCli;
    char idAcc[40];
    int adeudo;
    int credito;
    char pass[20];
};

//Flujos de datos utilizados durante el programa
struct cuenta *accountsFlux = NULL;
struct cliente *clientsFlux = NULL;

//Prototipos de las funciones
void client();
char *genID(char *buf);
void admin();
void clientValidated(int id);
void regDeposit(int id);
void regPurchase(int id);
void printClientAccs(int id);
void saveAccounts(int size);
int clientAuth(char account[40], char pass[20]);
void registerClient();
void registerAccount();
void printClients();
void printAccounts();
int getClients();
void deleteAcc(int id);
int getAccounts();

//Funcion principal
int main(){
    int opc = 0;

    while(opc != 3){
        system("cls"); 
        printf("Escoja una opcion: \n");
        printf("1. Administracion\n");
        printf("2. Cliente\n");
        printf("3. Salir\n");

        scanf("%d", &opc);

        switch (opc){
        case 1:
            admin();
            break;
        case 2:
            client();
            break;
        default:
            break;
        }
    }
}

/// @brief Menu del cliente para el inicio de sesion
void client(){
    system("cls");
    char cuenta[40], pass[20];
    int id = 0;

    //Menu para el login
    printf("Bienvenido, cliente.");
    printf("\nCuenta: ");
    scanf(" %[^\n]", &cuenta);
    printf("\nClave: ");
    scanf(" %[^\n]", &pass);

    //Autenticacion del cliente, si los datos no coinciden el id es 0, caso contrario regresa el id del cliente
    id = clientAuth(cuenta, pass);
    if(id){
        clientValidated(id);
    }
}

/// @brief Menu del cliente validado
/// @param id Es el id del cliente
void clientValidated(int id){
    int opc = 0;

    while(opc != 5){
        system("cls");
        printf("Bienvenido. Seleccione una opcion:");
        printf("\n1. Registrar deposito.");
        printf("\n2. Registrar compra.");
        printf("\n3. Ver cuentas.");
        printf("\n4. Eliminar cuenta.");
        printf("\n5. Salir\n");
        scanf(" %d", &opc);
        switch (opc)
        {
        case 1:
            regDeposit(id);
            break;
        case 2:
            regPurchase(id);
            break;
        case 3:
            system("cls");
            printClientAccs(id);
            system("pause");
            break;
        case 4:
            deleteAcc(id);
            break;
        default:
            break;
        }
    }
}

/// @brief Elimina una cuenta del cliente
/// @param id Es el id del cliente que quiere eliminar su cuenta
void deleteAcc(int id){
    char acc[40];
    system("cls");
    printf("Para eliminar una cuenta su saldo debe ser 0.\n");
    printClientAccs(id);
    printf("\nInserte la cuenta que quiere eliminar: ");
    scanf(" %[^\n]", &acc);
    int size = getAccounts(), opc = 0;
    //Iteramos las cuentas del flujo para checar cada una
    for (int i = 0; i < size; i++){
        //Si la cuenta, el id del client coinciden y ademas el adeudo es 0, se continua con la eliminacion
        if(strcmp(accountsFlux[i].idAcc, acc) == 0 && accountsFlux[i].idCli == id && accountsFlux[i].adeudo == 0){
            printf("\nLa cuenta sera eliminada de forma permanente, si desea continuar escibra 1, caso contrario escriba cualquier otro: ");
            scanf("%d", &opc);
            if(opc == 1){
                //For que elimina la posicion i (la que contiene a la cuenta)
                for(int j = i; j<size-1; j++){
                    accountsFlux[j] = accountsFlux[j+1];
                }
                printf("\nCuenta eliminada.\n");
                saveAccounts(size-1);
            }
            else {
                printf("\nNo se elimino la cuenta\n");
            }
            break;
        }
    }
    system("pause");   
}

/// @brief Registra un deposito en alguna de las cuentas del cliente
/// @param id Es el id del cliente que quiere realizar un deposito
void regDeposit(int id){
    char account[40];
    int deposit = 0;
    system("cls");
    printClientAccs(id);
    printf("\nSeleccione una cuenta para hacer un deposito: ");
    printf("\nCuenta: ");
    scanf(" %[^\n]%*c", &account);
    printf("\nDeposito: ");
    scanf(" %d", &deposit);
    int size = getAccounts();
    //Ciclo for que itera cada cuenta del flujo de datos
    for (int i = 0; i < size; i++){
        //Si la cuenta y el id del cliente coinciden, se procede con el flujo
        if(strcmp(accountsFlux[i].idAcc, account) == 0 && accountsFlux[i].idCli == id){
            //Comprobamos si el deposito sobrepasa la deuda
            if(accountsFlux[i].adeudo - deposit < 0){
                printf("\nCuenta liquidada, su cambio es: %d\n", deposit - accountsFlux[i].adeudo);
                accountsFlux[i].adeudo = 0;
            }
            else{
                //Si el deposito es menor o igual a 0 no se realiza.
                if(deposit <= 0){
                    printf("\nNo se pudo realizar el deposito.\n");
                } else {
                    printf("\nDeposito realizado. Su nuevo adeudo es: %d\n", accountsFlux[i].adeudo - deposit);
                    accountsFlux[i].adeudo -= deposit;
                }
            }
            //Guardamos los cambios
            saveAccounts(size);
            break;
        }
    }

    free(accountsFlux);
    system("pause");
}

/// @brief Funcion para registrar una compra del usuario a una cuenta
/// @param id es el id del usuario que realizara la compra
void regPurchase(int id){
    char account[40];
    int purchase = 0, m = 0;

    system("cls");
    printClientAccs(id);
    int size = getAccounts();
    printf("\nSeleccione una cuenta para registrar una compra: ");
    printf("\nCuenta: ");
    scanf("%s", &account);
    printf("\nMonto de compra: ");
    scanf(" %d", &purchase);
    printf("\nMeses para pagar (0/3/6/9): \n");
    scanf(" %d", &m);

    //Switch que determina el interes que sera aplicado al monto de compra
    switch (m){
    case 0:
        break;
    case 3:
        purchase *= 1.06;
        break;
    case 6:
        purchase *= 1.12;
        break;
    case 9:
        purchase *= 1.18;
        break;
    default:
        printf("No es aplicable el plazo.\n");
        purchase = 0;
        break;
    }

    //Si la compra es un valor menor o igual a 0 no se realiza la compra, caso contrario la registra
    if (purchase <= 0 ){
        printf("No se registro la compra.\n");
    }
    else{
        //For que itera cada cuenta del flujo de datos
        for (int i = 0; i < size; i++){
            //Si la cuenta y el id coinciden, se procede en el flujo
            if(strcmp(accountsFlux[i].idAcc, account) == 0 && accountsFlux[i].idCli == id){
                //Si la compra excede el credito no se registra la compra, caso contrario la aplica
                if(accountsFlux[i].credito < accountsFlux[i].adeudo + purchase){
                    printf("No es posible registrar la comprar. El monto excedio el credito.\n");
                }
                else{
                    accountsFlux[i].adeudo += purchase;
                    printf("Compra registrada.\n");
                    saveAccounts(size);
                }
                break;
            }
        }
    }

    free(accountsFlux);
    system("pause");   
}

/// @brief Menu para los administradores
void admin(){
    int opc = 0;
    while(opc != 5){
        system("cls");
        printf("Bienvenido, administrador.");
        printf("\n1. Registrar cliente");
        printf("\n2. Registrar cuenta");
        printf("\n3. Ver clientes");
        printf("\n4. Ver cuentas");
        printf("\n5. Salir\n");
        scanf(" %d", &opc);

        switch (opc){
        case 1:
            registerClient();
            break;
        case 2:
            registerAccount();
            break;
        case 3:
            printClients();
            system("pause");
            break;
        case 4:
            printAccounts();
            break;
        default:
            break;
        }
    }
}

/// @brief Funcion que imprime las cuentas de los clientes
/// @param id Id del cliente al que le pertenecen las cuentas
void printClientAccs(int id){
    int size = getAccounts();
    //Ciclo for que itera las cuentas
    for (int i = 0; i < size; i++){
        //Comprueba si la cuenta le pertenece al cliente
        if(accountsFlux[i].idCli == id){
            printf("\nCuenta: %s", accountsFlux[i].idAcc);
            printf("\nCredito: %d", accountsFlux[i].credito);
            printf("\nAdeudo: %d\n", accountsFlux[i].adeudo);
        }
    }

    free(accountsFlux);
}

/// @brief Funcion que guarda las cuentas
/// @param size es el tamaño del arreglo que va a guardar
void saveAccounts(int size){
    FILE *db = fopen("accounts.txt", "w");
    if(db == NULL){
        perror("Error al guardar los cambios.");
    } else {
        //Ciclo for que escribe en el archivo una posicion del arreglo en el formato dado
        for (int i = 0; i < size; i++){
            fprintf(db, "%d|%s|%d|%d|%s|\n", accountsFlux[i].idCli, accountsFlux[i].idAcc, accountsFlux[i].adeudo, accountsFlux[i].credito, accountsFlux[i].pass);
        }
        fflush(db);
        fclose(db);
    }
}

/// @brief Funcion que hace la autenticacion de la informacion del cliente en el inicio de sesion
/// @param account es una cuenta del cliente
/// @param passw es la contraseña de la cuenta
/// @return  id del cliente, o 0 si no coinciden los datos
int clientAuth(char account[40], char passw[20]){
    int id = 0, size  = getAccounts();
    char aux[20] = "";
    //for que itera el flujo de datos
    for (int i = 0; i < size; i++){
        //Compara si las cuentas coinciden
        if(strcmp(accountsFlux[i].idAcc, account) == 0){
            //Compara si las contrasenas coinciden
            if(strcmp(accountsFlux[i].pass, passw) == 0){
                id = accountsFlux[i].idCli;
            } else {
                printf("\nClave incorrecta\n");
                system("pause");        
            }
            break;
        } 
    }

    free(accountsFlux);
    return id;   
}

/// @brief Funcion que genera un uuid
/// @param buf es el buffer en el que se trabaja el uuid
/// @return la direccion de memorias del uuid
char *genID(char *buf){
    buf = (char *)malloc(UUID4_LEN * sizeof(char));
    uuid4_init();
    uuid4_generate(buf);
    return buf;
}

/// @brief Funcion que registra un cliente
void registerClient(){
    char name[20], lname[25];
    int id, size;
    //Obtencion del flujo de datos de clientes
    size = getClients();

    printf("\nInserte el nombre del cliente: ");
    scanf(" %s", &name);
    printf("\nInserte el apellido del cliente: ");
    scanf(" %s", &lname);

    //Si el tamaño del flujo es 0, significa que es el primer cliente registrado, se le asigna el id=1
    if(size == 0){
        id = 1;
    }
    //Caso contrario asigna el id siguiente
    else{
        id = clientsFlux[size-1].idCli + 1;
    }

    //Abrimos un archivo para añadir el nuevo cliente
    FILE *saveFlux = fopen("clients.txt", "a");
    if(saveFlux == NULL){
        perror("\nError al escribir en el archivo.\n");
        system("pause");
    }
    else{
        char aux[50];
        fprintf(saveFlux, "%d|%s|%s|\n", id, name, lname);
        fflush(saveFlux);
        fclose(saveFlux);

        printf("\nCliente guardado\n");
        system("pause");
    }
}

/// @brief Funcion que registra una cuenta, asignada para un cliente
void registerAccount(){
    printClients();
    char pass[20], *uuid;
    int idCli, credit, size;
    size = getClients();
    uuid = genID(uuid);

    printf("\nInserte el id del cliente: ");
    scanf(" %d", &idCli);
    printf("\nInserte el credito solicitado: ");
    scanf(" %d", &credit);
    printf("\nInserte la clave de la cuenta: ");
    scanf(" %s", &pass);

    //Comprobamos si el id al que asigno la cuenta si exitse
    if(idCli > clientsFlux[size-1].idCli || idCli <= 0){
        printf("\nNo se puede asignar la cuenta.\n");
        system("pause");
    } else {
        FILE *saveFlux = fopen("accounts.txt", "a");
        if(saveFlux == NULL){
            perror("\nError al escribir en el archivo.\n");
            system("pause");
        }
        else{
            fprintf(saveFlux, "%d|%s|%d|%d|%s|\n", idCli, uuid, credit, credit, pass);
            fflush(saveFlux);
            fclose(saveFlux);

            printf("\nCuenta guardada.");
            printf("\nSu cuenta es: %s\n", uuid);
            system("pause");
        }
    }
}

/// @brief Funcion que imprime los clientes registrados
void printClients(){
    system("cls");
    int size = getClients();
    //Se comprueba si existen clientes
    if(size == 0){
        printf("\nNo hay clientes registrados.\n");        
    } else {
        //For que itera cada uno de los clientes
        for (int i = 0; i < size; i++){
            printf("\nId de cliente: %d", clientsFlux[i].idCli);
            printf("\nNombre: %s", clientsFlux[i].nombre);
            printf("\nApellido: %s\n", clientsFlux[i].apellido);
        }
    }
    free(clientsFlux);
}

/// @brief Funcion que imprime las cuentas registradas
void printAccounts(){
    system("cls");
    int size = getAccounts();
    //Se comprueba si existen cuentas
    if(size == 0){
        printf("\nNo hay cuentas registradas.\n");
    }
    else{
        //For que itera cada una de las cuentas
        for (int i = 0; i < size; i++){
            printf("\nCuenta: %s", accountsFlux[i].idAcc);
            printf("\nId del cliente: %d", accountsFlux[i].idCli);
            printf("\nAdeudo de la cuenta: %d", accountsFlux[i].adeudo);
            printf("\nCredito de la cuenta: %d", accountsFlux[i].credito);
            printf("\nContrasena: %s\n", accountsFlux[i].pass);
        }
    }

    free(accountsFlux);
    system("pause");
}

/// @brief Funcion que obtiene el flujo de datos de los clientes
/// @return el tamaño del arreglo recuperado
int getClients(){
    FILE *db = fopen("clients.txt", "rt");
    struct cliente *auxClientsFlux = NULL;
    int index = 0, tmp = 0;
    char aux[5];
    if(db == NULL){
        perror("Error al abrir el archivo. Pero tranquilo, no fue tu culpa.");
        db = fopen("clients.txt", "w");
        fclose(db);
        return 0;
    } else {
        //Se crea un espacio en la memoria para el arreglo
        clientsFlux = (struct cliente *)malloc(sizeof(struct cliente));
        //Mientras cada linea tenga los 3 datos y los guarda
        while(fscanf(db, "%d%*c %[^|]%*c %[^|]%*c", &clientsFlux[index].idCli, &clientsFlux[index].nombre, &clientsFlux[index].apellido) == 3){
            //El index indica cuantos clientes hay
            index++;
            //Tmp indica el nuevo tamaño de memoria del arreglo, se agrega un espacio del arreglo para la iteracion siguiente
            tmp = (index+1)*sizeof(struct cliente);
            //Se hace un realloc con el nuevo tamaño de memoria, modificando el arreglo
            auxClientsFlux = (struct cliente *)realloc(clientsFlux, tmp);
            //Comprobamos si el realloc sucedio bien
            if(auxClientsFlux == NULL){
                free(clientsFlux);
                printf("\nError al asignar la memoria. Pero tranquilo, no fue tu culpa");
                return 0;
            }
            else {
                clientsFlux = auxClientsFlux;
            }
        }

        //Antes de terminar el while se reserva un espacio de mas, por ello se recorta en una unidad el arreglo
        tmp -= sizeof(struct cliente);
        auxClientsFlux = realloc(clientsFlux, tmp);
        //Se comprueba si el realloc funcionó bien
        if(auxClientsFlux == NULL){
            free(clientsFlux);
            return 0;
        }
        else {
            clientsFlux = auxClientsFlux;
        }

        fclose(db);
    }

    //Regresa el tamaño del arreglo (el valor total de la memoria del arreglo dividido por el tamaño de la estrcutura)
    return tmp/sizeof(struct cliente);
}

/// @brief Funcion que obtiene el flujo de datos de las cuentas
/// @return El tamaño del arreglo recuperado
int getAccounts(){
    FILE *db = fopen("accounts.txt", "rt");
    struct cuenta *auxAccountsFlux = NULL;
    int index = 0, idcli, adeudo, credito;
    int tmp = 0;
    char pass[20], idacc[40];
    if(db == NULL){
        perror("Error al abrir el archivo. Pero tranquilo, no fue tu culpa");
        db = fopen("accounts.txt", "w");
        fclose(db);
        return tmp;
    } else {
        //Se reserva un espacio de memoria para una cuenta
        accountsFlux = (struct cuenta *)malloc(sizeof(struct cuenta));
        //Mientras se escaneen 5 datos por cada linea y se guardan
        while(fscanf(db, "%d%*c %[^|]%*c %d%*c %d%*c %[^|]%*c", &accountsFlux[index].idCli, &accountsFlux[index].idAcc, &accountsFlux[index].adeudo, &accountsFlux[index].credito, &accountsFlux[index].pass) == 5){
            //El index indica el numero de registros que hay, se le agrega uno en cada scaneo exitoso
            index++;
            //tmp indica la memoria del arreglo, se redefine para cada iteracion y agrega un espacio mas para el arreglo para la proxima iteracion
            tmp = (index+1)*sizeof(struct cuenta);
            //se realiza el realloc con la nueva memoria
            auxAccountsFlux = (struct cuenta *)realloc(accountsFlux, tmp);
            //se comprueba que el realloc haya funcionado bien
            if(auxAccountsFlux == NULL){
                free(accountsFlux);
                printf("\nError al asignar la memoria. Pero tranquilo, no fue tu culpa");
                return 0;
            }
            else{
                accountsFlux = auxAccountsFlux;
            }
        }

        //Una vez terminado el while, sobra un espacio de memoria reservado por si habia una nueva iteracion, este espacio es borrado, reduciendo a tmp
        tmp -= sizeof(struct cuenta);
        //Se hace el realloc con el nuevo tamaño
        auxAccountsFlux = realloc(accountsFlux, tmp);
        if(auxAccountsFlux == NULL){
            free(accountsFlux);
            return 0;
        }
        else{
            accountsFlux = auxAccountsFlux;
        }

        fclose(db);
    }

    //Retorna el tamaño del arreglo, que es el espacio total del arreglo entre el tamaño de la estructura de la cuenta
    return tmp/sizeof(struct cuenta);
}