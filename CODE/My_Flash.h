/*
 * My_Flash.h
 *
 *  Created on: 2022Äê5ÔÂ6ÈÕ
 *      Author: Fily
 */

#ifndef CODE_MY_FLASH_H_
#define CODE_MY_FLASH_H_

#define SectorofThreshold (0)
#define PageofThreshold (0)

#define SectorofMiddleLine (1)
#define PageofMiddleLine (0)

#define SectorofSanca (2)
#define PageofSanca (0)

#define SectorofOut (3)
#define PageofOut (0)

#define SectorofOutYuansu (4)
#define SectorofOutMotor  (5)


uint32 SavedValueAdd(uint32 sector,uint32 page_num);
uint32 SavedValueMini(uint32 sector,uint32 page_num);

#endif /* CODE_MY_FLASH_H_ */
