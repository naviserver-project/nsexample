/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://mozilla.org/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is AOLserver Code and related documentation
 * distributed by AOL.
 * 
 * The Initial Developer of the Original Code is America Online,
 * Inc. Portions created by AOL are Copyright (C) 1999 America Online,
 * Inc. All Rights Reserved.
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU General Public License (the "GPL"), in which case the
 * provisions of GPL are applicable instead of those above.  If you wish
 * to allow use of your version of this file only under the terms of the
 * GPL and not to allow others to use your version of this file under the
 * License, indicate your decision by deleting the provisions above and
 * replace them with the notice and other provisions required by the GPL.
 * If you do not delete the provisions above, a recipient may use your
 * version of this file under either the License or the GPL.
 */

/*
 * nsexample.c --
 *
 *      An example loadable module which registers the single Tcl command
 *      example_greet.  See README for more explanation.
 */

#include "ns.h"

NS_EXPORT int Ns_ModuleVersion = 1;
NS_EXPORT Ns_ModuleInitProc Ns_ModuleInit;

/*
 * The following structure records configuration information for the module.
 */

typedef struct Config {
    const char   *greeting;
    const char   *server;
    const char   *module;
} Config;

/*
 * Static functions defined in this file.
 */

static Tcl_ObjCmdProc GreetObjCmd;
static Ns_TclTraceProc AddCmds;

/*
 * Static variables defined in this file.
 */

static int numLoaded = 0;  /* Number of times module has been loaded. */



/*
 *----------------------------------------------------------------------
 *
 * Ns_ModuleInit --
 *
 *      Module entry point.  The server runs this function each time
 *      the module is loaded.  The configurable greeting is checked and
 *      a function to create the Tcl command for each interp is
 *      registered.
 *
 * Results:
 *      NS_OK or NS_ERROR.
 *
 * Side effects:
 *      Global numLoaded counter is incremented.
 *
 *----------------------------------------------------------------------
 */

Ns_ReturnCode
Ns_ModuleInit(const char *server, const char *module)
{
    Config     *cfg;
    const char *path;

    if (server == NULL) {
        Ns_Log(Error, "nsexample: module requires a virtual server");
        return NS_ERROR;
    }

    cfg = ns_malloc(sizeof(Config));
    cfg->server = server;
    cfg->module = module;

    path = Ns_ConfigGetPath(server, module, NULL);
    cfg->greeting = Ns_ConfigGetValue(path, "greeting");
    if (cfg->greeting == NULL) {
        cfg->greeting = "Hello";
    }

    Ns_TclRegisterTrace(server, AddCmds, cfg, NS_TCL_TRACE_CREATE);
    numLoaded++;

    return NS_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * AddCmds --
 *
 *      Register module commands for a freshly created Tcl interp.
 *
 * Results:
 *      NS_OK or NS_ERROR.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

static Ns_ReturnCode
AddCmds(Tcl_Interp *interp, const void *arg)
{
    const Config *cfg = arg;

    Tcl_CreateObjCommand(interp, "example_greet", GreetObjCmd, (ClientData)cfg, NULL);

    return NS_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * GreetObjCmd --
 *
 *      A simple Tcl command which prints a friendly greeting to the
 *      person named by the first argument.  The name of the module, the
 *      virtual server it is loaded into, and the total number of times
 *      this module has been loaded are also reported.
 *
 * Results:
 *      TCL_ERROR if no name given, otherwise TCL_OK.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

static int
GreetObjCmd(ClientData arg, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Config *cfg = arg;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "name");
        return TCL_ERROR;
    }
    Ns_TclPrintfResult(interp, "%s %s! (server: %s module: %s loaded: %d)",
                       cfg->greeting, Tcl_GetString(objv[1]),
                       cfg->server, cfg->module, numLoaded);

    return TCL_OK;
}

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * fill-column: 78
 * indent-tabs-mode: nil
 * End:
 */
