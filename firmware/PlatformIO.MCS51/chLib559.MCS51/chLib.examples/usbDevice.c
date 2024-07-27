/********************************** (C) COPYRIGHT *******************************
* File Name          :USBDEV.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        : CH559ģ��USB 37X�豸
 USB device example for CH559, use CH372 tool TEST.exe to check it under Windows
*******************************************************************************/
#include "..\..\DEBUG.C"                                                       //������Ϣ��ӡ
#include "..\..\DEBUG.H"
#include <string.h>

#define THIS_ENDP0_SIZE         DEFAULT_ENDP0_SIZE
#define ENDP1_IN_SIZE           64
#define ENDP2_IN_SIZE           64
#define ENDP2_OUT_SIZE          64

/*�豸������*/
UINT8C  MyDevDescr[] = { 0x12, 0x01, 0x10, 0x01,
                         0xFF, 0x80, 0x55, THIS_ENDP0_SIZE,
                         0x48, 0x43, 0x37, 0x55,                              // ����ID�Ͳ�ƷID
                         0x00, 0x01, 0x01, 0x02,
                         0x00, 0x01
                       };
/*����������*/
UINT8C  MyCfgDescr[] = { 0x09, 0x02, 0x27, 0x00, 0x01, 0x01, 0x00, 0x80, 0x32,
                         0x09, 0x04, 0x00, 0x00, 0x03, 0xFF, 0x80, 0x55, 0x00,
                         0x07, 0x05, 0x82, 0x02, ENDP2_IN_SIZE, 0x00, 0x00,
                         0x07, 0x05, 0x02, 0x02, ENDP2_OUT_SIZE, 0x00, 0x00,
                         0x07, 0x05, 0x81, 0x03, ENDP1_IN_SIZE, 0x00, 0x00
                       };
/*����������*/
UINT8C  MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };
/*������Ϣ*/
UINT8C  MyManuInfo[] = { 0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0 };
/*��Ʒ��Ϣ*/
UINT8C  MyProdInfo[] = { 0x0C, 0x03, 'C', 0, 'H', 0, '5', 0, '5', 0, '9', 0 };

UINT8   UsbConfig = 0;                                                         // USB���ñ�־
UINT8X  Ep0Buffer[MIN(64,THIS_ENDP0_SIZE+2)] _at_ 0x0000 ; // OUT&IN, must even address
UINT8X  Ep1Buffer[MIN(64,ENDP1_IN_SIZE+2)] _at_ MIN(64,THIS_ENDP0_SIZE+2); // IN, must even address
UINT8X  Ep2Buffer[MIN(64,ENDP2_IN_SIZE+2)+MIN(64,ENDP2_OUT_SIZE+2)] _at_ (MIN(64,THIS_ENDP0_SIZE+2)+MIN(64,ENDP1_IN_SIZE+2));// OUT+IN, must even address

#define UsbSetupBuf     ((PUSB_SETUP_REQ)Ep0Buffer)

/*����״ָ̬ʾ*/
sbit LED_RUN  = P1^0;
sbit LED_CFG  = P1^1;
sbit LED_TMP  = P1^2;

#pragma NOAREGS

