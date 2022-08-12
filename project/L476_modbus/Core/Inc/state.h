#include "hsm.h"

/*
    States of the slave state machine
*/
enum SlaveState
{
    STATE_IDLE,
    STATE_RECEIVE,
    STATE_WAIT,
    STATE_PARSE,
};

/*
*  --------------------- Function prototype ---------------------
*/

state_machine_result_t idle_handler(state_machine_t* const StateMachine);
state_machine_result_t idle_entry_handler(state_machine_t* const StateMachine);
state_machine_result_t idle_exit_handler(state_machine_t* const StateMachine);

state_machine_result_t receive_handler(state_machine_t* const StateMachine);
state_machine_result_t receive_entry_handler(state_machine_t* const StateMachine);
state_machine_result_t receive_exit_handler(state_machine_t* const StateMachine);

state_machine_result_t parse_handler(state_machine_t* const StateMachine);
state_machine_result_t parse_entry_handler(state_machine_t* const StateMachine);
state_machine_result_t parse_exit_handler(state_machine_t* const StateMachine);

state_machine_result_t wait_handler(state_machine_t* const StateMachine);
state_machine_result_t wait_entry_handler(state_machine_t* const StateMachine);
state_machine_result_t wait_exit_handler(state_machine_t* const StateMachine);
