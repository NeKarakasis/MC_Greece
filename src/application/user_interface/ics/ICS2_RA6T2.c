









/**********************************************************

  RA6T2  Cortex M33

  RX by FIFO
  TX by FIFO

***********************************************************/

#include <stdint.h>
//#include "cmsis_gcc.h"
#include "ICS2_RA6T2.h"


#define   ICS_MSTPCRB      (*(volatile uint32_t*)0x40084004)
#define   ICS_PWPR         (*(volatile uint8_t* )0x4001fd0c)
#define   ICS_PFS605       (*(volatile uint32_t*)0x4001fb54)
#define   ICS_PFS606       (*(volatile uint32_t*)0x4001fb58)

#define   UART9_RDR        (*(volatile uint32_t*)0x40118900)
#define   UART9_TDR        (*(volatile uint32_t*)0x40118904)
#define   UART9_CCR0       (*(volatile uint32_t*)0x40118908)
#define   UART9_CCR1       (*(volatile uint32_t*)0x4011890C)
#define   UART9_CCR2       (*(volatile uint32_t*)0x40118910)
#define   UART9_CCR3       (*(volatile uint32_t*)0x40118914)
#define   UART9_CCR4       (*(volatile uint32_t*)0x40118918)
#define   UART9_FCR        (*(volatile uint32_t*)0x40118924)
#define   UART9_CSR        (*(volatile uint32_t*)0x40118948)
#define   UART9_FRSR       (*(volatile uint32_t*)0x40118950)
#define   UART9_FTSR       (*(volatile uint32_t*)0x40118954)
#define   UART9_CFCLR      (*(volatile uint32_t*)0x40118968)
#define   UART9_FFCLR      (*(volatile uint32_t*)0x40118970)



//#define  INSPECT_CHECK
//#define  ICS_DEBUG

//#define   EN_XOR_BYTE           // Enable if RL78, ARM_M0, ARM_M0+, (ARM_M23??)
  #define   EN_UNALIGNED_ACCESS   // Enable if RX, ARM_M3, ARM_M4, ARM_M7, ARM_M33

#define   ICS_VER_INT      (3)
#define   ICS_VER_FRAC     (50)

#define   WATCH_SUPPORT_VARTYPE      (0x1FFU)
#define   SCOPE_SUPPORT_CHMAX        (12)
#define   SCOPE_SUPPORT_VARTYPE      (0x1FFU)
#define   TXBUFLEN                   (18)

#define   WTYPE_UINT8         (0)
#define   WTYPE_INT8          (1)
#define   WTYPE_UINT16        (2)
#define   WTYPE_INT16         (3)
#define   WTYPE_UINT32        (4)
#define   WTYPE_INT32         (5)
#define   WTYPE_FLOAT         (6)
#define   WTYPE_BOOL          (7)
#define   WTYPE_LOGIC         (8)

#define   RAMSTARTADDR        (0x20000000U)    // RA6T2
#define   RAMENDADDR          (0x20001000U)    // RA6T2


#define   UART_RXRDY_MASK     (0x80)
#define   UART_ERROR_MASK     (0x19)

#define   UART_GetRxStatus()   (UART9_CSR>>24)           // OK
#define   UART_ErrorClear()    (UART9_CFCLR=0xbd070010)  // OK
#define   UART_GetData()       ((uint8_t)UART9_RDR)               // OK
#define   UART_SendData(d)     (UART9_TDR=(d))           // OK
#define   UART_RxFIFOClear()   (UART9_FCR|=0x808000)     // OK
#define   UART_RxRdy()         ((UART9_FRSR>>8)&0x3F)    // OK
#define   UART_TxFIFONum()     (UART9_FTSR)              // OK
#define   UART_TxEmpty()       ((UART9_CSR>>29)&1)

//static void UART_Reset(void);        
static void ics_send_packet(uint8_t st);
static void ics_command_ack_raw(void);
static void ics_command_error(uint8_t ecode);
static void ics_interpreter(void);
static void ics_send(void);
       void ics_int_sci_eri(void);
       void ics_int_sci_rxi(void);

//R_SCI0_Type  *ICS_UARTx;


#ifdef  ICS_DEBUG
uint32_t  ics_save = 0;
volatile uint32_t ics_command_t    = 0;
volatile uint32_t ics_count_skip   = 0;
volatile uint32_t ics_count_t      = 0;
volatile uint32_t ics_count_r      = 0;
volatile uint32_t ics_count_err    = 0;
volatile uint32_t ics_count_sp     = 0;
volatile uint32_t ics_count_ok_chr = 0;
volatile uint32_t ics_checksum_ng  = 0;
volatile uint32_t ics_checksum_ok  = 0;
volatile uint8_t  ics_byte[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}; // Need over 13
volatile uint32_t ics_debug_cnt    = 0;
#endif

