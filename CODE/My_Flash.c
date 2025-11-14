/*
 * My_Flash.c
 *
 *  Created on: 2022年5月6日
 *      Author: Fily
 */
#include "headfile.h"
#include "My_Flash.h"
//-------------------------------------------------------------------------------------------------------------------
//  @brief      数据++ and 数据--
//  @param      uint32
//  @return     void
//  @since      sector扇区；page_num页
//-------------------------------------------------------------------------------------------------------------------
uint32 SavedValueAdd(uint32 sector,uint32 page_num)
{
    uint32 Value;
    Value = flash_read(sector,page_num,uint32);
    if(flash_check(sector, page_num))
    {
        eeprom_erase_sector(sector);
    }
    Value++;
    eeprom_page_program(sector, page_num, &Value);
    return Value;
}

uint32 SavedValueMini(uint32 sector,uint32 page_num)
{
    uint32 Value;
    Value = flash_read(sector,page_num,uint32);
    if(flash_check(sector, page_num))
    {
        eeprom_erase_sector(sector);
    }
    Value--;
    eeprom_page_program(sector, page_num, &Value);
    return Value;
}

