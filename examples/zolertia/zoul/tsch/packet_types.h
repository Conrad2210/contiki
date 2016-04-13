



typedef struct{
	uint16_t src;
	uint16_t dst;
	uint8_t type;
}BasePacket;


typedef struct{
		BasePacket base;
		uint8_t stage;
		uint16_t shortAddr;
		int8_t rssi;
		int8_t lqi;
}CIDER_PACKET;

typedef struct{
		BasePacket base;
		uint16_t size;
		uint8_t slotframeHandle;

}scheduleUpdate_Packet;



CIDER_PACKET parse_CIDER_PK(void * packet);