/*******************************************************************************
* Function Name  : USB_DeviceInterrupt()
* Description    : CH559USBģ�������жϴ�������
*******************************************************************************/
void    USB_DeviceInterrupt( void ) interrupt INT_NO_USB using 1               /* USB�жϷ������,ʹ�üĴ�����1 */
{
    UINT8   i, len;
    static  UINT8   SetupReqCode, SetupLen;
    static  PUINT8  pDescr;
    LED_TMP = 0;
	
    if ( UIF_TRANSFER )                                                        // USB�������
    {
        if ( U_IS_NAK )                                                        // not enable for this example
        {
//          switch ( USB_INT_ST & ( MASK_UIS_TOKEN | MASK_UIS_ENDP ) )         // �����������ƺͶ˵��
//          {  
//              case UIS_TOKEN_OUT | 2:                                        // endpoint 2# �����˵��´�
//                  break;
//              case UIS_TOKEN_IN | 2:                                         // endpoint 2# �����˵��ϴ�
//                  break;
//              case UIS_TOKEN_IN | 1:                                         // endpoint 1# �ж϶˵��ϴ�
//                  break;
//              default:
//                  break;
//          }
            printf("NAK INT,PrepareData\n");
        }
        else
        {
            switch ( USB_INT_ST & ( MASK_UIS_TOKEN | MASK_UIS_ENDP ) )         // �����������ƺͶ˵��
            {
            case UIS_TOKEN_OUT | 2:                                            // endpoint 2# �����˵��´�
                if ( U_TOG_OK )                                                // ��ͬ�������ݰ�������
                {
                    UEP2_CTRL ^= bUEP_R_TOG;                                   // �ֶ���ת
                    len = USB_RX_LEN;
                    for ( i = 0; i < len; i ++ )
                    {
                        Ep2Buffer[MAX_PACKET_SIZE+i] = Ep2Buffer[i] ^ 0xFF;    // OUT����ȡ����IN�ɼ������֤
                    }
                    UEP2_T_LEN = len;
                    UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;  // �����ϴ�
                }
                break;
            case UIS_TOKEN_IN | 2:                                             // endpoint 2# �����˵��ϴ�
                UEP2_CTRL ^= bUEP_T_TOG;                                       // �ֶ���ת
                UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;      // ��ͣ�ϴ�
                break;
            case UIS_TOKEN_IN | 1:                                             // endpoint 1# �ж϶˵��ϴ�
                UEP1_CTRL ^= bUEP_T_TOG;                                       // �ֶ���ת
                UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;      // ��ͣ�ϴ�
                break;
            case UIS_TOKEN_SETUP | 0:                                          // endpoint 0# SETUP
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
				len = USB_RX_LEN;
                if ( len == sizeof( USB_SETUP_REQ ) )                          // SETUP������
                {
                    SetupLen = UsbSetupBuf->wLengthL;
                    if ( UsbSetupBuf->wLengthH || SetupLen > 0x7F )
                    {
                        SetupLen = 0x7F;                                       // �����ܳ���
                    }
                    len = 0;                                                   // Ĭ��Ϊ�ɹ������ϴ�0����
                    if ( ( UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )/* ֻ֧�ֱ�׼���� */
                    {
                        len = 0xFF;                                            // ����ʧ��
                        printf("ErrEp0ReqType=%02X\n",(UINT16)UsbSetupBuf->bRequestType);
                    }
                    else                                                       // ��׼����
                    {
                        SetupReqCode = UsbSetupBuf->bRequest;
                        switch( SetupReqCode )                                 // ������
                        {
                        case USB_GET_DESCRIPTOR:
                            switch( UsbSetupBuf->wValueH )
                            {
                            case 1:                                            // �豸������
                                pDescr = (PUINT8)( &MyDevDescr[0] );
                                len = sizeof( MyDevDescr );
                                break;
                            case 2:                                            // ����������
                                pDescr = (PUINT8)( &MyCfgDescr[0] );
                                len = sizeof( MyCfgDescr );
                                break;
                            case 3:                                            // �ַ���������
                                switch( UsbSetupBuf->wValueL )
                                {
                                case 1:
                                    pDescr = (PUINT8)( &MyManuInfo[0] );       
                                    len = sizeof( MyManuInfo );
                                    break;
                                case 2:
                                    pDescr = (PUINT8)( &MyProdInfo[0] );        
                                    len = sizeof( MyProdInfo );
                                    break;
                                case 0:
                                    pDescr = (PUINT8)( &MyLangDescr[0] );
                                    len = sizeof( MyLangDescr );
                                    break;
                                default:
                                    len = 0xFF;                                 // ��֧�ֵ��ַ���������
                                    break;
                                }
                                break;
                            default:
                                len = 0xFF;                                     // ��֧�ֵ�����������
                                break;
                            }
                            if ( SetupLen > len )
                            {
                                SetupLen = len;                                 // �����ܳ���
                            }
                            len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;  // ���δ��䳤��
                            memcpy( Ep0Buffer, pDescr, len );                   /* �����ϴ����� */
                            SetupLen -= len;
                            pDescr += len;
                            break;
                        case USB_SET_ADDRESS:
                            SetupLen = UsbSetupBuf->wValueL;                    // �ݴ�USB�豸��ַ
                            break;
                        case USB_GET_CONFIGURATION:
                            Ep0Buffer[0] = UsbConfig;
                            if ( SetupLen >= 1 )
                            {
                                len = 1;
                            }
                            break;
                        case USB_SET_CONFIGURATION:
                            UsbConfig = UsbSetupBuf->wValueL;
                            if ( UsbConfig )
                            {
                                LED_CFG = 0;
                            }
                            else
                            {
                                LED_CFG = 1;
                            }
                            break;
                        case USB_CLEAR_FEATURE:
                            if ( ( UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )// �˵�
                            {
                                switch( UsbSetupBuf->wIndexL )
                                {
                                case 0x82:
                                    UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                                    break;
                                case 0x02:
                                    UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                                    break;
                                case 0x81:
                                    UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                                    break;
                                case 0x01:
                                    UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                                    break;
                                default:
                                    len = 0xFF;                                            // ��֧�ֵĶ˵�
                                    break;
                                }
                            }
                            else
                            {
                                len = 0xFF;                                                // ���Ƕ˵㲻֧��
                            }
                            break;
                        case USB_SET_FEATURE:                                              /* Set Feature */
                        if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x00 )                 /* �����豸 */
                        {
                          if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x01 )
                          {
                            if( MyCfgDescr[ 7 ] & 0x20 )
                            {
                                 /* ���û���ʹ�ܱ�־ */
                            }
                            else
                            {
                                len = 0xFF;                                                 /* ����ʧ�� */
                            }
                          }
                          else
                          {
                            len = 0xFF;                                                     /* ����ʧ�� */
                          }
                        }
                        else if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x02 )             /* ���ö˵� */
                        {
                          if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x00 )
                          {
                            switch( ( ( UINT16 )UsbSetupBuf->wIndexH << 8 ) | UsbSetupBuf->wIndexL )
                            {
                                case 0x82:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* ���ö˵�2 IN STALL */
                                    break;

                                case 0x02:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;/* ���ö˵�2 OUT Stall */
                                    break;

                                case 0x81:
                                    UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* ���ö˵�1 IN STALL */
                                    break;

                                default:
                                    len = 0xFF;                                    /* ����ʧ�� */
                                    break;
                              }
                             }
                             else
                             {
                               len = 0xFF;                                          /* ����ʧ�� */
                             }
                           }
                           else
                            {
                             len = 0xFF;                                             /* ����ʧ�� */
                            }
                            break;													
                        case USB_GET_INTERFACE:
                            Ep0Buffer[0] = 0x00;
                            if ( SetupLen >= 1 )
                            {
                                len = 1;
                            }
                            break;
                        case USB_GET_STATUS:
                            Ep0Buffer[0] = 0x00;
                            Ep0Buffer[1] = 0x00;
                            if ( SetupLen >= 2 )
                            {
                                len = 2;
                            }
                            else
                            {
                                len = SetupLen;
                            }
                            break;
                        default:
                            len = 0xFF;                                             // ����ʧ��
                            printf("ErrEp0ReqCode=%02X\n",(UINT16)SetupReqCode);
                            break;
                        }
                    }
                }
                else
                {
                    len = 0xFF;                                                    // SETUP�����ȴ���
                    printf("ErrEp0ReqSize\n");
                }
                if ( len == 0xFF )                                                 // ����ʧ��
                {
                    SetupReqCode = 0xFF;
                    UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;// STALL
                }
                else if ( len <= THIS_ENDP0_SIZE )                                 // �ϴ����ݻ���״̬�׶η���0���Ȱ�
                {
                    UEP0_T_LEN = len;
                    UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;// Ĭ�����ݰ���DATA1
                }
                else                                                               // �´����ݻ�����
                {
                    UEP0_T_LEN = 0;                                                // ��Ȼ��δ��״̬�׶Σ�������ǰԤ���ϴ�0�������ݰ��Է�������ǰ����״̬�׶�
                    UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;// Ĭ�����ݰ���DATA1
                }
                break;
            case UIS_TOKEN_IN | 0:                                                 // endpoint 0# IN
                switch( SetupReqCode )
                {
                case USB_GET_DESCRIPTOR:
                    len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen; // ���δ��䳤��
                    memcpy( Ep0Buffer, pDescr, len );                               /* �����ϴ����� */
                    SetupLen -= len;
                    pDescr += len;
                    UEP0_T_LEN = len;
                    UEP0_CTRL ^= bUEP_T_TOG;                                        // ��ת
                    break;
                case USB_SET_ADDRESS:
                    USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
                    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                    break;
                default:
                    UEP0_T_LEN = 0;                                                 // ״̬�׶�����жϻ�����ǿ���ϴ�0�������ݰ��������ƴ���
                    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                    break;
                }
                break;
            case UIS_TOKEN_OUT | 0:                                                 // endpoint 0# OUT
                switch( SetupReqCode )
                {
//                      case download:
//                          if ( U_TOG_OK ) {                                       // ��ͬ�������ݰ�������
//                              UEP0_CTRL ^= bUEP_R_TOG;                            // ��ת
//                              get_data;
//                              //UEP0_CTRL = UEP0_CTRL & bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;// Ԥ���ϴ�0�������ݰ�DATA1�Է�������ǰ����״̬�׶�
//                          }
//                          break;
                case USB_GET_DESCRIPTOR:
                default:
                    if ( U_TOG_OK )                                                 // ��ͬ�������ݰ�������
                    {
//                              if ( USB_RX_LEN ) control_status_error;
//                              else control_ok;                                    // �յ�0���Ȱ���ʾ���ƶ�����/�ϴ�OK
                    }
//                          else control_status_error;
                    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;                      // ׼����һ���ƴ���
                    break;
                }
                break;
            default:
                printf("ErrEndp INT\n");
                break;
            }
        }
        UIF_TRANSFER = 0;                                                           // ���жϱ�־
    }
    else if ( UIF_BUS_RST )                                                         // USB���߸�λ
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP1_CTRL = UEP_T_RES_NAK;
        UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        USB_DEV_AD = 0x00;
        LED_CFG = 1;
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
        UIF_BUS_RST = 0;                                                            // ���жϱ�־
    }
    else if ( UIF_SUSPEND )                                                         // USB���߹���/�������
    {
        UIF_SUSPEND = 0;
        if ( USB_MIS_ST & bUMS_SUSPEND )                                            // ����
        {
            printf( "Zzz" );                                                        // ˯��״̬
            LED_RUN = 1;
            while ( XBUS_AUX & bUART0_TX );                                         // �ȴ��������
            SAFE_MOD = 0x55;
            SAFE_MOD = 0xAA;
            WAKE_CTRL = bWAK_BY_USB | bWAK_RXD0_LO;                                 // USB����RXD0���ź�ʱ�ɱ�����
            PCON |= PD;                                                             // ˯��
            SAFE_MOD = 0x55;
            SAFE_MOD = 0xAA;
            WAKE_CTRL = 0x00;
        }
        else                                                                        // ����
        {
            LED_RUN = 0;
        }
    }
    else 
    {                                                                               // ������ж�,�����ܷ��������
        printf("Unknown INT\n");
        USB_INT_FG = 0xFF;                                                          // ���жϱ�־
    }
    LED_TMP = 1;
}

