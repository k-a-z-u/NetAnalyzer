#include "Debug.h"

#include "test/Test.h"

#include "globals.h"

#include "config/CmdLine.h"
#include "config/Config.h"

#include "Settings.h"

#include "network/source/PacketProviderSniffer.h"
#include "export/PeriodicExporterThread.h"

#include "analyzer/Analyzer.h"
#include "analyzer/source/NetSource.h"
#include "analyzer/source/FolderSource.h"

#include <csignal>

#include <csignal>

/* global variables. they must be accessible from siginthandler */
struct globals globals;


/** parse cmd-line args */
bool parseArgs(CmdLine& cmd, struct globals& globals) {

	// debug level?
	dbgLevel = atoi(cmd.get("verbose", false, "0").c_str());

	// now check if testing is requested and start tests
	if (cmd.has("test"))	{RUN_TESTS(); return false;}

	// where to gather data from
	std::string src = cmd.get("src", true);


	if (src.compare("sniffer") == 0) {

		// create sniffer for the requested device
		std::string device = cmd.get("dev", true);
		PacketProviderSniffer* sniffer = &PacketProviderSniffer::get();
		sniffer->setDevice(device);
		sniffer->open();
		sniffer->setFilter("tcp port 80");


		// create a NetSource around this sniffer that will handle TCP streams
		NetSource* src = new NetSource();
		src->setPacketProvider(sniffer);

		// attach the NetSource to the Analyzer
		globals.analyzer->setHTTPDataSource(src);
		globals.packetProvider = sniffer;
		globals.httpSrc = src;

	} else if (src.compare("folder") == 0) {

		// get the folder to load files from
		std::string folderName = cmd.get("folder", true);
		int chunkSize = atoi(cmd.get("chunk-size", true).c_str());

		// create a new folder source
		File folder(folderName);
		globals.httpSrc = new FolderSource(folder, chunkSize);
		globals.analyzer->setHTTPDataSource(globals.httpSrc);

	}



	// initialize config file
	std::string cfg = cmd.get("cfg", false, "config.xml");
	
	try {
		Config::init(cfg);
	} catch (std::exception& e) {
		debug(DBG_MAIN, DBG_LVL_ERR, "ConfigError: " + e.what());
	}

	// which gui to use?
	globals.guiName = cmd.get("gui", true);
	if		(globals.guiName.compare("ncurses") == 0)	{globals.gui = GUINcurses::get();}
	else if	(globals.guiName.compare("none") == 0)		{globals.gui = new GUIDummy();}
	else												{error(DBG_MAIN, "unknown gui-type selected: " << globals.guiName);}

	// attach the gui
	globals.analyzer->setGUI(globals.gui);

	// OK
	return true;

}



void globals_cleanup() {

	// Yes, this is terrible. But how to handle application exit when
	// we must support a clean exit via strg-c?
	debug(DBG_MAIN, DBG_LVL_INFO, "cleanup global variables");

	// delete the gui
	if (globals.gui) {
		delete globals.gui;
		globals.gui = nullptr;
	}

	// stop the PeriodicExporterThread
	if (globals.expThread) {

		// stop the thread
		globals.expThread->stop();

	}



	// shutdown and delete the analyzer object
	if (globals.analyzer) {
		delete globals.analyzer;
		globals.analyzer = nullptr;
	}


	// stop the data source and delete it
	if (globals.httpSrc) {
		globals.httpSrc->stop();
		delete globals.httpSrc;
		globals.httpSrc = nullptr;
	}
	// shutdown and delete the packet provider object
	//FIXME (move this into the NetSource itself??
	if (globals.packetProvider) {
		//FIXME: sniffer is a singleton and MUST NOT be deleted.. but what about other providers?
		//delete globals.packetProvider;
		globals.packetProvider = nullptr;
	}

	// shutdown and delete the http-data-source
	if (globals.httpSrc) {
		delete globals.httpSrc;
		globals.httpSrc = nullptr;
	}



	// the analyzer might contain still-open streams which were
	// closed using the block above. closing connections means:
	// informing listeners (here: exporters!)
	// thus: exporters need to die AFTER the analyzer has died.
	// cleanup-order is important!

	// finally, delete the thread and exporters
	if (globals.expThread) {

		// delete all exporters
		globals.exporters.clear();

		// delete the thread object
		delete globals.expThread;
		globals.expThread = nullptr;

	}


	// remove configuration
	Config::uninit();

	// FIXME ugly.. see main()
	globals.running = false;

}

/** what to do on ctrl-c */
void siginthandler(int sig) {
	(void) sig;
	debug(DBG_MAIN, DBG_LVL_INFO, "CTRL-C!");
	globals_cleanup();
}


/** start here */
int main(int argc, char* argv[]) {

	// register for ctrl-c
	signal(SIGINT, siginthandler);

	std::string guiName = "";
	globals.analyzer = new Analyzer();
	globals.expThread = new PeriodicExporterThread();

	// create command line parser
	std::string usage = ""
			"--src=sniffer --dev=[wlanX|ethX|...] \n"
			"--src=folder --folder=/my/path/ \n"
			"--gui=[ncurses|none] \n"
			"--cfg=FILE {--verbose=LEVEL} {--test}";
	CmdLine cmdLine(argc, argv, usage);

	// parse the cmd-line arguments
	// (some settings are only applicable using the cmd-line)
	if (!parseArgs(cmdLine, globals)) {return 0;}

	// load settings from config file
	Settings::load(globals);

	// start exporting
	globals.expThread->start();

	// perform the magic
	globals.httpSrc->start();

	// FIXME ugly as hell.
	while(globals.running) {sleep(1);}

	// blocks until terminated or error
	debug(DBG_MAIN, DBG_LVL_INFO, "exit main loop");

	// perform final cleanup
	globals_cleanup();

}



