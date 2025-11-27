#include <stdio.h>
#include <string.h>


#define TAM_LINEA 16
#define NUM_FILAS 8
int globaltime = 0;
int numfallos = 0;

typedef struct {
  unsigned char ETQ;
  unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque);


int main() {
  
}

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]) { //inicializar los campos de la cache
  for(int i = 0; i < NUM_FILAS; i++){
    tbl[i].ETQ = 0xFF;
    for(int j = 0; j < TAM_LINEA; j++){
      tbl[i].Data[j] = 0x23;
    }
  }
}

void VolcarCACHE(T_CACHE_LINE *tbl) {
  for(int i = 0; i < NUM_FILAS; i++){
    printf("Linea %d (ETQ=%02X): ", i, tbl[i].ETQ);
    for(int j = TAM_LINEA - 1; j >= 0; j--){//de menor a mayor peso
      printf("%02X ", tbl[i].Data[j]);
    }
    printf("\n");
  }
}

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque) {
  *ETQ     = addr / 128; //5 bits iniciales de la etq
  *palabra = addr % 16; //4 bits del final
  *linea   = (addr / 16) % 8;
  *bloque  = addr / 16;
}

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque) {
  numfallos++;
  globaltime += 20;
  printf("T: %d, Fallo de CACHE %d, ADDR %04X Label %x linea %02X palabra %02X bloque %02X\n", globaltime, numfallos, (bloque << 4), ETQ, linea, bloque & 0xF, bloque);
  for(int i = 0; i < TAM_LINEA; i++){
    tbl[linea].Data[i] = MRAM[bloque * TAM_LINEA + i];
  }
  tbl[linea].ETQ = ETQ;
  printf("Cargando bloque %02X en línea %02X\n", bloque, linea);
}


