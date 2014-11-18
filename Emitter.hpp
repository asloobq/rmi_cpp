#ifndef EMITTER_HPP
#define EMITTER_HPP



#include <stdarg.h>
#include <iosfwd>
#include <string>



class Emitter {

    public:

        Emitter(std::ostream *, const std::string &replacement = var_name);

        void emitLineStartF(const char *fmt, ...) const;
        void emitLineStartF(int indent_delta, const char *fmt, ...);
        void emitString(const std::string &) const;
        void emitStringF(const char *fmt, ...) const;
        void emitLineEnd(const std::string &) const;
        void emitLineEndF(const char *fmt, ...) const;
        void emitLine(const std::string &) const;
        void emitLine(int indent_delta, const std::string &);
        void emitLineF(const char *fmt, ...) const;
        void emitLineF(int indent_delta, const char *fmt, ...);

        void increment_indent_level();
        void decrement_indent_level();

    private:

        void emitHelper(const char *fmt, va_list ap, bool is_start, bool is_end) const;

        std::ostream *os;
        const std::string replacement;

        int indent_level;

	static const std::string var_name;
};



#endif



/*
 * These lines should go at the end of every source code file
 * to set the indent, tabs, and other options.  Feel free to
 * add other appropriate options here.
 *
 */

/* Local Variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
/* vim: set filetype=cpp tabstop=8 shiftwidth=4 softtabstop=4 tw=79 expandtab: */
