#include "PowerAnalyzerApp.h"
#include "client/ClientApp.h"
#include "server/ServerApp.h"
#include "../../NetAnalyzer/src/Helper.h"
#include "../../NetAnalyzer/src/Debug.h"

namespace PowerAnalyzer {

static const char* DBG_APP = "PWRNLYZR";

PowerAnalyzerApp::PowerAnalyzerApp(const Options& op) : options(op) {

}

PowerAnalyzerApp::~PowerAnalyzerApp() {
	if (client) {
		debug(DBG_APP, DBG_LVL_INFO, "client is about to stop");
		client->stop();
	}

	if (server) {
		debug(DBG_APP, DBG_LVL_INFO, "server is about to stop");
		server->stop();
	}
}

void PowerAnalyzerApp::start() {
	if (options.getMode() == OperationMode::Client) {
		runClient();
	} else if (options.getMode() == OperationMode::Server) {
		runServer();
	} else {
		throw "Unknown operation mode.";
	}
}

void PowerAnalyzerApp::runClient() {
	client = make_unique<ClientApp>(options);

	debug(DBG_APP, DBG_LVL_INFO, "program is about to run in CLIENT mode");
	client->run();
}

void PowerAnalyzerApp::runServer() {
	server = make_unique<ServerApp>(options);

	debug(DBG_APP, DBG_LVL_INFO, "program is about to run in SERVER mode");
	server->run();
}

} // namespace PowerAnalyzer
