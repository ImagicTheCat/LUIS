
#ifndef H_SUBMAINS
#define H_SUBMAINS

#include <iostream>
#include "network.hpp"
#include "MainArgs.hpp"
#include "MapData.hpp"
#include "tools.hpp"
#include "Dir.hpp"
#include "Command.hpp"
#include "Contract.hpp"
#include <sodium.h>

int submain_server(MainArgs &args);
int submain_identity(MainArgs &args);
int submain_key(MainArgs &args);

#endif
