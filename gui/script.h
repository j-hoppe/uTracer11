/* script - execute JavaScript code to control uStepping


	A JavaScript program could look like:

	function fetchFrom(addr) {
		// DATI fetch occurs in ustep 15
		if ( equals("BAADDR", addr)
			&& equals(BACYCLE, 0) // DATI
			&& equals("MPC", o(015) )
			return true ;
		else
			return false ;
	}

	man_clk_enable() ; // stop u machine
	log("now stepping to error point") ;
	while (!fetchFrom( o(165020) )
		ustep() ;
	log("reached entry of monitor") ;
	log("stepping to defective ustep");
	while (!equals("MPC", o(365) )
		ustep() ;
	log("reached ASL R0 failure point") ;



*/
#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include <string>
#include "duktape.h"

//#include "pdp11adapter.h"
class Pdp11Adapter; // cross-include

class Script {
private:
    duk_context* ctx ;
public:

    enum class RunState {
        executing,
        userAbortPending, // button press signal
        userAbort,
        complete, // run to end without errors
        javaScriptError, // invalid JavaScript, syntax
        semanticError // error in access to pdp11adapter
    } ;


    // this is what we control.
    // global var, as must be accessed by context free C functions
    // Global single instance, as JavaScript call back functions are pre "C", not class members
    static Pdp11Adapter* pdp11Adapter;
    static volatile RunState runState ;


    Script();
    ~Script() ;

    // config JavaScript engine
    void init(Pdp11Adapter *pdp11Adapter) ;

    /* interface to GUI */

    // run interpreter on code
    // catch errors and route to log textControl
    void execute(std::string js_code) ; // run

    void abort() ; // from GUI

    std::string helpText() ;

};

#endif // __SCRIPT_H__