void    InitUSB_Device( void )                                                      // ��ʼ��USB�豸
{
    IE_USB = 0;
    LED_CFG = 1;
    LED_RUN = 0;
    USB_CTRL = 0x00;                                                                // ���趨ģʽ
    UEP4_1_MOD = bUEP1_TX_EN;                                                       // �˵�1�ϴ�IN
    UEP2_3_MOD = bUEP2_RX_EN | bUEP2_TX_EN;                                         // �˵�2�´�OUT���ϴ�IN
    UEP0_DMA = Ep0Buffer;
    UEP1_DMA = Ep1Buffer;
    UEP2_DMA = Ep2Buffer;
    USB_DEV_AD = 0x00;
    UDEV_CTRL = bUD_DP_PD_DIS | bUD_DM_PD_DIS;                                      // ��ֹDP/DM��������
    USB_CTRL = bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;                           // ����USB�豸��DMA�����ж��ڼ��жϱ�־δ���ǰ�Զ�����NAK
    UDEV_CTRL |= bUD_PORT_EN;                                                       // ����USB�˿�
    USB_INT_FG = 0xFF;                                                              // ���жϱ�־
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
    IE_USB = 1;
}

main( )
{
    UINT8   i;
    mDelaymS(30);                                                                   //�ϵ���ʱ,�ȴ��ڲ������ȶ�,�ؼ�
    mInitSTDIO( );                                                                  /* Ϊ���ü����ͨ�����ڼ����ʾ���� */
    printf( "Start @ChipID=%02X\n", (UINT16)CHIP_ID );
    InitUSB_Device( );
    EA = 1;
    while ( 1 )
    {
        i = getkey( );
        printf( "%c", (UINT8)i );
        if ( i >= '0' && i <= 'z' )
        {
            while(( UEP1_CTRL & MASK_UEP_T_RES ) == UEP_T_RES_ACK);                 //�ȴ��ϴ���ɣ�����ͬʱ��д������					
            memcpy( Ep1Buffer, (PUINT8C)(i-'0'), MAX_PACKET_SIZE );                 /* �����ϴ����� */
            UEP1_T_LEN = i-'0' > 8 ? 8 : i-'0';
            UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;
        }
    }
}