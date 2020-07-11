#if defined(__cplusplus)
 extern "C" {
#endif

#ifndef __APP_PROTOCOL_H_
#define __APP_PROTOCOL_H_
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>         /* for 'NULL' */
#include "stm32f4xx.h"

#include	"Protocol_format.h"



#define CO_LITTLE_ENDIAN
/* Critical sections */
#define CO_LOCK_CAN_SEND()          __set_PRIMASK(1);
#define CO_UNLOCK_CAN_SEND()        __set_PRIMASK(0);

#define CO_LOCK_EMCY()              __set_PRIMASK(1);
#define CO_UNLOCK_EMCY()            __set_PRIMASK(0);

#define CO_LOCK_OD()                __set_PRIMASK(1);
#define CO_UNLOCK_OD()              __set_PRIMASK(0);

/* Data types */

#define bool_t bool
    typedef float                   float32_t;
    typedef long double             float64_t;
    typedef char                    char_t;
    typedef unsigned char           oChar_t;
    typedef unsigned char           domain_t;

 /**
 * Helper union for manipulating data bytes.
 */
typedef union{
    uint8_t  u8[8];  /**< 8 bytes */
    uint16_t u16[4]; /**< 4 words */
    uint32_t u32[2]; /**< 2 double words */
}CO_bytes_t;
 
#pragma pack(push,1)
typedef   struct   //__Pack	__packed
{
	    /** application layer command type . */
    uint8_t             command;
    /** The index of Object from 0x1000 to 0xFFFF */
    uint16_t            index;
    /** Number of (sub-objects - 1). If Object Type is variable, then
    maxSubIndex is 0, otherwise maxSubIndex is equal or greater than 1. */
    uint8_t             maxSubIndex;
    /* app layer data length  */
    uint16_t            length;
    /** pData is pointer to Array  */
    uint8_t             *pData;
}__attribute__((packed))App_Layer_Format;
#pragma pack(pop)

#define		APP_LAYER_HEADER		sizeof(uint8_t)+sizeof(uint16_t)+sizeof(uint8_t)+sizeof(uint16_t)

/**
 * SDO buffer size.
 *
 * Size of the internal SDO buffer.
 *
 * Size must be at least equal to size of largest variable in @ref CO_SDO_objectDictionary.
 * If data type is domain, data length is not limited to SDO buffer size. If
 * block transfer is implemented, value should be set to 889.
 *
 * Value can be in range from 7 to 889 bytes.
 */
 
    #ifndef CO_SDO_BUFFER_SIZE
        #define CO_SDO_BUFFER_SIZE    44*52*sizeof(uint16_t)+sizeof(App_Layer_Format)
    #endif
	

#define	MAX_MUL_FRAME_CNT			CO_SDO_BUFFER_SIZE / FRAME_BUFFER_SIZE + 1
				
#define	WRITE_OD_MAX_SIZE			sizeof(uint32_t)
	
				

/**
 * Object Dictionary attributes. Bit masks for attribute in CO_OD_entry_t.
 */
typedef enum{
    CO_ODA_MEM_ROM          = 0x0001U,  /**< Variable is located in ROM memory */
    CO_ODA_MEM_RAM          = 0x0002U,  /**< Variable is located in RAM memory */
    CO_ODA_MEM_EEPROM       = 0x0003U,  /**< Variable is located in EEPROM memory */
    CO_ODA_READABLE         = 0x0004U,  /**< SDO server may read from the variable */
    CO_ODA_WRITEABLE        = 0x0008U,  /**< SDO server may write to the variable */
    CO_ODA_RPDO_MAPABLE     = 0x0010U,  /**< Variable is mappable for RPDO */
    CO_ODA_TPDO_MAPABLE     = 0x0020U,  /**< Variable is mappable for TPDO */
    CO_ODA_TPDO_DETECT_COS  = 0x0040U,  /**< If variable is mapped to any PDO, then
                                             PDO is automatically send, if variable
                                             changes its value */
    CO_ODA_MB_VALUE         = 0x0080U   /**< True when variable is a multibyte value */
}attributes_t;


/**
 * SDO abort codes.
 *
 * Send with Abort SDO transfer message.
 *
 * The abort codes not listed here are reserved.
 */
typedef enum{
    CO_SDO_AB_NONE                  = 0x00000000UL, /**< 0x00000000, No abort */
    CO_SDO_AB_TOGGLE_BIT            = 0x05030000UL, /**< 0x05030000, Toggle bit not altered */
    CO_SDO_AB_TIMEOUT               = 0x05040000UL, /**< 0x05040000, SDO protocol timed out */
    CO_SDO_AB_CMD                   = 0x05040001UL, /**< 0x05040001, Command specifier not valid or unknown */
    CO_SDO_AB_BLOCK_SIZE            = 0x05040002UL, /**< 0x05040002, Invalid block size in block mode */
    CO_SDO_AB_SEQ_NUM               = 0x05040003UL, /**< 0x05040003, Invalid sequence number in block mode */
    CO_SDO_AB_CRC                   = 0x05040004UL, /**< 0x05040004, CRC error (block mode only) */
    CO_SDO_AB_OUT_OF_MEM            = 0x05040005UL, /**< 0x05040005, Out of memory */
    CO_SDO_AB_UNSUPPORTED_ACCESS    = 0x06010000UL, /**< 0x06010000, Unsupported access to an object */
    CO_SDO_AB_WRITEONLY             = 0x06010001UL, /**< 0x06010001, Attempt to read a write only object */
    CO_SDO_AB_READONLY              = 0x06010002UL, /**< 0x06010002, Attempt to write a read only object */
    CO_SDO_AB_NOT_EXIST             = 0x06020000UL, /**< 0x06020000, Object does not exist */
    CO_SDO_AB_NO_MAP                = 0x06040041UL, /**< 0x06040041, Object cannot be mapped to the PDO */
    CO_SDO_AB_MAP_LEN               = 0x06040042UL, /**< 0x06040042, Number and length of object to be mapped exceeds PDO length */
    CO_SDO_AB_PRAM_INCOMPAT         = 0x06040043UL, /**< 0x06040043, General parameter incompatibility reasons */
    CO_SDO_AB_DEVICE_INCOMPAT       = 0x06040047UL, /**< 0x06040047, General internal incompatibility in device */
    CO_SDO_AB_HW                    = 0x06060000UL, /**< 0x06060000, Access failed due to hardware error */
    CO_SDO_AB_TYPE_MISMATCH         = 0x06070010UL, /**< 0x06070010, Data type does not match, length of service parameter does not match */
    CO_SDO_AB_DATA_LONG             = 0x06070012UL, /**< 0x06070012, Data type does not match, length of service parameter too high */
    CO_SDO_AB_DATA_SHORT            = 0x06070013UL, /**< 0x06070013, Data type does not match, length of service parameter too short */
    CO_SDO_AB_SUB_UNKNOWN           = 0x06090011UL, /**< 0x06090011, Sub index does not exist */
    CO_SDO_AB_INVALID_VALUE         = 0x06090030UL, /**< 0x06090030, Invalid value for parameter (download only). */
    CO_SDO_AB_VALUE_HIGH            = 0x06090031UL, /**< 0x06090031, Value range of parameter written too high */
    CO_SDO_AB_VALUE_LOW             = 0x06090032UL, /**< 0x06090032, Value range of parameter written too low */
    CO_SDO_AB_MAX_LESS_MIN          = 0x06090036UL, /**< 0x06090036, Maximum value is less than minimum value. */
    CO_SDO_AB_NO_RESOURCE           = 0x060A0023UL, /**< 0x060A0023, Resource not available: SDO connection */
    CO_SDO_AB_GENERAL               = 0x08000000UL, /**< 0x08000000, General error */
    CO_SDO_AB_DATA_TRANSF           = 0x08000020UL, /**< 0x08000020, Data cannot be transferred or stored to application */
    CO_SDO_AB_DATA_LOC_CTRL         = 0x08000021UL, /**< 0x08000021, Data cannot be transferred or stored to application because of local control */
    CO_SDO_AB_DATA_DEV_STATE        = 0x08000022UL, /**< 0x08000022, Data cannot be transferred or stored to application because of present device state */
    CO_SDO_AB_DATA_OD               = 0x08000023UL, /**< 0x08000023, Object dictionary not present or dynamic generation fails */
    CO_SDO_AB_NO_DATA               = 0x08000024UL  /**< 0x08000024, No data available */
}CO_SDO_abortCode_t;


/**
 * Internal states of the SDO server state machine
 */
typedef enum {
    CO_SDO_ST_IDLE                  = 0x00U,
    CO_SDO_ST_DOWNLOAD_INITIATE     = 0x11U,
    CO_SDO_ST_DOWNLOAD_SEGMENTED    = 0x12U,
    CO_SDO_ST_DOWNLOAD_BL_INITIATE  = 0x14U,
    CO_SDO_ST_DOWNLOAD_BL_SUBBLOCK  = 0x15U,
    CO_SDO_ST_DOWNLOAD_BL_SUB_RESP  = 0x16U,
    CO_SDO_ST_DOWNLOAD_BL_END       = 0x17U,
    CO_SDO_ST_UPLOAD_INITIATE       = 0x21U,
    CO_SDO_ST_UPLOAD_SEGMENTED      = 0x22U,
    CO_SDO_ST_UPLOAD_BL_INITIATE    = 0x24U,
    CO_SDO_ST_UPLOAD_BL_INITIATE_2  = 0x25U,
    CO_SDO_ST_UPLOAD_BL_SUBBLOCK    = 0x26U,
    CO_SDO_ST_UPLOAD_BL_END         = 0x27U
}CMD_state_t;


/**
 * Common DS301 object dictionary entries.
 */
typedef enum{
    OD_H1000_DEV_TYPE             = 0x1000U,/**< Device type */
    OD_H1001_ERR_REG              = 0x1001U,/**< Error register */
    OD_H1002_MANUF_STATUS_REG     = 0x1002U,/**< Manufacturer status register */
    OD_H1003_PREDEF_ERR_FIELD     = 0x1003U,/**< Predefined error field */
    OD_H1004_RSV                  = 0x1004U,/**< Reserved */
    OD_H1005_COBID_SYNC           = 0x1005U,/**< Sync message cob-id */
    OD_H1006_COMM_CYCL_PERIOD     = 0x1006U,/**< Communication cycle period */
    OD_H1007_SYNC_WINDOW_LEN      = 0x1007U,/**< Sync windows length */
    OD_H1008_MANUF_DEV_NAME       = 0x1008U,/**< Manufacturer device name */
    OD_H1009_MANUF_HW_VERSION     = 0x1009U,/**< Manufacturer hardware version */
    OD_H100A_MANUF_SW_VERSION     = 0x100AU,/**< Manufacturer software version */
    OD_H100B_RSV                  = 0x100BU,/**< Reserved */
    OD_H100C_GUARD_TIME           = 0x100CU,/**< Guard time */
    OD_H100D_LIFETIME_FACTOR      = 0x100DU,/**< Life time factor */
    OD_H100E_RSV                  = 0x100EU,/**< Reserved */
    OD_H100F_RSV                  = 0x100FU,/**< Reserved */
    OD_H1010_STORE_PARAM_FUNC     = 0x1010U,/**< Store parameter in persistent memory function */
    OD_H1011_REST_PARAM_FUNC      = 0x1011U,/**< Restore default parameter function */
    OD_H1012_COBID_TIME           = 0x1012U,/**< Timestamp message cob-id */
    OD_H1013_HIGH_RES_TIMESTAMP   = 0x1013U,/**< High resolution timestamp */
    OD_H1014_COBID_EMERGENCY      = 0x1014U,/**< Emergency message cob-id */
    OD_H1015_INHIBIT_TIME_MSG     = 0x1015U,/**< Inhibit time message */
    OD_H1016_CONSUMER_HB_TIME     = 0x1016U,/**< Consumer heartbeat time */
    OD_H1017_PRODUCER_HB_TIME     = 0x1017U,/**< Producer heartbeat time */
    OD_H1018_IDENTITY_OBJECT      = 0x1018U,/**< Identity object */
    OD_H1019_SYNC_CNT_OVERFLOW    = 0x1019U,/**< Sync counter overflow value */
    OD_H1020_VERIFY_CONFIG        = 0x1020U,/**< Verify configuration */
    OD_H1021_STORE_EDS            = 0x1021U,/**< Store EDS */
    OD_H1022_STORE_FORMAT         = 0x1022U,/**< Store format */
    OD_H1023_OS_CMD               = 0x1023U,/**< OS command */
    OD_H1024_OS_CMD_MODE          = 0x1024U,/**< OS command mode */
    OD_H1025_OS_DBG_INTERFACE     = 0x1025U,/**< OS debug interface */
    OD_H1026_OS_PROMPT            = 0x1026U,/**< OS prompt */
    OD_H1027_MODULE_LIST          = 0x1027U,/**< Module list */
    OD_H1028_EMCY_CONSUMER        = 0x1028U,/**< Emergency consumer object */
    OD_H1029_ERR_BEHAVIOR         = 0x1029U,/**< Error behaviour */
    OD_H1200_SDO_SERVER_PARAM     = 0x1200U,/**< SDO server parameters */
    OD_H1280_SDO_CLIENT_PARAM     = 0x1280U,/**< SDO client parameters */
    OD_H1400_RXPDO_1_PARAM        = 0x1400U,/**< RXPDO communication parameter */
    OD_H1401_RXPDO_2_PARAM        = 0x1401U,/**< RXPDO communication parameter */
    OD_H1402_RXPDO_3_PARAM        = 0x1402U,/**< RXPDO communication parameter */
    OD_H1403_RXPDO_4_PARAM        = 0x1403U,/**< RXPDO communication parameter */
    OD_H1600_RXPDO_1_MAPPING      = 0x1600U,/**< RXPDO mapping parameters */
    OD_H1601_RXPDO_2_MAPPING      = 0x1601U,/**< RXPDO mapping parameters */
    OD_H1602_RXPDO_3_MAPPING      = 0x1602U,/**< RXPDO mapping parameters */
    OD_H1603_RXPDO_4_MAPPING      = 0x1603U,/**< RXPDO mapping parameters */
    OD_H1800_TXPDO_1_PARAM        = 0x1800U,/**< TXPDO communication parameter */
    OD_H1801_TXPDO_2_PARAM        = 0x1801U,/**< TXPDO communication parameter */
    OD_H1802_TXPDO_3_PARAM        = 0x1802U,/**< TXPDO communication parameter */
    OD_H1803_TXPDO_4_PARAM        = 0x1803U,/**< TXPDO communication parameter */
    OD_H1A00_TXPDO_1_MAPPING      = 0x1A00U,/**< TXPDO mapping parameters */
    OD_H1A01_TXPDO_2_MAPPING      = 0x1A01U,/**< TXPDO mapping parameters */
    OD_H1A02_TXPDO_3_MAPPING      = 0x1A02U,/**< TXPDO mapping parameters */
    OD_H1A03_TXPDO_4_MAPPING      = 0x1A03U /**< TXPDO mapping parameters */
}CO_ObjDicId_t;


 


/**
 * Object for record type entry in @ref CO_SDO_objectDictionary.
 *
 * See CO_OD_entry_t.
 */
typedef struct{
    /** See #CO_SDO_OD_attributes_t */
    void               *pData;
    /** Length of variable in bytes. If object type is Domain, length is zero */
    uint16_t            attribute;
    /** Pointer to data. If object type is Domain, pData is null */
    uint16_t            length;
}entryRecord_t;


/**
 * Object for one entry with specific index in @ref CO_SDO_objectDictionary.
 */
typedef struct {
    /** The index of Object from 0x1000 to 0xFFFF */
    uint16_t            index;
    /** Number of (sub-objects - 1). If Object Type is variable, then
    maxSubIndex is 0, otherwise maxSubIndex is equal or greater than 1. */
    uint16_t            maxSubIndex;
    /** If Object Type is record, attribute is set to zero. Attribute for
    each member is then set in special array with members of type
    CO_OD_entryRecord_t. If Object Type is Array, attribute is common for
    all array members. See #CO_SDO_OD_attributes_t. */
    uint16_t            attribute;
    /** If Object Type is Variable, length is the length of variable in bytes.
    If Object Type is Array, length is the length of one array member.
    If Object Type is Record, length is zero. Length for each member is
    set in special array with members of type CO_OD_entryRecord_t.
    If Object Type is Domain, length is zero. Length is specified
    by application in @ref CO_SDO_OD_function. */
    uint16_t            length;
    /** If Object Type is Variable, pData is pointer to data.
    If Object Type is Array, pData is pointer to data. Data doesn't
    include Sub-Object 0.
    If object type is Record, pData is pointer to special array
    with members of type CO_OD_entryRecord_t.
    If object type is Domain, pData is null. */
    void               *pData;
}entry_t;




/**
 * Object contains all information about the object being transferred by SDO server.
 *
 * Object is used as an argument to @ref CO_SDO_OD_function. It is also
 * part of the CO_SDO_t object.
 */
typedef struct{
    /** Informative parameter. It may point to object, which is connected
    with this OD entry. It can be used inside @ref CO_SDO_OD_function, ONLY
    if it was registered by CO_OD_configure() function before. */
    void               *object;
    /** SDO data buffer contains data, which are exchanged in SDO transfer.
    @ref CO_SDO_OD_function may verify or manipulate that data before (after)
    they are written to (read from) Object dictionary. Data have the same
    endianes as processor. Pointer must NOT be changed. (Data up to length
    can be changed.) */
    uint8_t            *data;
    /** Pointer to location in object dictionary, where data are stored.
    (informative reference to old data, read only). Data have the same
    endianes as processor. If data type is Domain, this variable is null. */
    const void         *ODdataStorage;
    /** Length of data in the above buffer. Read only, except for domain. If
    data type is domain see @ref CO_SDO_OD_function for special rules by upload. */
    uint16_t            dataLength;
    /** Attribute of object in Object dictionary (informative, must NOT be changed). */
    uint16_t            attribute;
    /** Pointer to the #CO_SDO_OD_flags_t byte. */
    uint8_t            *pFlags;
    /** Index of object in Object dictionary (informative, must NOT be changed). */
    uint16_t            index;
    /** Subindex of object in Object dictionary (informative, must NOT be changed). */
    uint8_t             subIndex;
    /** True, if SDO upload is in progress, false if SDO download is in progress. */
    bool_t              reading;
    /** Used by domain data type. Indicates the first segment. Variable is informative. */
    bool_t              firstSegment;
    /** Used by domain data type. If false by download, then application will
    receive more segments during SDO communication cycle. If uploading,
    application may set variable to false, so SDO server will call
    @ref CO_SDO_OD_function again for filling the next data. */
    bool_t              lastSegment;
    /** Used by domain data type. By upload @ref CO_SDO_OD_function may write total
    data length, so this information will be send in SDO upload initiate phase. It
    is not necessary to specify this variable. By download this variable contains
    total data size, if size is indicated in SDO download initiate phase */
    uint32_t            dataLengthTotal;
    /** Used by domain data type. In case of multiple segments, this indicates the offset
    into the buffer this segment starts at. */
    uint32_t            offset;
}CO_ODF_arg_t;


/**
 * Object is used as array inside CO_SDO_t, parallel to @ref CO_SDO_objectDictionary.
 *
 * Object is generated by function CO_OD_configure(). It is then used as
 * extension to Object dictionary entry at specific index.
 */
typedef struct{
    /** Pointer to @ref CO_SDO_OD_function */
    CO_SDO_abortCode_t (*pODFunc)(CO_ODF_arg_t *ODF_arg);
    /** Pointer to object, which will be passed to @ref CO_SDO_OD_function */
    void               *object;
    /** Pointer to #CO_SDO_OD_flags_t. If object type is array or record, this
    variable points to array with length equal to number of subindexes. */
    uint8_t            *flags;
}extension_t;


/**
 * APP PROTOCOL server object.
 */

typedef struct{
    /** data bytes of the received message. */
   // uint8_t             *rxData;
    /** SDO data buffer of size, application layer work circute . */
    uint8_t             rxDataBuffer[FRAME_BUFFER_SIZE];									
    /** Internal flag indicates, that this object has own OD */
    bool_t              ownOD;
    /** Pointer to the @ref CO_SDO_objectDictionary (array) */
    const entry_t *OD;
    /** Size of the @ref CO_SDO_objectDictionary */
    uint16_t            ODSize;
    /** Pointer to array of CO_OD_extension_t objects. Size of the array is
    equal to ODSize. */
    extension_t  *ODExtensions;

    /** Sequence number of OD entry as returned from CO_OD_find() */
    uint16_t            entryNo;
	
    /** CO_ODF_arg_t object with additional variables. Reference to this object
    is passed to @ref CO_SDO_OD_function */
    CO_ODF_arg_t        ODF_arg;
	
    /** From CO_SDO_init() */
    uint32_t             nodeId;
    /** Current internal state of the SDO server state machine #CMD_state_t */
    CMD_state_t      state;
    /** Toggle bit in segmented transfer or block sequence in block transfer */
    uint8_t             sequence;
    /** Timeout timer for SDO communication */
    uint16_t            timeoutTimer;
    /** Number of segments per block with 1 <= blksize <= 127 */
    uint8_t             blksize;
    /** True, if CRC calculation by block transfer is enabled */
    bool_t              crcEnabled;
    /** Calculated CRC code */
    uint32_t            crc;
    /** Length of data in the last segment in block upload */
    uint8_t             lastLen;
    /** Indication end of block transfer */
    bool_t              endOfTransfer;
    /** Variable indicates, if new SDO message received from CAN bus */
    bool_t              rxNew;
    /** From CO_SDO_initCallback() or NULL */
    void              (*pFunctSignal)(void);
    /** From CO_SDO_init() */
    CO_socket_t     *SocketDevTx; 
    /** transmit buffer inside CANdev for CAN tx message */
    uint8_t         txDataBuffer[CO_SDO_BUFFER_SIZE];
    /** Offset in buffer of data segment being written */
    uint16_t            txBufferOffset;
	
}CO_SDO_t;

/**  change callback funtion interface	*/
void CO_SDO_initCallback(
        CO_SDO_t               *SDO,
        void                  (*pFunctSignal)(void));

/*
 * Read received message from socket module.
 * Function will be called (by socket receive ) every time, when CAN
 * message with correct identifier will be received. For more information and
 * description of parameters see file Protocol.h.
 * 
 */
 
 void CO_SDO_receive(void *object, void *msg);


#endif

#ifdef __cplusplus
     }
#endif
