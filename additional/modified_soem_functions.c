

//(c)Arthur Ketels 2010-2012
//Altered function taken from eepromtool.c within SOEM
//Added ebuf pointer as argument - Fraser Gordon 09/08/2019

#include "modified_soem_functions.h"


int eeprom_read(int slave, int start, int length, uint8_t *ebuf)
{
    int i, ainc = 4;
    uint16 estat, aiadr;
    uint32 b4;
    uint64 b8;
    uint8 eepctl;

    if((ec_slavecount >= slave) && (slave > 0) && ((start + length) <= MAXBUF))
    {
        aiadr = 1 - slave;
        eepctl = 2;
        ec_APWR(aiadr, ECT_REG_EEPCFG, sizeof(eepctl), &eepctl , EC_TIMEOUTRET); /* force Eeprom from PDI */
        eepctl = 0;
        ec_APWR(aiadr, ECT_REG_EEPCFG, sizeof(eepctl), &eepctl , EC_TIMEOUTRET); /* set Eeprom to master */

        estat = 0x0000;
        aiadr = 1 - slave;
        ec_APRD(aiadr, ECT_REG_EEPSTAT, sizeof(estat), &estat, EC_TIMEOUTRET); /* read eeprom status */
        estat = etohs(estat);
        if (estat & EC_ESTAT_R64)
        {
            ainc = 8;
            for (i = start ; i < (start + length) ; i+=ainc)
            {
                b8 = ec_readeepromAP(aiadr, i >> 1 , EC_TIMEOUTEEP);
                ebuf[i] = b8 & 0xFF;
                ebuf[i+1] = (b8 >> 8) & 0xFF;
                ebuf[i+2] = (b8 >> 16) & 0xFF;
                ebuf[i+3] = (b8 >> 24) & 0xFF;
                ebuf[i+4] = (b8 >> 32) & 0xFF;
                ebuf[i+5] = (b8 >> 40) & 0xFF;
                ebuf[i+6] = (b8 >> 48) & 0xFF;
                ebuf[i+7] = (b8 >> 56) & 0xFF;
            }
        }
        else
        {
            for (i = start ; i < (start + length) ; i+=ainc)
            {
                b4 = ec_readeepromAP(aiadr, i >> 1 , EC_TIMEOUTEEP) & 0xFFFFFFFF;
                ebuf[i] = b4 & 0xFF;
                ebuf[i+1] = (b4 >> 8) & 0xFF;
                ebuf[i+2] = (b4 >> 16) & 0xFF;
                ebuf[i+3] = (b4 >> 24) & 0xFF;
            }
        }
        return 1;
    }
    return 0;
}