static uint32_t ics_waddr[SCOPE_SUPPORT_CHMAX];
static uint32_t ics_buf2nd[SCOPE_SUPPORT_CHMAX-4];
static uint32_t ics_data1;
static uint32_t ics_data2;

static union
{
    uint8_t   txbuf[ TXBUFLEN];         /* for DMA buffer */
    uint16_t  txbufw[TXBUFLEN>>1];      /* 16bit */
    uint32_t  txbufL[TXBUFLEN>>2];
} ics_tx;

static union
{
    uint8_t  ics_wbuf[TXBUFLEN];        /* for watch function       */
    uint16_t ics_wbufw[TXBUFLEN>>1];
    uint32_t ics_wbufL[TXBUFLEN>>2];
} wbuf;

static int16_t ics_wready  = 0;         /* 0:No data in ics_wbuf[] */
static uint8_t ics_rx_cnt  = 0;
static uint8_t ics_rsum    = 0;
static uint8_t ics_rcom    = 0;
static uint8_t ics_even    = 0;

static uint8_t ics_port    = 0;
static uint8_t ics_port_h4 = 0;
static uint8_t ics_tmode   = 0;



static void ics_send_packet(uint8_t st)
{
    wbuf.ics_wbuf[0] = st;
    wbuf.ics_wbuf[1] = 0xFFU;
    ics_wready = 1;
}


static void ics_command_ack_raw(void)
{
    ics_send_packet(0xA5U);
}


static void ics_command_error(uint8_t ecode)
{
    wbuf.ics_wbuf[4] = ecode;
    UART_ErrorClear();
    UART_RxFIFOClear();
    ics_send_packet(0x0C5U);
}


void ics_int_sci_rxi(void)
{
    uint8_t  ch;
    uint8_t  st;

    st = UART_GetRxStatus()&UART_ERROR_MASK;
    if (st!=0)
    {
        UART_ErrorClear();
        UART_RxFIFOClear();
        ics_rx_cnt = 0;
        ics_command_error(0xfe);
      #ifdef ICS_DEBUG
        ics_count_err++;
      #endif
    }
    else if (UART_RxRdy()!=0)
    {
        ics_rx_cnt = (uint8_t)(ics_rx_cnt + 1);
        ch = UART_GetData();

      #ifdef ICS_DEBUG
        ics_count_ok_chr++;
        if (ics_rx_cnt>=12) { ics_debug_cnt++; } else { ics_byte[ics_rx_cnt] = ch; }
      #endif

        switch (ics_rx_cnt)
        {
        case 1:
            if (ch==0xAA)
            {
                ics_rsum  = 0x0AAU;
                ics_rcom  = 0;
                ics_data1 = 0;
                ics_data2 = 0;
            }
            else
            {
                ics_rx_cnt = 0;
              #ifdef ICS_DEBUG
                ics_count_skip++;
              #endif
            }
            break;

        case 2:
            ics_rsum = ics_rsum ^ ch;
            break;

        case 3:
            ics_rsum = ics_rsum ^ ch;
            ics_rcom = ch;
            break;

        case 4:
            ics_rsum = ics_rsum ^ ch;
            break;

        case 5:
            ics_rsum = ics_rsum ^ ch;
            ics_data1 = ((uint32_t)ch);
            break;

        case 6:
            ics_rsum = ics_rsum ^ ch;
            ics_data1 = ics_data1 | (((uint32_t)ch)<<8);
            break;

        case 7:
            ics_rsum = ics_rsum ^ ch;
            ics_data1 = ics_data1 | (((uint32_t)ch)<<16);
            break;

        case 8:
            ics_rsum = ics_rsum ^ ch;
            ics_data1 = ics_data1 | (((uint32_t)ch)<<24);
            break;

        case 9:
            ics_rsum = ics_rsum ^ ch;
            ics_data2 = ((int32_t)ch);
            break;

        case 10:
            ics_rsum = ics_rsum ^ ch;
            ics_data2 = ics_data2 | (((uint32_t)ch)<<8);
            break;

        case 11:
            ics_rsum = ics_rsum ^ ch;
            ics_data2 = ics_data2 | (((uint32_t)ch)<<16);
            break;

        case 12:
            ics_rx_cnt = 0;
            ics_rsum = ics_rsum ^ ch;
            ics_data2 = ics_data2 | (((uint32_t)ch)<<24);
        
            if (ics_rsum == 0x0FFU)      /* check sum OK */
            {
              #ifdef  ICS_DEBUG
                ics_checksum_ok++;
              #endif
                ics_interpreter();
            }
            else                         /* check sum NG */
            {
              #ifdef ICS_DEBUG
                ics_checksum_ng++;
              #endif
                ics_command_error(10);   /* check sum error */
            }
            break;

        default:
            ics_rx_cnt = 0;
            ics_command_error(0x90);
            break;
        }
    }
}



