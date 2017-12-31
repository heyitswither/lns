//
// Created by lorenzo on 12/30/17.
//

#include "update.h"
#include "revision.h"

signed char update() {
    if (!system(nullptr)) throw lns::cmd_processor_unavailable();
    int update_required = check_update();
    std::cout << update_required;
    switch (update_required) {
        case 256:
            std::cout << "The interpreter is up to date. Do you want to update anyway? ";
            break;
        case 512:
            std::cout << "Do you want to install it? ";
            break;
        case 768:
            std::cout << "An error occurred while checking updates. Do you want to attempt to install anyway? ";
            break;
    }
    std::string s;
    do {
        std::cout << "(y/n): ";
        std::cin >> s;
    } while (s != "y" && s != "n" && s != "Y" && s != "N");

    if (s == "y" || s == "Y") {
        if (system("bash -c \"" LNS_INSTALL_LOCATION "update_lns.sh\"") == 0) return 0;
        std::cout << "\nInstallation failed." << std::endl;
        return FATAL_ERROR;
    }
    return 0;
}

int check_update() {
    std::string s("bash " LNS_INSTALL_LOCATION "check_revision.sh ");
    s += std::to_string(REVISION_NUMBER);
    return system(s.c_str());
}
