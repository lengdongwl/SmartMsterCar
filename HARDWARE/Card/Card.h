#ifndef __CARD_H
#define __CARD_H
#include "sys.h"

extern uint8_t RFID_Card_One[16];   //¶Á¿¨Êı¾İ»º´æÇø 1
extern uint8_t RFID_Card_Two[16];	  //¶Á¿¨Êı¾İ»º´æÇø 2
extern uint8_t RFID_Card_Three[16];	//¶Á¿¨Êı¾İ»º´æÇø 3
extern uint8_t Read_Flag[3];	      //¶Á¿¨±êÖ¾Î»


uint8_t Card_One(uint16_t distance);		// µ¥¶Î¼ì²â(Â·ÕÏ-¶Á¿¨-¶Á¿¨)
uint8_t Card_Two(void);									// Ñ°ÖÕµã¿¨(¶Á¿¨)
uint8_t Card_Three(uint16_t distance);	// Ë«¶Î¼ì²â£¨Â·ÕÏ-¶Á¿¨-Â·ÕÏ-¶Á¿¨-Â·ÕÏ-¶Á¿¨£©
uint8_t Card_Four(uint16_t distance);		// Ë«¶ÎÖĞÂ·ÕÏ¼ì²â£¨Â·ÕÏ£©
uint8_t Card_Five(uint16_t distance);		// µ¥¶Î¼ì²âÍ££¨Â·ÕÏ-¶Á¿¨£©
uint8_t Card_SIX(uint16_t distance);		// Ë«¶Î¼ì²âÂ·ÕÏ²»¶ÁÖÕµã¿¨£¨Â·ÕÏ-¶Á¿¨£©







void Obtain_section_distance(void);							//²âÁ¿µ¥¶Î¾àÀë
void RFID_detection_motion(void);								//Î¢¶¯¼ì²â¿¨
uint8_t Auto_write_card(uint8_t Block_address); //Ğ´¿¨
uint8_t Auto_Read_card(uint8_t Block_address);	//¶Á¿¨

#endif