static void ics_interpreter(void)
{
    switch (ics_rcom)
    {
    case 3:       /* Ask CPU name */
        wbuf.ics_wbuf[4] = 'R';
        wbuf.ics_wbuf[5] = 'A';
        wbuf.ics_wbuf[6] = '6';
        wbuf.ics_wbuf[7] = 'T';
        wbuf.ics_wbuf[8] = '2';
        wbuf.ics_wbuf[9] = '/';
        wbuf.ics_wbuf[10]= 'S';
        wbuf.ics_wbuf[11]= 'C';
        wbuf.ics_wbuf[12]= 'I';
        wbuf.ics_wbuf[13]= (uint8_t)('0' + ics_port_h4);
        wbuf.ics_wbuf[14]=  0;
        wbuf.ics_wbuf[15]=  0;
        ics_command_ack_raw();
        break;

    case 4:       /* Ask LIB version */
        ics_data1   = ics_data1 & WATCH_SUPPORT_VARTYPE;
        wbuf.ics_wbuf[4] = ICS_VER_FRAC;          /* decimal */
        wbuf.ics_wbuf[5] = ICS_VER_INT;           /* integer */
        wbuf.ics_wbuf[6] = (uint8_t)(ics_data1);  /* 8,16,32, signed, unsigned */
        wbuf.ics_wbuf[7] = (uint8_t)(ics_data1>>8);
        ics_command_ack_raw();
        break;

    case 5:       /* Read 8bit */
        if ((ics_data1>=RAMSTARTADDR)&&(ics_data1<RAMENDADDR))
        {
          #if 1
            wbuf.ics_wbufL[1] = *((uint8_t*)ics_data1); 
          #else
            wbuf.ics_wbuf[4] = * ((uint8_t*)ics_data1);
            wbuf.ics_wbuf[5] = 0;
            wbuf.ics_wbuf[6] = 0;
            wbuf.ics_wbuf[7] = 0;
          #endif
            ics_command_ack_raw();
        }
        else
        {
            ics_command_error(0x87);  // Invalid alignment access
        }
        break;

    case 6:       /* Read 16bit */
        {
          #ifdef   EN_UNALIGNED_ACCESS
            if ((ics_data1>=RAMSTARTADDR)&&(ics_data1<RAMENDADDR))                      // range check
          #else
            if (((ics_data1&1)==0)&&(ics_data1>=RAMSTARTADDR)&&(ics_data1<RAMENDADDR))  // alignment and range check
          #endif
            {
              #if 0
                uint16_t  tdata16;
                tdata16      = * ((int16_t*)ics_data1);
                wbuf.ics_wbuf[4] = (uint8_t)tdata16;
                wbuf.ics_wbuf[5] = (uint8_t)(tdata16>>8);
                wbuf.ics_wbuf[6] = 0;
                wbuf.ics_wbuf[7] = 0;
              #else
                wbuf.ics_wbufL[1] = *((uint16_t*)ics_data1);
              #endif
                ics_command_ack_raw();
            }
            else
            {
                ics_command_error(0x87);  // Invalid alignment access
            }
        }
        break;

    case 7:       /* Read 32bit */
      #ifdef   EN_UNALIGNED_ACCESS
        if ((ics_data1>=RAMSTARTADDR)&&(ics_data1<RAMENDADDR))                      // range check
      #else
        if (((ics_data1&3)==0)&&(ics_data1>=RAMSTARTADDR)&&(ics_data1<RAMENDADDR))  // alignment and range check
      #endif
        {
            wbuf.ics_wbufL[1] = *((uint32_t*)ics_data1);
            ics_command_ack_raw();
        }
        else
        {
            ics_command_error(0x87);  // Invalid alignment access
        }
        break;

    case 8:       /* Write 8bit */
        if ((ics_data1>=RAMSTARTADDR)&&(ics_data1<RAMENDADDR))                      // range check
        {
            * ((int8_t*)ics_data1) = (int8_t)ics_data2;
            ics_command_ack_raw();
        }
        else
        {
            ics_command_error(0x87);  // Invalid alignment access
        }
        break;

    case 9:       /* Write 16bit */
      #ifdef   EN_UNALIGNED_ACCESS
        if ((ics_data1>=RAMSTARTADDR)&&(ics_data1<RAMENDADDR))                      // range check
      #else
        if (((ics_data1&1)==0)&&(ics_data1>=RAMSTARTADDR)&&(ics_data1<RAMENDADDR))  // alignment and range check
      #endif
        {
            * ((int16_t*)ics_data1) = (int16_t)ics_data2;
            ics_command_ack_raw();
        }
        else
        {
            ics_command_error(0x87);  // Invalid alignment access
        }
        break;

    case 10:      /* Write 32bit */
      #ifdef   EN_UNALIGNED_ACCESS
        if ((ics_data1>=RAMSTARTADDR)&&(ics_data1<RAMENDADDR))                      // range check
      #else
        if (((ics_data1&3)==0)&&(ics_data1>=RAMSTARTADDR)&&(ics_data1<RAMENDADDR))  // alignment and range check
      #endif
        {
            * ((uint32_t*)ics_data1) = ics_data2;
            ics_command_ack_raw();
        }
        else
        {
            ics_command_error(0x87);  // Invalid alignment access
        }
        break;

    case 11:      /* Check rightfulness */
        wbuf.ics_wbufL[1] = (uint32_t)(ics_data1 ^ 0x19631017UL ^ ICS_VER_FRAC);
        ics_command_ack_raw();
        break;

    case 32:      /* Ask Scope support1 */
        ics_data1   = ics_data1 & SCOPE_SUPPORT_VARTYPE; /* Scope 32,16,8bit */
      #if 0
        wbuf.ics_wbuf[4] = 0;                      /* reserved */
        wbuf.ics_wbuf[5] = 0;                      /* reserved */
        wbuf.ics_wbuf[6] = (uint8_t)(ics_data1);
        wbuf.ics_wbuf[7] = (uint8_t)(ics_data1>>8);
      #else
        wbuf.ics_wbufw[2] = 0;
        wbuf.ics_wbufw[3] = (uint16_t)ics_data1;
      #endif
        wbuf.ics_wbuf[8] = 16;
        ics_command_ack_raw();
        break;

    case 34:      /* Set Scope channel */
        {
            uint8_t  ty;
            uint8_t  ch;

            ch = (uint8_t)(ics_data1);
            ty = (uint8_t)(ics_data1>>8);

            if (ch<SCOPE_SUPPORT_CHMAX)
            {
                if (ty<=WTYPE_LOGIC)
                {
                  #ifndef   EN_UNALIGNED_ACCESS
                    if (((ty==2)||(ty==3))&&((ics_data2&1)!=0))
                    {
                        ics_command_error(0x87);  // Invalid alignment access
                    }
                    else if (((ty==4)||(ty==5))&&((ics_data2&3)!=0))
                    {
                        ics_command_error(0x87);  // Invalid alignment access
                    }
                    else
                    { /* no code */ }
                  #endif

                    if ((ics_data2<RAMSTARTADDR)||(ics_data2>=RAMENDADDR))
                    {
                        ics_command_error(0x87);  // Invalid alignment access
                    }
                    else
                    {
                        ics_waddr[ch] = ics_data2;
                      //ics_wtype[ch] = ty;
                      #if 0
                        wbuf.ics_wbuf[4] = 0;      /* reserved */
                        wbuf.ics_wbuf[5] = 0;      /* reserved */
                        wbuf.ics_wbuf[6] = 0;      /* reserved */
                        wbuf.ics_wbuf[7] = 0;      /* reserved */
                      #else
                        wbuf.ics_wbufL[1] = 0;
                      #endif
                        ics_command_ack_raw();
                    }
                }
                else
                {
                    ics_command_error(0x8A);
                }
            }
            else
            {
                ics_command_error(0x89);
            }
        }
        break;

    default:
        ics_command_error(11);    /* Undefined command error */
        break;
    }
}


