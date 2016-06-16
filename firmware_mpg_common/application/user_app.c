/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserAppInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserAppRunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserAppFlags;                       /* Global state flags */


static void  led_display_button(u8 * u8lastdata,u8 *u8new_data);

/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern AntSetupDataType G_stAntSetupData;                         /* From ant.c */

extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentData[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */

extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserAppInitialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserAppInitialize(void)
{
  u8 au8WelcomeMessage[] = "Please press buttons";

  /* Write a weclome message on the LCD */
#if MPG1
  /* Set a message up on the LCD. Delay is required to let the clear command send. */
  LCDCommand(LCD_CLEAR_CMD);
  for(u32 i = 0; i < 10000; i++);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage);
#endif /* MPG 1*/
  
#if 0 // untested for MPG2
  
#endif /* MPG2 */

 /* Configure ANT for this application */
  G_stAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  G_stAntSetupData.AntSerialLo         = ANT_SERIAL_LO_USERAPP;
  G_stAntSetupData.AntSerialHi         = ANT_SERIAL_HI_USERAPP;
  G_stAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  G_stAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  G_stAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  G_stAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  G_stAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  G_stAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  
  /* If good initialization, set state to Idle */
  if( AntChannelConfig(ANT_MASTER) )
  {
    AntOpenChannel();
    UserApp_StateMachine = UserAppSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp_StateMachine = UserAppSM_FailedInit;
  }

} /* end UserAppInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function UserAppRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserAppRunActiveState(void)
{
  UserApp_StateMachine();

} /* end UserAppRunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserAppSM_Idle(void)
{
  u8 au8PressMessage[] = "Please press buttons";
  u8 au8RightMessage[]="You are right!";
  u8 au8WrongMessage[]="Game over!";
  u8 au8FollowMessage[]="  Follow  the  other  ";
  
  static u8 au8TestMessage[] = {0, 0, 0, 0, 0, 0, 0, 0};
  static u8 au8CurrentMessage[8]= {0};
  static u8 u8Count=0;
  
  static u8 au8Follow[8];
  static u8 u8FollowCount=0;
  static u8 au8Receive[8]={0};
  
  
  static u8 u8signal=0;
  static u8 u8flag=0;
  
  static u16 u16delay=0;
  static u16 u16Time=0;
  static u8 u8button_en=0;
  u16Time++;
  
  static u8 u8NewData;
  static u8 u8Counter;
  static bool judge=FALSE;
  static u16 u16PressTime=0;
  static u16 u16FollowTime=0;
  static u8 u8DataCounter;
  
  static u8 u8WinCount=0;
if(u8signal==0)//Send
{
  u16PressTime++;
  if(u16PressTime>2000)
  {           
       u16PressTime=0;         
       LCDCommand(LCD_CLEAR_CMD);
       LCDMessage(LINE1_START_ADDR,au8PressMessage);             
   }

  if(u16delay>200)
    {
    
       LedOff(WHITE);
       LedOff(BLUE);
       LedOff(YELLOW);
       LedOff(RED);
    }
    else
    {
      u16delay++;
    }
  
 if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    LedOn(WHITE);
    au8TestMessage[u8Count]=1;
    u8Count++;
    u16delay=0;
    u16Time=0;
    u8button_en=1;
  }
 if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    LedOn(BLUE);
    au8TestMessage[u8Count]=2;
    u8Count++;
    u16delay=0;
    u16Time=0;
    u8button_en=1;
  }
 if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    LedOn(YELLOW);
    au8TestMessage[u8Count]=3;
    u8Count++;
    u16delay=0;
    u16Time=0;
    u8button_en=1;
  }
 if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    LedOn(RED);
    au8TestMessage[u8Count]=4;
    u8Count++;
    u16delay=0;
    u16Time=0;
    u8button_en=1;
  }
  
  if(u8Count==8)
  {
    u8Count=0;
  }
}//end if(signal==0)
if(u8signal==1)//Receive
{
  u16FollowTime++;
  if(u16FollowTime>2000)
  {
    u16FollowTime=0;
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR,au8FollowMessage);
  }// LCDClearChars(LINE1_START_ADDR+17,3);
  if( WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    au8Follow[u8FollowCount]=1;
    u8FollowCount++;
  }
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    au8Follow[u8FollowCount]=2;
    u8FollowCount++;
  }
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    au8Follow[u8FollowCount]=3;
    u8FollowCount++;
  } 
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    au8Follow[u8FollowCount]=4;
    u8FollowCount++;
  }
  if(u8DataCounter!=0)
  {
    if(u8DataCounter==u8FollowCount)
    {
      u8FollowCount=0;
      for(u8 k=0;k<u8DataCounter;k++)
      {
        if(au8Follow[k]==au8Receive[k])
        {
          u8Counter++;
        }
      }
      if(u8Counter==u8DataCounter)
      {
        judge=TRUE;
      }
      if(judge)
       {
         u8signal=0;
         //LCDCommand(LCD_CLEAR_CMD);
         LCDMessage(LINE2_START_ADDR+2,au8RightMessage);
         u8WinCount++;
       }
      else if(judge==FALSE)
      {
        LCDCommand(LCD_CLEAR_CMD);
         
        LCDMessage(LINE2_START_ADDR+4,au8WrongMessage);
        u8signal=2;
      }
    }
    u8DataCounter=0;
  }//end if u8DataCount!=0
  
  
  
  
  
}//end if(signal==1)

if(u8signal==2)
{
 AntCloseChannel();
}

if(u8NewData==1)
{
  led_display_button(au8Receive,&u8NewData) ;        
}

for(u8 m=0;m<8;m++)
{
  if(au8Receive[m]!=0)
  {
      u8DataCounter++;
   } 
} 

  
  
  if( AntReadData() )
  {
      
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      for(u8 i=0;i<8;i++)
      {
        if(au8Receive[i]!=G_au8AntApiCurrentData[i])
        {
          au8Receive[i]=G_au8AntApiCurrentData[i];
          u8NewData=1;
        }
      }
       
    }//end DATA
    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
     
      if((u16Time>2000)&&(u8button_en==1))
       { 
            u16Time =0;
           u8button_en=0;
           u8Count=0;
           u8signal=1;
           for(u8 u8count=0;u8count<8;u8count++)
           {
             au8CurrentMessage[u8count]=au8TestMessage[u8count];
             au8TestMessage[u8count]=0;
            }
       }
     AntQueueBroadcastMessage(au8CurrentMessage);
    }//end TICK
  } /* end AntReadData() */
  
} /* end UserAppSM_Idle() */

