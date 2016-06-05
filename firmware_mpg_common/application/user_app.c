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
extern u8 G_au8DebugScanfBuffer[];                     /* From debug.c */
extern u8 G_u8DebugScanfCharCount;                     /* From debug.c  */

extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static u8 au8UserInputBuffer[USER_INPUT_BUFFER_SIZE];  /* Buffer for reading key input */

static fnCode_type UserApp_StateMachine;               /* The state machine function pointer */
static u32 UserApp_u32Timeout;                         /* Timeout counter used across states */

static u8 UserApp_au8MyName[] = "A3.leif"; 
static u8 aeLCDMessage[100];

static u8 u8NAMECount;

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
  
  LCDMessage(LINE1_START_ADDR, UserApp_au8MyName);     /*LCD Line 1 output My name*/
  LCDClearChars(LINE1_START_ADDR+7,11 );               /*Clean up LCD Line 1 of other content*/
  
  /*Backlight to purple*/
  LedOn(LCD_RED);
  LedOff(LCD_GREEN);
  LedOn(LCD_BLUE);
 
  PWMAudioSetFrequency(BUZZER1, 500);
  
  
   
   
  /* If good initialization, set state to Idle */
  if( 1 )
  {
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
  static u8 u8NumCharsMessage[] = "\n\rCharacters in buffer: ";
  static u8 u8NumNAMEMessage[]="\n\rleif,Your name has been detected:";
  static u8 u8EmptyMessage[]   = "\n\rCharacter count cleared!\r";
  u8 u8CharCount;
  
  static u16 u16TimeCounter=5000;
  
  static u8 u8Line2Adress=0;
  static u8 u8InputCount=0;    /*The variate to keep track of the total number of characters*/
  
  static u8 u8NAME[4];
  static u8 u8NAMECount=0;
  
  static u8 u8NAMETotalCount=0;
  
  static u8 Buzzer=0;
 
  
  u8CharCount = DebugScanf(au8UserInputBuffer);
  if(u8CharCount >0)
  {
    /*Display each character on LCD Line 2 starting from the left and going across the screen*/
     LCDMessage(LINE2_START_ADDR+u8Line2Adress, au8UserInputBuffer);
     u8Line2Adress++;
     /*Once input a character,the total of number plus 1*/
     u8InputCount++;
     
    /*Monitor each character as it arrives and compare it to the letters of your name. 
     As each letter arrives that matches the next letter of your name*/
   if(au8UserInputBuffer[DebugScanf(au8UserInputBuffer)]=='l')
    {
       u8NAME[0]=au8UserInputBuffer[DebugScanf(au8UserInputBuffer)];
       /*Once has detected 'l',outputs 'l' on LCD Line1 */
        LCDMessage(LINE1_START_ADDR+9, au8UserInputBuffer);
        u8NAMECount++;
    }
    if(au8UserInputBuffer[DebugScanf(au8UserInputBuffer)]=='e'&&u8NAME[0]=='l')
    {
      u8NAME[1]=au8UserInputBuffer[DebugScanf(au8UserInputBuffer)];
      LCDMessage(LINE1_START_ADDR+10, "e");
      u8NAMECount++;
    }
    if(au8UserInputBuffer[DebugScanf(au8UserInputBuffer)]=='i'&&u8NAME[1]=='e')
    {
       u8NAME[2]=au8UserInputBuffer[DebugScanf(au8UserInputBuffer)];
       LCDMessage(LINE1_START_ADDR+11, "i");
       u8NAMECount++;
    }
    if(au8UserInputBuffer[DebugScanf(au8UserInputBuffer)]=='f'&&u8NAME[2]=='i')
    {
       u8NAME[3]=au8UserInputBuffer[DebugScanf(au8UserInputBuffer)];
       LCDMessage(LINE1_START_ADDR+12, "f");
       u8NAMECount++;
       /*When your nanme has been detected,outputs name and how many times your name has been detected*/
       u8NAMETotalCount++;
       DebugPrintf(u8NumNAMEMessage);
       DebugPrintNumber(u8NAMETotalCount);
       DebugLineFeed();
      
       Buzzer=1;
    }
   
   /*Once has detected all name characters,so make sure NAME ARRAY empty*/
   if(u8NAMECount%4==0)
   {
    for(u8 i=0;i<4;i++)
    {
      u8NAME[i]='0';
    }
     LCDClearChars(LINE1_START_ADDR+9, 4);
   }
    
    
    /*Once the screen is full,clear Line 2 and start again from the left*/
     if(u8Line2Adress==20)
     {
       u8Line2Adress=0;
       LCDClearChars(LINE2_START_ADDR, 20);
     }
     
    }
  
  
  
   if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    /* The button is currently pressed, so make sure Line 2 is cleared */
    LCDClearChars(LINE2_START_ADDR , 20);
    /*start againg from the left*/
    u8Line2Adress=0; 
  }
 
  /* Print message with the total number of characters received 0n the debug port */
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    
    DebugPrintf(u8NumCharsMessage);
    DebugPrintNumber(u8InputCount);
    DebugLineFeed();
  }
  
  
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    /* The button is currently pressed, so reports a message of empty */
    u8InputCount=0;       /*Make sure output 0 when Button1 was preesed*/
    DebugPrintf(u8EmptyMessage);
    DebugLineFeed();
  }
  
  
 if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    /* The button is currently pressed, prints the current letter buffer that is storing your name */
   LCDMessage(LINE1_START_ADDR+14, u8NAME);
   /*Make sure output current letter right when your name is detected twice or more*/
   LCDClearChars(LINE1_START_ADDR+14+u8NAMECount%4 , 6);
   
  }

  
   u16TimeCounter--;
   if(Buzzer==1)
   {
      PWMAudioOn(BUZZER1);
    
     LedOn(RED);
   }
   if(u16TimeCounter==0)
   {
       PWMAudioOff(BUZZER1);
       Buzzer=0;
       LedOff(RED);
       u16TimeCounter=5000;
    }


} /* end UserAppSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserAppSM_Error(void)          
{
  
} /* end UserAppSM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserAppSM_FailedInit(void)          
{
    
} /* end UserAppSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
