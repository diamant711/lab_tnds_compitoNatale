#include "arrayManager.h"

arrayInt* createArrayInt(int len){
    //  Allocazione della struttura per la gestione del vettore nello HEAP
    arrayInt* vettI = new arrayInt;
    //  Salvataggio dell'indirizzo di memoria del vettore effettivo nel campo apposito della struttura
    vettI->raw = new int[len];
    //  Inizializzazione del campo della dimensione del vettore
    vettI->size = len;
    //  Inizializzazione del campo dell'utilizzo del vettore
    vettI->used = 0;
    //  Viene eseguito "return" con il puntatore alla struttura nello HEAP
    return vettI;
}

arrayFloat* createArrayFloat(int len){
    //  Overload della funzione precedente
    arrayFloat* vettF = new arrayFloat;
    vettF->raw = new float[len];
    vettF->size = len;
    vettF->used = 0;
    return vettF;
}

arrayDouble* createArrayDouble(int len){
    //  Overload della funzione precedente
    arrayDouble* vettD = new arrayDouble;
    vettD->raw = new double[len];
    vettD->size = len;
    vettD->used = 0;
    return vettD;
}

void deleteArray(arrayInt* vettI){
    //  De-allocazione del vettore
    delete[] vettI->raw;
    //  De-allocazione della struttura
    delete vettI;
}

void deleteArray(arrayFloat* vettF){
    //  Overload della funzione precedente
    delete[] vettF->raw;
    delete vettF;
}

void deleteArray(arrayDouble* vettD){
    //  Overload della funzione precedente
    delete[] vettD->raw;
    delete vettD;
}

void loadArrayIntFile(const char* nomefile, arrayInt* vettI){
    //  Creazione di una variabile che conterrà il numero di linee/n-uple lette dal file
    int nDati = 0;
    //  Creazione varibile temporanea
    int tmp;
    //  Apertura stream da file e controllo della qualità dello stesso
    ifstream file_in;
    file_in.open(nomefile);
    if(!file_in.good()){
        //  In caso di errori nell'apertura del file il puntatore del vettore nella struttura verrà annullato
        vettI->raw = NULL;
        return;
    }
    //  Implementazione del ciclo Spoletini per il conteggio delle righe del file (saranno salvate in nDati)
    file_in >> tmp;
    while(!file_in.eof()){
        nDati++;
        file_in >> tmp;
    }
    //  Reset dello stream da file
    file_in.clear();
    file_in.seekg(0, ios::beg);
    //  Caricamento di nDati dal file nel vettore gestito dalla struct
    for(int i = 0; i <= nDati; i++)
        file_in >> vettI->raw[i];
    //  Aggiornamento del campo per le celle usate del vettore
    vettI->used = nDati;
    //  Chiusura dello stream da file
    file_in.close();
}

void loadArrayFloatFile(const char* nomefile, arrayFloat* vettF){
    //  Overload della funzione precedente
    int nDati = 0;
    float tmp;
    ifstream file_in;
    file_in.open(nomefile);
    if(!file_in.good()){
        vettF->raw = NULL;
        return;
    }
    file_in >> tmp;
    while(!file_in.eof()){
        nDati++;
        file_in >> tmp;
    }
    file_in.clear();
    file_in.seekg(0, ios::beg);
    for(int i = 0; i <= nDati; i++)
        file_in >> vettF->raw[i];
    vettF->used = nDati;
    file_in.close();
}

void loadArrayDoubleFile(const char* nomefile, arrayDouble* vettD){
    //  Overload della funzione precedente
    int nDati = 0;
    double tmp;
    ifstream file_in;
    file_in.open(nomefile);
    if(!file_in.good()){
        vettD->raw = NULL;
        return;
    }
    file_in >> tmp;
    while(!file_in.eof()){
        nDati++;
        file_in >> tmp;
    }
    file_in.clear();
    file_in.seekg(0, ios::beg);
    for(int i = 0; i <= nDati; i++)
        file_in >> vettD->raw[i];
    vettD->used = nDati;
    file_in.close();
}

void resizeArray(arrayInt* vettI, int newSize){
    //  Se la nuova dimensione è minore o uguale a 0, viene chiamata delete() sulla struttura e termina la funzione
    if(newSize <= 0){
        deleteArray(vettI);
        return;
    }
    //  Allocazione un nuovo vettore dinamico con la nuova dimensione
    int* newRaw = new int[newSize];
    //  Chiamata a memcpy() per la copia byte per byte delle aree di memoria interessate
    memcpy(newRaw, vettI->raw, newSize*sizeof(int));
    //  De-allocazione il vecchio vettore
    delete[] vettI->raw;
    //  Aggiornamento dell'indirizzo di memoria del vettore nella struttura
    vettI->raw = newRaw;
    //  Aggiornamento del nuovo valore di dimensione del vettore nella struttura
    vettI->size = newSize;
    //  Nel caso di un nuovo vettore che abbia "tagliato" dei valori precedenti:
    if(vettI->used > newSize)
        //  Aggiornare il numero di celle utilizzate con il nuovo record ->size della struttura
        vettI->used = newSize;
}

void resizeArray(arrayFloat* vettF, int newSize){
    //  Overload della funzione precedente
    if(newSize <= 0){
        deleteArray(vettF);
        return;
    }
    float* newRaw = new float[newSize];
    memcpy(newRaw, vettF->raw, newSize*sizeof(float));
    delete[] vettF->raw;
    vettF->raw = newRaw;
    vettF->size = newSize;
    if(vettF->used > newSize)
        vettF->used = newSize;
}

void resizeArray(arrayDouble* vettD, int newSize){
    //  Overload della funzione precedente
    if(newSize <= 0){
        deleteArray(vettD);
        return;
    }
    double* newRaw = new double[newSize];
    memcpy(newRaw, vettD->raw, newSize*sizeof(double));
    delete[] vettD->raw;
    vettD->raw = newRaw;
    vettD->size = newSize;
    if(vettD->used > newSize)
        vettD->used = newSize;
}

bool isFull(arrayInt* vettI){
    //  Nel caso in cui il campo size e il campo used siano uguali eseguire "return true"
    return ((vettI->size == vettI->used) ? true:false);
}

bool isFull(arrayFloat* vettF){
    //  Overload della funzione precedente
    return ((vettF->size == vettF->used) ? true:false);
}

bool isFull(arrayDouble* vettD){
    //  Overload della funzione precedente
    return ((vettD->size == vettD->used) ? true:false);
}

bool isEmpty(arrayInt* vettI){
    //  Nel caso in cui il campo used sia uguale a 0 eseguire "return true"
    return ((vettI->used == 0) ? true:false);
}

bool isEmpty(arrayFloat* vettF){
    //  Overload della funzione precedente
    return ((vettF->used == 0) ? true:false);
}

bool isEmpty(arrayDouble* vettD){
    //  Overload della funzione precedente
    return ((vettD->used == 0) ? true:false);
}