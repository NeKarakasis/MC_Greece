











#ifndef __ICS2_RA6T3_H__
#define __ICS2_RA6T3_H__

#define ICS_SCI0_P101_P100     (0x00U)
#define ICS_SCI0_P213_P212     (0x01U)
#define ICS_SCI0_P411_P410     (0x02U)
#define ICS_SCI9_P110_P109     (0x90U)

void     ics2_init(uint8_t port, uint8_t speed, uint8_t mode);
void     ics2_watchpoint(void);
uint32_t ics2_version(void);

#endif /* __ICS2_RA6T3_H__ */