void ics2_watchpoint(void)
{
    ics_int_sci_rxi();

#ifdef INSPECT_CHECK

    if (ics_wready==1)
    {
      #ifdef   EN_UNALIGNED_ACCESS
        ics_tx.txbufL[0]  = wbuf.ics_wbufL[0];
        ics_tx.txbufL[1]  = wbuf.ics_wbufL[1];
        ics_tx.txbufL[2]  = wbuf.ics_wbufL[2];
        ics_tx.txbufL[3]  = wbuf.ics_wbufL[3];
        ics_tx.txbufw[8]  = wbuf.ics_wbufw[8];
      #else
        ics_tx.txbuf[0]  = wbuf.ics_wbuf[0];
        ics_tx.txbuf[1]  = wbuf.ics_wbuf[1];
        ics_tx.txbuf[2]  = wbuf.ics_wbuf[2];
        ics_tx.txbuf[3]  = wbuf.ics_wbuf[3];
        ics_tx.txbuf[4]  = wbuf.ics_wbuf[4];
        ics_tx.txbuf[5]  = wbuf.ics_wbuf[5];
        ics_tx.txbuf[6]  = wbuf.ics_wbuf[6];
        ics_tx.txbuf[7]  = wbuf.ics_wbuf[7];
        ics_tx.txbuf[8]  = wbuf.ics_wbuf[8];
        ics_tx.txbuf[9]  = wbuf.ics_wbuf[9];
        ics_tx.txbuf[10] = wbuf.ics_wbuf[10];
        ics_tx.txbuf[11] = wbuf.ics_wbuf[11];
        ics_tx.txbuf[12] = wbuf.ics_wbuf[12];
        ics_tx.txbuf[13] = wbuf.ics_wbuf[13];
        ics_tx.txbuf[14] = wbuf.ics_wbuf[14];
        ics_tx.txbuf[15] = wbuf.ics_wbuf[15];
        ics_tx.txbuf[16] = wbuf.ics_wbuf[16];
        ics_tx.txbuf[17] = wbuf.ics_wbuf[17];
      #endif
        ics_wready = 0;

        ics_send();
    }

#else

    if (ics_wready==1)
    {
      #ifdef ICS_DEBUG
        ics_command_t++;
      #endif

      #ifdef   EN_UNALIGNED_ACCESS
        ics_tx.txbufL[0]  = wbuf.ics_wbufL[0];
        ics_tx.txbufL[1]  = wbuf.ics_wbufL[1];
        ics_tx.txbufL[2]  = wbuf.ics_wbufL[2];
        ics_tx.txbufL[3]  = wbuf.ics_wbufL[3];
        ics_tx.txbufw[8]  = wbuf.ics_wbufw[8];
      #else
        ics_tx.txbuf[0]  = wbuf.ics_wbuf[0];
        ics_tx.txbuf[1]  = wbuf.ics_wbuf[1];
        ics_tx.txbuf[2]  = wbuf.ics_wbuf[2];
        ics_tx.txbuf[3]  = wbuf.ics_wbuf[3];
        ics_tx.txbuf[4]  = wbuf.ics_wbuf[4];
        ics_tx.txbuf[5]  = wbuf.ics_wbuf[5];
        ics_tx.txbuf[6]  = wbuf.ics_wbuf[6];
        ics_tx.txbuf[7]  = wbuf.ics_wbuf[7];
        ics_tx.txbuf[8]  = wbuf.ics_wbuf[8];
        ics_tx.txbuf[9]  = wbuf.ics_wbuf[9];
        ics_tx.txbuf[10] = wbuf.ics_wbuf[10];
        ics_tx.txbuf[11] = wbuf.ics_wbuf[11];
        ics_tx.txbuf[12] = wbuf.ics_wbuf[12];
        ics_tx.txbuf[13] = wbuf.ics_wbuf[13];
        ics_tx.txbuf[14] = wbuf.ics_wbuf[14];
        ics_tx.txbuf[15] = wbuf.ics_wbuf[15];
        ics_tx.txbuf[16] = wbuf.ics_wbuf[16];
        ics_tx.txbuf[17] = wbuf.ics_wbuf[17];
      #endif
        ics_wready = 0;
    }

    else if (ics_tmode==1)  // 32bit 8ch twice mode
    {
        if (ics_even==0)
        {
          #ifdef   EN_UNALIGNED_ACCESS
            *(uint32_t*)(&(ics_tx.txbufw[1])) = *(uint32_t*)ics_waddr[0];
            *(uint32_t*)(&(ics_tx.txbufw[3])) = *(uint32_t*)ics_waddr[1];
            *(uint32_t*)(&(ics_tx.txbufw[5])) = *(uint32_t*)ics_waddr[2];
            *(uint32_t*)(&(ics_tx.txbufw[7])) = *(uint32_t*)ics_waddr[3];
          #else
            int32_t  temp;
            temp = *(int32_t*)ics_waddr[0];
            ics_tx.txbufw[1] = (int16_t)temp;
            ics_tx.txbufw[2] = (int16_t)(temp>>16);
            temp = *(int32_t*)ics_waddr[1];
            ics_tx.txbufw[3] = (int16_t)temp;
            ics_tx.txbufw[4] = (int16_t)(temp>>16);
            temp = *(int32_t*)ics_waddr[2];
            ics_tx.txbufw[5] = (int16_t)temp;
            ics_tx.txbufw[6] = (int16_t)(temp>>16);
            temp = *(int32_t*)ics_waddr[3];
            ics_tx.txbufw[7] = (int16_t)temp;
            ics_tx.txbufw[8] = (int16_t)(temp>>16);
          #endif
            ics_buf2nd[0] = *(uint32_t*)ics_waddr[4];
            ics_buf2nd[1] = *(uint32_t*)ics_waddr[5];
            ics_buf2nd[2] = *(uint32_t*)ics_waddr[6];
            ics_buf2nd[3] = *(uint32_t*)ics_waddr[7];

            ics_tx.txbufw[0] = 0xFF56U;
            ics_even = 1;
        }
        else
        {
          #ifdef   EN_UNALIGNED_ACCESS
            *(uint32_t*)(&(ics_tx.txbufw[1])) = ics_buf2nd[0];
            *(uint32_t*)(&(ics_tx.txbufw[3])) = ics_buf2nd[1];
            *(uint32_t*)(&(ics_tx.txbufw[5])) = ics_buf2nd[2];
            *(uint32_t*)(&(ics_tx.txbufw[7])) = ics_buf2nd[3];
          #else
            ics_tx.txbufw[1] = (int16_t)ics_buf2nd[0];
            ics_tx.txbufw[2] = (int16_t)(ics_buf2nd[0]>>16);
            ics_tx.txbufw[3] = (int16_t)ics_buf2nd[1];
            ics_tx.txbufw[4] = (int16_t)(ics_buf2nd[1]>>16);
            ics_tx.txbufw[5] = (int16_t)ics_buf2nd[2];
            ics_tx.txbufw[6] = (int16_t)(ics_buf2nd[2]>>16);
            ics_tx.txbufw[7] = (int16_t)ics_buf2nd[3];
            ics_tx.txbufw[8] = (int16_t)(ics_buf2nd[3]>>16);
          #endif

            ics_tx.txbufw[0] = 0xFF57U;
            ics_even = 0;
        }
    }
    else if (ics_tmode==2)  // 32bit 4ch one time mode
    {
         {
          #ifdef   EN_UNALIGNED_ACCESS
            *(uint32_t*)(&(ics_tx.txbufw[1])) = *(uint32_t*)ics_waddr[0];
            *(uint32_t*)(&(ics_tx.txbufw[3])) = *(uint32_t*)ics_waddr[1];
            *(uint32_t*)(&(ics_tx.txbufw[5])) = *(uint32_t*)ics_waddr[2];
            *(uint32_t*)(&(ics_tx.txbufw[7])) = *(uint32_t*)ics_waddr[3];
          #else
            uint32_t  temp;

            temp = *(int32_t*)ics_waddr[0];
            ics_tx.txbufw[1] = (int16_t)temp;
            ics_tx.txbufw[2] = (int16_t)(temp>>16);
            temp = *(int32_t*)ics_waddr[1];
            ics_tx.txbufw[3] = (int16_t)temp;
            ics_tx.txbufw[4] = (int16_t)(temp>>16);
            temp = *(int32_t*)ics_waddr[2];
            ics_tx.txbufw[5] = (int16_t)temp;
            ics_tx.txbufw[6] = (int16_t)(temp>>16);
            temp = *(int32_t*)ics_waddr[3];
            ics_tx.txbufw[7] = (int16_t)temp;
            ics_tx.txbufw[8] = (int16_t)(temp>>16);
          #endif

            ics_tx.txbufw[0] = 0xFF58U;
        }
    }
    else if (ics_tmode==3)  // 32bit 12ch 3times mode
    {
        if (ics_even==0)
        {
          #ifdef   EN_UNALIGNED_ACCESS
            *(uint32_t*)(&(ics_tx.txbufw[1])) = *(uint32_t*)ics_waddr[0];
            *(uint32_t*)(&(ics_tx.txbufw[3])) = *(uint32_t*)ics_waddr[1];
            *(uint32_t*)(&(ics_tx.txbufw[5])) = *(uint32_t*)ics_waddr[2];
            *(uint32_t*)(&(ics_tx.txbufw[7])) = *(uint32_t*)ics_waddr[3];
          #else
            uint32_t  temp;

            temp = *(int32_t*)ics_waddr[0];
            ics_tx.txbufw[1] = (int16_t)temp;
            ics_tx.txbufw[2] = (int16_t)(temp>>16);
            temp = *(int32_t*)ics_waddr[1];
            ics_tx.txbufw[3] = (int16_t)temp;
            ics_tx.txbufw[4] = (int16_t)(temp>>16);
            temp = *(int32_t*)ics_waddr[2];
            ics_tx.txbufw[5] = (int16_t)temp;
            ics_tx.txbufw[6] = (int16_t)(temp>>16);
            temp = *(int32_t*)ics_waddr[3];
            ics_tx.txbufw[7] = (int16_t)temp;
            ics_tx.txbufw[8] = (int16_t)(temp>>16);
          #endif

            ics_buf2nd[0] = *(uint32_t*)ics_waddr[4];
            ics_buf2nd[1] = *(uint32_t*)ics_waddr[5];
            ics_buf2nd[2] = *(uint32_t*)ics_waddr[6];
            ics_buf2nd[3] = *(uint32_t*)ics_waddr[7];
            ics_buf2nd[4] = *(uint32_t*)ics_waddr[8];
            ics_buf2nd[5] = *(uint32_t*)ics_waddr[9];
            ics_buf2nd[6] = *(uint32_t*)ics_waddr[10];
            ics_buf2nd[7] = *(uint32_t*)ics_waddr[11];

            ics_tx.txbufw[0] = 0xFF59U;
            ics_even = 1;
        }
        else if (ics_even==1)
        {
          #ifdef   EN_UNALIGNED_ACCESS
            *(uint32_t*)(&(ics_tx.txbufw[1])) = ics_buf2nd[0];
            *(uint32_t*)(&(ics_tx.txbufw[3])) = ics_buf2nd[1];
            *(uint32_t*)(&(ics_tx.txbufw[5])) = ics_buf2nd[2];
            *(uint32_t*)(&(ics_tx.txbufw[7])) = ics_buf2nd[3];
          #else
            ics_tx.txbufw[1] = (int16_t)ics_buf2nd[0];
            ics_tx.txbufw[2] = (int16_t)(ics_buf2nd[0]>>16);
            ics_tx.txbufw[3] = (int16_t)ics_buf2nd[1];
            ics_tx.txbufw[4] = (int16_t)(ics_buf2nd[1]>>16);
            ics_tx.txbufw[5] = (int16_t)ics_buf2nd[2];
            ics_tx.txbufw[6] = (int16_t)(ics_buf2nd[2]>>16);
            ics_tx.txbufw[7] = (int16_t)ics_buf2nd[3];
            ics_tx.txbufw[8] = (int16_t)(ics_buf2nd[3]>>16);
          #endif

            ics_tx.txbufw[0] = 0xFF5AU;
            ics_even = 2;
        }
        else // (ics_evem==2)
        {
          #ifdef   EN_UNALIGNED_ACCESS
            *(uint32_t*)(&(ics_tx.txbufw[1])) = ics_buf2nd[4];
            *(uint32_t*)(&(ics_tx.txbufw[3])) = ics_buf2nd[5];
            *(uint32_t*)(&(ics_tx.txbufw[5])) = ics_buf2nd[6];
            *(uint32_t*)(&(ics_tx.txbufw[7])) = ics_buf2nd[7];
          #else
            ics_tx.txbufw[1] = (int16_t)ics_buf2nd[4];
            ics_tx.txbufw[2] = (int16_t)(ics_buf2nd[4]>>16);
            ics_tx.txbufw[3] = (int16_t)ics_buf2nd[5];
            ics_tx.txbufw[4] = (int16_t)(ics_buf2nd[5]>>16);
            ics_tx.txbufw[5] = (int16_t)ics_buf2nd[6];
            ics_tx.txbufw[6] = (int16_t)(ics_buf2nd[6]>>16);
            ics_tx.txbufw[7] = (int16_t)ics_buf2nd[7];
            ics_tx.txbufw[8] = (int16_t)(ics_buf2nd[7]>>16);
          #endif

            ics_tx.txbufw[0] = 0xFF5BU;
            ics_even = 0;
        }
    }
    else
    {
        // No code 
    }

    ics_send();
#endif
}


