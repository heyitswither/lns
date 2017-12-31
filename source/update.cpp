//
// Created by lorenzo on 12/30/17.
//

#include "update.h"

signed char update() {
    if (!system(nullptr)) throw lns::cmd_processor_unavailable();
    if (system("bash /lns/update_lns.sh") == 0) return 0;
    std::cout << "\nInstallation failed." << std::endl;
    return FATAL_ERROR;
}