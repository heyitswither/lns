//
// Created by lorenzo on 2/17/18.
//

#ifndef CPPLNS_ENVIRONMENTS_H
#define CPPLNS_ENVIRONMENTS_H

#include <map>
#include "callable.h"
#include "primitives.h"
#include "token.h"

namespace lns {

    class variable {
    public:
        const bool is_final;
        const visibility visibility_;
        const char *def_file;
        std::shared_ptr<object> value;

        explicit variable();

        variable(lns::visibility vis, bool is_final, std::shared_ptr<object> value, const char *def_file);

        const variable &operator=(variable const &v);
    };

    class runtime_environment {
    protected:
        std::set<callable *> natives;
        std::shared_ptr<runtime_environment> enclosing;
        std::map<std::string, lns::variable> values;

        bool contains_key(std::string name);

    public:
        explicit runtime_environment();

        //virtual ~runtime_environment();

        explicit runtime_environment(std::shared_ptr<runtime_environment> enc);

        virtual std::shared_ptr<object> get(const token *name, const char *accessing_file);

        virtual void define(const token *name, std::shared_ptr<object> o, bool is_final, visibility visibility,
                            const char *def_file);

        virtual void
        define(const std::string &name, std::shared_ptr<object> o, bool is_final, visibility vis, const char *def_file);

        void assign(const token *name, token_type op, std::shared_ptr<object> obj, const char *assigning_file);


        bool clear_var(const token *name);

        bool is_native(callable *ptr);

        void reset();

        void add_natives(const std::set<callable *> &natives);

        void add_native(callable *ptr);
    };

    class context : public lns::runtime_environment, public function_container {
    public:
        explicit context(std::shared_ptr<runtime_environment> previous) : runtime_environment(previous),
                                                                          function_container(CONTEXT_T) {}

        void define(const std::string &name, std::shared_ptr<object> o, bool is_final, visibility vis,
                    const char *def_file) override;

        std::string str() const override;

        bool operator==(const object &o) const override;

        std::shared_ptr<object> clone() const override;

        std::set<callable *> declare_natives() const override;

        void define(const token *name, std::shared_ptr<object> o, bool is_final, visibility visibility,
                    const char *def_file) override;
    };

}
#endif //CPPLNS_ENVIRONMENTS_H
