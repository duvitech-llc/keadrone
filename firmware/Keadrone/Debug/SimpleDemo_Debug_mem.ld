/*
 * GENERATED FILE - DO NOT EDIT
 * (C) Code Red Technologies Ltd, 2008-9
 * Generated linker script include file for 
 * (created from LinkMemoryTemplate (LPCXpresso v3.6.2 [Build 279] [29/01/2011] ) on Wed Mar 23 21:10:12 CET 2011)
*/

MEMORY
{
  /* Define each memory region */
  MFlash128 (rx) : ORIGIN = 0x0, LENGTH = 0x20000 /* 128k */
  RamLoc16 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x4000 /* 16k */
  RamAHB16 (rwx) : ORIGIN = 0x2007c000, LENGTH = 0x4000 /* 16k */

}
  /* Define a symbol for the top of each memory region */
  __top_MFlash128 = 0x0 + 0x20000;
  __top_RamLoc16 = 0x10000000 + 0x4000;
  __top_RamAHB16 = 0x2007c000 + 0x4000;

