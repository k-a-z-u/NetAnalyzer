#ifndef PA_APP_H
#define PA_APP_H

#include "Options.h"

#include <memory>

namespace PowerAnalyzer {

class ClientApp;
class ServerApp;

class PowerAnalyzerApp {
public:
	explicit PowerAnalyzerApp(const Options& op);
	~PowerAnalyzerApp();

	void start();
	void runClient();
	void runServer();

private:
	const Options options;

	std::unique_ptr<ClientApp> client;
	std::unique_ptr<ServerApp> server;
};

} // namespace PowerAnalyzer

#endif // PA_APP_H
