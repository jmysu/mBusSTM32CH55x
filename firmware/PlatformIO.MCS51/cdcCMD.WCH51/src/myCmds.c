/*
 *  myCmds.c
 *
 *    CH55x CDC CMDs
 * 
 *    by jimmy.su @ 2024
 * 
 */
#include "myCmds.h"


typedef struct {
    const char* cmd;
    const char* helper;
    void (*function)();
  } cmdDictionary;
//=============================================================================
const cmdDictionary tableCmds[]= {
    {"help",    "show this help!",                  cmdHelp },
    {"reset",   "reset me!",                        cmdReset},
    {"bl",      "jump boootloader!",                cmdBL},
    {"p1w",     "write P1(1~7), p1w <x=<1|0>",      cmdP1W},
    {"p1r",     "read P1(1~7)",                     cmdP1R},
    {"eew",     "write EE(00~7F), eew <xx=xx>",     cmdEEW},
    {"eer",     "read EE",                          cmdEER},
    /*---------------------------------------------------*/
    {"?",       "show this help!",              cmdHelp}
    };
//=============================================================================

//=============================================================================
/*
SFR(P1_MOD_OC,	0x92);	// port 1 output mode: 0=push-pull, 1=open-drain
SFR(P1_DIR_PU,	0x93);	// port 1 direction for push-pull or pullup enable for open-drain
// Pn_MOD_OC & Pn_DIR_PU: pin input & output configuration for Pn (n=1/3)
//   0 0:  float input only, without pullup resistance
//   0 1:  push-pull output, strong driving high level and low level
//   1 0:  open-drain output and input without pullup resistance
//   1 1:  quasi-bidirectional (standard 8051 mode), open-drain output and input with pullup resistance, just driving high level strongly for 2 clocks if turning output level from low to high
 */
void cmdP1W(){
    int pin,v;

    pin=atoi(cmdParam(1));
    v=atoi(cmdParam(2));
    
    
    if ((pin>0) && (pin<7)) { 
        //pin output
        P1_MOD_OC &= ~(1<<(pin&7));
        P1_DIR_PU |= (1<<(pin&7));

        P1 &= ~(1<<pin);
        P1 |=  (v<<pin);
        }
    PRINTF("Pin%d:%d[0x%02X]\n", pin, v, P1);
}
/*
SFR(P1_MOD_OC,	0x92);	// port 1 output mode: 0=push-pull, 1=open-drain
SFR(P1_DIR_PU,	0x93);	// port 1 direction for push-pull or pullup enable for open-drain
// Pn_MOD_OC & Pn_DIR_PU: pin input & output configuration for Pn (n=1/3)
//   0 0:  float input only, without pullup resistance
//   0 1:  push-pull output, strong driving high level and low level
//   1 0:  open-drain output and input without pullup resistance
//   1 1:  quasi-bidirectional (standard 8051 mode), open-drain output and input with pullup resistance, just driving high level strongly for 2 clocks if turning output level from low to high
 */
void cmdP1R(){
    PRINTF("P1.7654321\n");
    PRINTF("----------\n");
    PRINTF("P1:");
    P1_MOD_OC=0xFF;
    P1_DIR_PU=0xFF;
    for (int i=7;i>0;i--){
        PRINTF("%c", P1&(1<<i)?'1':'0');
    }
    PRINTF("\n");
}


void cmdHelp(){
 char iCmdCount = sizeof(tableCmds)/sizeof(cmdDictionary);
  
    PRINTF("\ncmd HELP\n");
    PRINTF("________________________\n");
    for (int i=0;i<iCmdCount;i++) {
        PRINTF("\033["ANSI_YELLOW"m"); 
        PRINTF("%s", tableCmds[i].cmd);
        PRINTF("\033[0m");
        PRINTF("\t%s\n", tableCmds[i].helper);
        }
    PRINTF("________________________\n");
}

void cmdMatch(char *s) {
  int iCmd;  
  int iCmdCount = sizeof(tableCmds)/sizeof(cmdDictionary);
  bool bCmdOK = false;
 

  //Serial.printf("Matching cmd %s:", sToken.c_str());
  //printf("Matching cmd:%s ",s);
  for (iCmd=0;iCmd<iCmdCount;iCmd++) {
      if (strcmp(s, tableCmds[iCmd].cmd)==0) { //compare s with cmds
          //tableCmds[i].function();
          bCmdOK = true;
          break;
          }
      }
  if (!bCmdOK) {
    //Serial.printf();
    PRINTF("Err: %s no found!\n", s); //8541
    }
  else { //Found cmd
    tableCmds[iCmd].function();
    }  
}

void cmdBL(){
    BOOT_prepare();     //Prepare for jumping to bootloader
    DLY_ms(100);        //Need some delay 
    BOOT_now();         //Jump
}

void cmdReset(){ RST_now();}

//=============================================================================
/* cmdParam(char c)
 *
 *  return the parameter at c
 * 
 */
 
char* cmdParam(char c) {    
    strcpy(buf1, buf); //copy buf
    
    char idx;
    char* token;
    idx=0; 
    //printf("cmdParam:%s\n", buf1);
    // walk through other tokens  
    token = strtok (buf1," =");
    while (token != NULL) {
        //printf ("[%d]:%s\n", idx, token);
        if (idx==c) break;
        idx++;
        token = strtok (NULL, " =");
        }
    //printf("cmdParam%d:%s\n", c, token);
    return token;
}

/*
 * cmdEER()
 *
 *  read internal 128 data flash
 */
void cmdEER() {
    uint8_t i,k;
    PRINTF("\n\033["ANSI_CYAN"m"); 
    PRINTF("CH55x EE:\n");
    for (i=0;i<16;i++) PRINTF("%02X ",i);PRINTF("\n");
    PRINTF("\033[0m");
    for (i=0;i<16;i++) PRINTF("---"); PRINTF("\n");
 
    for (i=0,k=1;i<128;i++) {
        PRINTF("%02X ", eeprom_read_byte(i));
        if ((k++)>15) {PRINTF("\n");k=1;}
        }

    for (i=0;i<16;i++) PRINTF("==="); PRINTF("\n");
}


unsigned int hexToInt(const char *hex)
  {
    unsigned int result = 0;

    while (*hex) {
        if (*hex > 47 && *hex < 58)         /*0123456789*/
          result += (*hex - 48);
        else if (*hex > 64 && *hex < 71)    /*ABCDEF*/
          result += (*hex - 55);
        else if (*hex > 96 && *hex < 103)   /*abcdef*/
          result += (*hex - 87);

        if (*++hex)
          result <<= 4;
        }

    return result;
  }
void cmdEEW() {

    uint8_t addr =  hexToInt(cmdParam(1));
    uint8_t val  =  hexToInt(cmdParam(2));

    if (addr <128)
        eeprom_write_byte(addr, val);

    PRINTF("CH55x EE[%02X]:%02X\n", addr, val );
}