void ics2_init(uint8_t port, uint8_t speed, uint8_t mode)
{
    int16_t  i;

    ics_port    = port;
    ics_port_h4 = ics_port>>4;
    ics_tmode   = mode;

    for(i=0;i<SCOPE_SUPPORT_CHMAX;i++)
    {
        ics_waddr[i] = RAMSTARTADDR;  /* RAM START ADDR */
      //ics_wtype[i] = WTYPE_INT32;   /* 32bit int      */
    }

    ics_rx_cnt = 0;
    ics_wready = 0;   /* no data in ics_wbuf[] */

    if      (ics_port==ICS_SCI9_PD05_PD06)
    {

        ICS_MSTPCRB &= ~(0x00400000UL);  // SCI9

        ICS_PWPR = 0x00;
        ICS_PWPR = 0x40;
        ICS_PFS605 = 0x05010000;
        ICS_PFS606 = 0x05010000;
        ICS_PWPR = 0x00;
        ICS_PWPR = 0x80;

        UART9_CCR0  = 0x00000000;
        UART9_CCR1  = 0x00000000;
        UART9_CCR2  = 0x00000040 | (((uint32_t)speed)<<8); //  ABCSE=1 (1/6)  20Mbps
        UART9_CCR3 |= (1UL<<20);  //  FIFO enable

                // R TRIG
      #if 0
        UART9_FCR   = (UART9_FCR & ~0x1F0000UL) | (1UL<<16);
      #else
        UART9_FCR   = (15UL<<24) | (1UL<<16) | (1UL<<8);
      #endif
        UART9_CCR0 = 0x00000011;  // TX/RX enable
    }
}


