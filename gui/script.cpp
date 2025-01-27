/* script - execute JavaScript code to control uStepping

 Embedded engine is duktape.org

 Exposes to JavaScript:
 - statevars[] a,d tests (equal(varname value))
 - commands to enable/disable manual clock
 - comannd to ustep

*/

#include <wx/log.h>
#include <wx/utils.h> // wxMilliSleep
#include "script.h"
#include "pdp11adapter.h"


/* Functions registered for and callable from "duktape" ECMA script interpreter
  Functions are standalone "C", and work on
  - single global context "Script::app" for GUI operations (log(), showPage())
  - on Script::app->pdp11Adapter fpr PDP11 control (statevars)
*/

Pdp11Adapter *Script::pdp11Adapter = nullptr ;
volatile Script::RunState Script::runState ;

// query GUI, process button events (abort!)
// check for abort. In that case, it does interrupt JavaScript execution
// and does not return
void script__service(duk_context* ctx) {
    wxYield(); // process pending GUI messages
    // possible callback to abort(), terminate JavaScript?

    if (Script::runState == Script::RunState::userAbortPending) {
        // send JavaScript break
        Script::pdp11Adapter->onScriptComplete(Script::RunState::userAbort) ;

        // Push an error object or string message onto the stack
        duk_push_string(ctx, "Terminated the program by user abort.");
        // Throw the error
        duk_throw(ctx);
        // dows not return, execution thread stopped
    }



//NO:	Script::abortPending = false ; // processed
    //abort button event calls indirectly script-abort
}

// log(): print argument in event log
static duk_ret_t script__log(duk_context* ctx) {
    script__service(ctx) ; // abort? GUI?

    const char* info = duk_to_string(ctx, 0);
    wxString line("SCRIPT: ");
    line += info;
    wxLogInfo(line); // with timestamp
    /*
    	Script::app->mainFrame->eventsTextCtrl->AppendText("SCRIPT: ");
    	Script::app->mainFrame->eventsTextCtrl->AppendText(info);
    	Script::app->mainFrame->eventsTextCtrl->AppendText("\n");
    */
    return 0;  // Number of return values
}


// issues a single micro step
// waits for repsonse
// M93X2/simulator must send captured UNIBUS
//	*before* MPC reponse!
static duk_ret_t script__ustep(duk_context* ctx) {
    script__service(ctx) ; // abort? GUI?
    Script::pdp11Adapter->requestUStep() ;
    return 0;  // Number of return values

    /*
    tag = send request() ;
    while (! repsonseReceived && !Script::abortPending)
    	script__service() ; // uaser abort?
    	wxGetApp().receiveAndProcessResponses();



    mpc->process: if script running
    	loopback:
    	Script->responseReceivedpsonse(tag)

    */
}

static duk_ret_t script__get(duk_context* ctx) {
    script__service(ctx) ; // abort? GUI?

    const char* arg = duk_to_string(ctx, 0);
    std::string varname(arg) ;
    if (!Script::pdp11Adapter->stateVars.exists(varname)) {
        // Push an error object or string message onto the stack
        duk_push_sprintf(ctx, "get('%s') variable does not exist!", arg);
        // Throw the error
        duk_throw(ctx);
    }

    Variable *var = Script::pdp11Adapter->stateVars.get(varname);
    duk_push_uint(ctx, var->value);  // Return value
    // dows not return, execution thread stopped
    return 1;  // Number of return values
}

// wait some milliseconds, while being abortable
static duk_ret_t script__wait(duk_context* ctx) {
    unsigned arg = duk_to_uint(ctx, 0);
    // wait in incrments of 100ms
    unsigned loopCount = arg / 100;
    for (unsigned i=0 ; i < loopCount; i++) {
        // blocking, but abortable
        script__service(ctx) ; // abort? GUI?
        wxMilliSleep(100);
    }
    return 0;  // Number of return values
}

Script::Script() {
    Script::pdp11Adapter = nullptr ;
    ctx = nullptr ;
}

Script::~Script() {
    // Clean up and destroy the Duktape context
    if (ctx)
        duk_destroy_heap(ctx);
}

void Script::init(Pdp11Adapter *_pdp11Adapter) {
    pdp11Adapter = _pdp11Adapter ;
    // Create a Duktape heap and context
    ctx = duk_create_heap_default();
    if (!ctx) {
        wxLogFatalError("Failed to create a Duktape heap.");
        return;
    }

    // Register the C functions in the Duktape context
    duk_push_c_function(ctx, script__log, 1 /*nargs*/);
    duk_put_global_string(ctx, "log");
    duk_push_c_function(ctx, script__ustep, 0 /*nargs*/);
    duk_put_global_string(ctx, "ustep");
    duk_push_c_function(ctx, script__get, 1 /*nargs*/);
    duk_put_global_string(ctx, "get");
    duk_push_c_function(ctx, script__wait, 1 /*nargs*/);
    duk_put_global_string(ctx, "wait");


    // selftest: print first "hello"
    execute("log('Hello from JavaScript!');");
}

// run JavaScript code
void Script::execute(std::string js_code) {
    runState = RunState::executing;
    // Evaluate the JavaScript code

    if (duk_peval_string(ctx, js_code.c_str()) == 0)
        runState = RunState::complete ;
    else { // unhandled JavaScript, or self generated with duk_throw()
        // in any case, error message is on stack
        const char* arg = duk_safe_to_string(ctx, -1);
        wxString message(arg);
        switch (runState) {
        case RunState::complete: // handled above
        case RunState::javaScriptError: // generated here
            wxLogFatalError("program logic error Script::execute()");
            break ;
        case RunState::userAbort: // no error
            wxLogInfo(message);
            break ;
        case RunState::executing: // error caused in peval()
        case Script::RunState::userAbortPending:
            runState = RunState::javaScriptError;
            wxLogError(message);
            break;
        case RunState::semanticError:
            // display as error
            wxLogError(message);
            break;
        }
    }
    pdp11Adapter->onScriptComplete(runState) ;
}

void Script::abort() {
    runState = RunState::userAbortPending;
}


std::string Script::helpText() {
    return
        "ustep() ;\n"
        "ustep() ;\n"
        "ustep() ;\n"
        "ustep() ;\n"
        "ustep() ;\n"
        ;
#if XXX
    return
        "/*\n"
        "Scripts use standard JavaScript syntax. \n"
        "Quick ref: http://cheat-sheets.org/saved-copy/jsquick.pdf\n"
        "Special functions to access PDP11 and GUI environment:\n"
        "log(string)     - print text into event log.\n"
        "wait(millisecs) - wait while beeing abortable.\n"
        "ustep()         - generate micro step.\n"
        "get(varname)    - get value of a KY11/KM11/simulator signal.\n"
        "Example:\n"
        "*/\n"
        "log(\"now stepping to error point\") ;\n"
        "while ( get(\"UBADDR\") != 0765524 && get(\"UBCYCLE\") != 0 /*DATI*/ )\n"
        "  ustep() ;\n"
        "log(\"defective opcode fetched, step to defective uword\");\n"
        "while (get(\"MPC\") != 0311 )\n"
        "  ustep() ;\n"
        "log(\"reached \"tstb @#dl11.rcsr\" failure point\") ;\n"
        ;
#endif
}

