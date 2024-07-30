#ifndef __SAVE_TO_FLASH_CONFIG_H
#define __SAVE_TO_FLASH_CONFIG_H

#define FLASH_SECTOR_MASK	(SECTORH|SECTORG|SECTORF|SECTORE)
#define FLASH_START_ADDR	0x3D8000	// Sector H start
#define FLASH_END_ADDR		0x3E7FFF	// Sector E end

#endif // __SAVE_TO_FLASH_CONFIG_H