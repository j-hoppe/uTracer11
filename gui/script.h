/* script - execute Jscript code to control uStepping


	A Jscript program could look like:

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

#include "duktape.h"
//#include "application.h"

class Application;

class Script {
private:
	duk_context* ctx ;
public:
	// this is what wie control.
	// Global single instance, as Jscript call back functions are pre "C", notcalss membes
	static Application *app ; // 

	Script();
	~Script() ;

	// config Jscript engine
	void init(Application* app) ;
		

	// interface to GUI
	// catch errors and route to log textControl
		
	void execute() ; // run

	// functions accessible under JScript
	
	// ! every interface functions must call this 
	// - to give wxYield() to the GUI
	// - to check for abort condition from GUI ABORT button press
	// - maybe highlight the code line executed? Compile with Debugger support!
	bool userAbort() ;
	
	// interpret decimal written numebr as ocatl
	// o(100) => 0100 = decimal 64.
	
};

#endif // __SCRIPT_H__