/*""FILE COMMENT""**********Technical reference data***************************
 * Outline      : Send setting
 * Description  : The send processing is started.
 *""FILE COMMENT END""*********************************************************/
static void ics_send(void)
{
  #ifdef  EN_XOR_BYTE   // 6.2us .. 7.3us (opt=high) good for M0+?
    /* generate check sum */
    ics_tx.txbuf[1] = ics_tx.txbuf[0]  ^ ics_tx.txbuf[1]  ^ ics_tx.txbuf[2]
                    ^ ics_tx.txbuf[3]  ^ ics_tx.txbuf[4]  ^ ics_tx.txbuf[5]
                    ^ ics_tx.txbuf[6]  ^ ics_tx.txbuf[7]  ^ ics_tx.txbuf[8]
                    ^ ics_tx.txbuf[9]  ^ ics_tx.txbuf[10] ^ ics_tx.txbuf[11]
                    ^ ics_tx.txbuf[12] ^ ics_tx.txbuf[13] ^ ics_tx.txbuf[14]
                    ^ ics_tx.txbuf[15] ^ ics_tx.txbuf[16] ^ ics_tx.txbuf[17];
  #else   // 5.3us .. 6.75us max=9.8us (opt=high) good for M3
    /* generate check sum */
    #if 0
        uint16_t  tempw;
        tempw = ics_tx.txbufw[0] ^ ics_tx.txbufw[1] ^ ics_tx.txbufw[2]
              ^ ics_tx.txbufw[3] ^ ics_tx.txbufw[4] ^ ics_tx.txbufw[5]
              ^ ics_tx.txbufw[6] ^ ics_tx.txbufw[7] ^ ics_tx.txbufw[8];
        ics_tx.txbuf[1] = (uint16_t)(tempw>>8) ^ (uint16_t)tempw;
    #else
        uint32_t  tempL;
        tempL = ics_tx.txbufL[0] ^ ics_tx.txbufL[1] ^ ics_tx.txbufL[2] ^ ics_tx.txbufL[3];
        ics_tx.txbuf[1] = ((uint8_t)(tempL>>24)) ^ ((uint8_t)(tempL>>16))
                        ^ ((uint8_t)(tempL>>8))  ^ ((uint8_t)tempL)
                        ^ ics_tx.txbuf[16]        ^ ics_tx.txbuf[17];
    #endif
  #endif

  #if 1
    UART_SendData(ics_tx.txbuf[0]);
    UART_SendData(ics_tx.txbuf[1]);
    UART_SendData(ics_tx.txbuf[2]);
    UART_SendData(ics_tx.txbuf[3]);
    UART_SendData(ics_tx.txbuf[4]);
    UART_SendData(ics_tx.txbuf[5]);
    UART_SendData(ics_tx.txbuf[6]);
    UART_SendData(ics_tx.txbuf[7]);
    UART_SendData(ics_tx.txbuf[8]);
    UART_SendData(ics_tx.txbuf[9]);
    UART_SendData(ics_tx.txbuf[10]);
    UART_SendData(ics_tx.txbuf[11]);
    UART_SendData(ics_tx.txbuf[12]);
    UART_SendData(ics_tx.txbuf[13]);
    UART_SendData(ics_tx.txbuf[14]);
    UART_SendData(ics_tx.txbuf[15]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[16]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[17]);
  #else
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[ 0]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[ 1]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[ 2]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[ 3]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[ 4]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[ 5]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[ 6]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[ 7]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[ 8]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[ 9]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[10]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[11]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[12]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[13]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[14]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[15]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[16]);
    while (UART_TxFIFONum()>15) { ; }    UART_SendData(ics_tx.txbuf[17]);
  #endif
}


uint32_t  ics2_version(void)
{
    return (ICS_VER_INT*65536+ICS_VER_FRAC);
}


void ics_int_sci_eri(void)
{

}

