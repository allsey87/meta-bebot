#ifndef PACKET_CONTROL_INTERFACE_H
#define PACKET_CONTROL_INTERFACE_H

#define RX_COMMAND_BUFFER_LENGTH 32
#define TX_COMMAND_BUFFER_LENGTH 32

#define PREAMBLE1  0xF0
#define PREAMBLE2  0xCA
#define POSTAMBLE1 0x53
#define POSTAMBLE2 0x0F

#define PREAMBLE_SIZE 2
#define TYPE_FIELD_SIZE 1
#define DATA_LENGTH_FIELD_SIZE 1
#define CHECKSUM_FIELD_SIZE 1
#define POSTAMBLE_SIZE 2

#define NON_DATA_SIZE (PREAMBLE_SIZE + TYPE_FIELD_SIZE + DATA_LENGTH_FIELD_SIZE + \
                       CHECKSUM_FIELD_SIZE + POSTAMBLE_SIZE)

#define TYPE_OFFSET 2
#define DATA_LENGTH_OFFSET 3
#define DATA_START_OFFSET 4
#define CHECKSUM_OFFSET -3

#include "uart_socket.h"

class CPacketControlInterface {

public:
   enum class EState {
      SRCH_PREAMBLE1,
      SRCH_PREAMBLE2,
      SRCH_POSTAMBLE1,
      SRCH_POSTAMBLE2,
      RECV_COMMAND,
      BUF_OVERFLOW,
   }; 

   class CPacket {
   public:
      
      enum class EType : uint8_t {
         /*************************************/
         /* Sensor-Actuator Microcontroller   */
         /*************************************/
         /* Differential Drive System Packets */
         SET_DDS_ENABLE = 0x10,
         SET_DDS_SPEED  = 0x11,
         GET_DDS_SPEED  = 0x12,
         /* Accelerometer System Packets */
         GET_ACCEL_READING = 0x20,

         /*************************************/
         /* Power Management Microcontroller  */
         /*************************************/
         /* Switches */
         SET_SYSTEM_POWER_ENABLE = 0x40,
         SET_ACTUATOR_POWER_ENABLE = 0x41,

         /*************************************/
         /* Manipulator Microcontroller       */
         /*************************************/
         GET_CHARGER_STATUS = 0x60,
         /* Lift Actuator System */
         SET_LIFT_ACTUATOR_SPEED = 0x71,
         GET_LIMIT_SWITCH_STATE = 0x72,
         /* Electromagnet Subsystem */
         SET_EM_CHARGE_ENABLE = 0x80,
         SET_EM_DISCHARGE_MODE = 0x81,
         GET_EM_ACCUM_VOLTAGE = 0x82,
         /* Range Finders */
         GET_RF_RANGE = 0x90,
         GET_RF_AMBIENT = 0x91,
         /* NFC Control */
         SEND_NFC_MESSAGE = 0xA0,
         
         /*************************************/
         /* Invalid value for conversions     */
         /*************************************/
         INVALID = 0xFF
      };
      
   public:

      CPacket(uint8_t un_type_id,
              uint8_t un_data_length,
              uint8_t* pun_data) :
         m_unTypeId(un_type_id),
         m_unDataLength(un_data_length),
         m_punData(pun_data) {}
      
      EType GetType() const;
      
      bool HasData() const;
      uint8_t GetDataLength() const;
      const uint8_t* GetDataPointer() const;

   private: 
      uint8_t m_unTypeId;
      uint8_t m_unDataLength;
      uint8_t* m_punData;
   };

public:
   CPacketControlInterface(CUARTSocket& c_uart_socket) :
      m_eState(EState::SRCH_PREAMBLE1),
      m_unRxBufferPointer(0),
      m_unUsedBufferLength(0),
      m_unReparseOffset(RX_COMMAND_BUFFER_LENGTH),
      m_cPacket(0xFF, 0, 0),
      m_cUARTSocket(c_uart_socket) {}

   EState GetState() const;

   const char* StateToString(EState e_state) const;

   const CPacket& GetPacket() const;
  
   void ProcessInput();

   void Reset();

   void SendPacket(CPacket::EType e_type,
                   uint8_t* pun_tx_data = NULL,
                   uint8_t un_tx_data_length = 0);
      
private:
   uint8_t ComputeChecksum(uint8_t* pun_buf_data, uint8_t un_buf_length);

   EState m_eState;

   uint8_t m_unRxBufferPointer;
   uint8_t m_unUsedBufferLength;
   uint8_t m_unReparseOffset;
   uint8_t m_punRxBuffer[RX_COMMAND_BUFFER_LENGTH];
   
   CPacket m_cPacket;

   CUARTSocket& m_cUARTSocket;
   

};
   
#endif
   
