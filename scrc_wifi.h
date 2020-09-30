#ifndef SCRC_WIFI_H
#define SCRC_WIFI_H

#include <stdlib.h>

//#include "secrets.h"

#define MAIN_SSID "Red"
#define MAIN_PASS "88888888"

//#define MAIN_SSID "IIITH Demo"
//r#define MAIN_PASS "Bsrc12#$"

void nw_setup();
bool nw_start(int timeout, int retry_delay);
void nw_stop();

#endif /* SCRC_WIFI_H */
