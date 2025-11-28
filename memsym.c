#include <stdio.h>
#include <string.h>
#include <unistd.h>
//Iñigo Catalina
//Victor Benito

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
  T_CACHE_LINE cache[NUM_FILAS]; //cache de 8 lineas
  LimpiarCACHE(cache);
  printf("Ver si limppiarcache funciona:\n");
  FILE *fRam = fopen("CONTENTS_RAM.bin", "rb");
  if (!fRam) {
    printf("Fallo al abrir el fichero CONTENTS\n");
    return 0;
  }
  char Simul_RAM[4096];
  fread(Simul_RAM, 1, 4096, fRam);
  fclose(fRam);
  FILE *fAcm = fopen("accesos_memoria.txt", "r");
  if (!fAcm) {
      printf("Fallo al abrir el fichero acceso_memoria\n");
      return 0;
  }
  int direccion;
  while (fscanf(fAcm, "%x", &direccion) == 1) {
        printf("\n==========================\n");
        printf("Acceso a direccion: %04X\n", direccion);
        int ETQ, palabra, linea, bloque;
        ParsearDireccion(direccion, &ETQ, &palabra, &linea, &bloque);
        printf("Parseo -> ETQ=%02X  linea=%d  palabra=%d  bloque=%d\n", ETQ, linea, palabra, bloque);
        if (cache[linea].ETQ != ETQ) {
            printf("---- FALLO DE CACHE----\n");
            TratarFallo(cache, Simul_RAM, ETQ, linea, bloque);
        } else {
            printf("---- ACIERTO DE CACHE ----\n");
        }
        VolcarCACHE(cache);
        sleep(1);
  }
  fclose(fAcm);
  printf("\nFIN. Fallos totales: %d\n", numfallos);
  FILE *fCache = fopen("CONTENTS_CACHE.bin", "wb");
  if (!fCache) {
      printf("Fallo al abrir contents cache\n");
      return -1;
  }
  fwrite(cache, sizeof(T_CACHE_LINE), NUM_FILAS, fCache);
  fclose(fCache);
  return 0;
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
  *ETQ = addr / 128; //5 bits iniciales de la etq
  *palabra = addr % 16; //4 bits del final
  *linea = (addr / 16) % 8;
  *bloque = addr / 16;
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
