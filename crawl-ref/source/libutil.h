/*
 *  File:       libutil.h
 *  Summary:    System independent functions 
 *
 *  Modified for Crawl Reference by $Author$ on $Date$
 *
 *  Change History (most recent first):
 *
 *      <1>   2001/Nov/01        BWR     Created
 *
 */

#ifndef LIBUTIL_H
#define LIBUTIL_H

#include "AppHdr.h"
#include "defines.h"
#include <cctype>
#include <string>
#include <vector>

std::string lowercase_string(std::string s);
std::string &lowercase(std::string &s);
std::string &uppercase(std::string &s);
bool ends_with(const std::string &s, const std::string &suffix);
std::string pluralise(const std::string &name, 
                      const char *no_of[] = NULL);

bool shell_safe(const char *file);

/**
 * Returns 1 + the index of the first suffix that matches the given string,
 * 0 if no suffixes match.
 */
int  ends_with(const std::string &s, const char *suffixes[]);

std::string strip_filename_unsafe_chars(const std::string &s);

std::string make_stringf(const char *format, ...);

std::string replace_all(std::string s,
                        const std::string &tofind,
                        const std::string &replacement);

std::string replace_all_of(std::string s,
                           const std::string &tofind,
                           const std::string &replacement);

int count_occurrences(const std::string &text, const std::string &searchfor);

void play_sound(const char *file);

// Pattern matching
void *compile_pattern(const char *pattern, bool ignore_case = false);
void free_compiled_pattern(void *cp);
bool pattern_match(void *compiled_pattern, const char *text, int length);

std::string & trim_string( std::string &str );
std::string trimmed_string( std::string s );

// Splits string 's' on the separator 'sep'. If trim == true, trims each
// segment. If accept_empties == true, accepts empty segments. If nsplits >= 0,
// splits on the first nsplits occurrences of the separator, and stores the
// remainder of the string as the last segment; negative values of nsplits
// split on all occurrences of the separator.
std::vector<std::string> split_string(
        const char *sep, 
        std::string s, 
        bool trim = true, 
        bool accept_empties = false,
        int nsplits = -1);

inline std::string lowercase_first(std::string s)
{
    if (s.length())
        s[0] = tolower(s[0]);
    return (s);
}

template <typename Z>
std::string comma_separated_line(Z start, Z end,
                                 const std::string &andc = " and ",
                                 const std::string &comma = ", ")
{
    std::string text;
    for (Z i = start; i != end; ++i)
    {
        if (i != start)
        {
            Z tmp = i;
            if (++tmp != end)
                text += comma;
            else
                text += andc;
        }

        text += *i;
    }
    return (text);
}

#ifdef NEED_USLEEP
void usleep( unsigned long time );
#endif

#ifdef NEED_SNPRINTF
int snprintf( char *str, size_t size, const char *format, ... );
#endif

// Sets a boolean to a new value in the scope of the object instance.
class unwind_bool
{
public:
    unwind_bool(bool &val_, bool newval) : val(val_), oldval(val_)
    {
        val = newval;
    }
    ~unwind_bool()
    {
        val = oldval;
    }
private:
    bool &val;
    bool oldval;
};

class base_pattern
{
public:
    virtual ~base_pattern() { };

    virtual bool valid() const = 0;
    virtual bool matches(const std::string &s) const = 0;
};

class text_pattern : public base_pattern
{
public:
    text_pattern(const std::string &s, bool icase = false) 
        : pattern(s), compiled_pattern(NULL),
          isvalid(true), ignore_case(icase)
    {
    }

    text_pattern()
        : pattern(), compiled_pattern(NULL),
         isvalid(false), ignore_case(false)
    {
    }

    text_pattern(const text_pattern &tp)
        : base_pattern(tp),
          pattern(tp.pattern),
          compiled_pattern(NULL),
          isvalid(tp.isvalid),
          ignore_case(tp.ignore_case)
    {
    }

    ~text_pattern()
    {
        if (compiled_pattern)
            free_compiled_pattern(compiled_pattern);
    }

    const text_pattern &operator= (const text_pattern &tp)
    {
        if (this == &tp)
            return tp;

        if (compiled_pattern)
            free_compiled_pattern(compiled_pattern);
        pattern = tp.pattern;
        compiled_pattern = NULL;
        isvalid      = tp.isvalid;
        ignore_case  = tp.ignore_case;
        return *this;
    }

    const text_pattern &operator= (const std::string &spattern)
    {
        if (pattern == spattern)
            return *this;

        if (compiled_pattern)
            free_compiled_pattern(compiled_pattern);
        pattern = spattern;
        compiled_pattern = NULL;
        isvalid = true;
        // We don't change ignore_case
        return *this;
    }

    bool compile() const
    {
        return !empty()?
            !!(compiled_pattern = compile_pattern(pattern.c_str(), ignore_case))
          : false;
    }

    bool empty() const
    {
        return !pattern.length();
    }

    bool valid() const
    {
        return isvalid &&
            (compiled_pattern || (isvalid = compile()));
    }

    bool matches(const char *s, int length) const
    {
        return valid() && pattern_match(compiled_pattern, s, length);
    }

    bool matches(const char *s) const
    {
        return matches(std::string(s));
    }

    bool matches(const std::string &s) const
    {
        return matches(s.c_str(), s.length());
    }

    const std::string &tostring() const
    {
        return pattern;
    }
    
private:
    std::string pattern;
    mutable void *compiled_pattern;
    mutable bool isvalid;
    bool ignore_case;
};


#endif
