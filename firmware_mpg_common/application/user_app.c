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
  u8 au8WelcomeMessage[] = "ANT Master";

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
  u8 au8FollowMessage[]="Follow the other";
  
  static u8 au8TestMessage[] = {0, 0, 0, 0, 0, 0, 0, 0};
  static u8 au8CurrentMessage[8]= {0};
  static u8 u8Count=0;
  
  static u8 au8Follow[8];
  static u8 u8FollowCount=0;
  static u8 au8Receive[8]={0};
  
  
  static u8 u8signal=0;
  
  static u16 u16delay=0;
  static u16 u16Time=0;
  static u8 u8button_en=0;
  u16Time++;
  
if(u8signal==0)//Send
{

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
  
  if(u8Count==7)
  {
    u8Count=0;
  }
}//end if(signal==0)

  if(u8signal==1)//Receive
  {
    
    LCDMessage(LINE1_START_ADDR,au8FollowMessage);
    if( WasButtonPressed(BUTTON0))
    {
     au8Follow[u8FollowCount]=1;
     u8FollowCount++;
    }
    if(WasButtonPressed(BUTTON1))
    {
      au8Follow[u8FollowCount]=2;
      u8FollowCount++;
    }
    if(WasButtonPressed(BUTTON2))
    {
      au8Follow[u8FollowCount]=3;
      u8FollowCount++;
    }
    if(WasButtonPressed(BUTTON3))
    {
      au8Follow[u8FollowCount]=4;
      u8FollowCount++;  
    }
  }//end if(signal==1)
 
  for(u8 i=0;i<8;i++)//compare
  {
    if(au8Follow[i]!=au8Receive[i])
    {
      LCDClearChars(LINE1_START_ADDR ,20);
      LCDMessage(LINE1_START_ADDR,au8WrongMessage);
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
        }
      }
      

#ifdef MPG1
      
#endif /* MPG1 */
      
#ifdef MPG2
#endif /* MPG2 */
      
    }
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
    }
  } /* end AntReadData() */
  
} /* end UserAppSM_Idle() */


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
