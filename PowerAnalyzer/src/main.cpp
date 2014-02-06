#include "PowerAnalyzerApp.h"
#include "Options.h"
#include "../../NetAnalyzer/src/Debug.h"

#include <csignal>
#include <cstdlib>

namespace {

static const char* DBG_APP = "PWRNLYZR";

PowerAnalyzer::PowerAnalyzerApp* g_app = nullptr;

void g_cleanup() {
	if (g_app) {
		delete g_app;
		g_app = nullptr;
	}
}

void sigintHandler(const int sig) {
	(void) sig;

	g_cleanup();
	exit(EXIT_SUCCESS);
}

} // unnamed namespace


int main(int argc, char* argv[]) {
	// register handler for ctrl-c
	signal(SIGINT, sigintHandler);

	try {
		PowerAnalyzer::Options op;
		if (op.parseCmdline(argc, argv)) {
			g_app = new PowerAnalyzer::PowerAnalyzerApp(op);
			g_app->start();
		} else {
			PowerAnalyzer::Options::printUsage(argv[0]);
		}
	}
	catch (const char* e) {
		debug(DBG_APP, DBG_LVL_ERR, "exception caught: " << e);
	}
	catch (...) {
		debug(DBG_APP, DBG_LVL_ERR, "Unknown exception thrown.");
	}

	g_cleanup();
}