static void  led_display_button(u8 *u8lastdata,u8 *u8new_data)
{
  static u32 u32dataCount=0;
  static u8  datai=0;  
  if(u32dataCount==0)
  {   
    if(u8lastdata[datai]==0)
    {      
        if( datai++>6)
        {          
          datai=0;
          *u8new_data=0;
          LedOff(WHITE);
          LedOff(BLUE);
          LedOff(YELLOW);
          LedOff(RED);           
        }
        return;     
    }
    if(u8lastdata[datai]==1)
    {
        LedOn(WHITE);
        LedOff(BLUE);
        LedOff(YELLOW);
        LedOff(RED);                      
    }
    if(u8lastdata[datai]==2)
    {
          LedOff(WHITE);
          LedOn(BLUE);
          LedOff(YELLOW);
          LedOff(RED); 
    }
    if(u8lastdata[datai]==3)
    {
          LedOff(WHITE);
          LedOff(BLUE);
          LedOn(YELLOW);
          LedOff(RED); 
     }
     if(u8lastdata[datai]==4)
     {
          LedOff(WHITE);
          LedOff(BLUE);
          LedOff(YELLOW);
          LedOn(RED); 
     }    
  }//end if u32dataCount==200
  if(u32dataCount++>200)
  {   
    u32dataCount=0;
    if( datai++>7)
    {         
        datai=0;
        *u8new_data=0;
        LedOff(WHITE);
        LedOff(BLUE);
        LedOff(YELLOW);
        LedOff(RED);           
    }  
  }//end delay
 
}//end led_disply_button


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserAppSM_Error(void)          
{
  UserApp_StateMachine = UserAppSM_Idle;
  
} /* end UserAppSM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserAppSM_FailedInit(void)          
{
    
} /* end UserAppSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
