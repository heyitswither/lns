//
// Created by lorenzo on 1/20/18.
//

#ifndef LIB_FILES_COMMON_H
#define LIB_FILES_COMMON_H

#include "defs.h"
#include <cstdio>

class file_ptr_o : public lns::object {
public:
    FILE *ptr;

    explicit file_ptr_o(FILE *ptr) : lns::object(lns::object_type::UNSPECIFIED), ptr(ptr) {};

    std::string str() const override {
        return std::to_string((long) this);
    };

    bool operator==(const lns::object &o) const override {
        return false;
    };

    std::shared_ptr<lns::object> clone() const override {
        return std::make_shared<file_ptr_o>(ptr);
    }
};

#endif //LIB_FILES_COMMON_H
