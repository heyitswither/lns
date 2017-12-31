//
// Created by lorenzo on 12/30/17.
//

#include "update.h"
#include "revision.h"

signed char update() {
    if (!system(nullptr)) throw lns::cmd_processor_unavailable();
    if (system("bash /lns/update_lns.sh") == 0) return 0;
    std::cout << "\nInstallation failed." << std::endl;
    return FATAL_ERROR;
}

void check_update() {
    if (!system(nullptr)) return;
    std::string s("bash /lns/check_revision.sh ");
    s += std::to_string(REVISION_NUMBER);
    system(s.c_str());
}
