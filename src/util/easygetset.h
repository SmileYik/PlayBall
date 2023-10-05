#ifndef EASYGETSET_H
#define EASYGETSET_H

// #define FIRST_CHAR_UPPER(TEXT) \
//     { \
//         char* chars = TEXT; \
//         if (chars[0] >= 'a' && chars[0] <= 'b') chars[0] += 'A' - 'a';
        
//     }

#define DECLARE_GETCONST_FUNCTION(TYPE, NAME) DECLARE_GET_FUNCTION(TYPE, NAME, , const)

#define DECLARE_GET_FUNCTION(TYPE, NAME, PREFIX, SUFFIX) \
    PREFIX TYPE get##NAME() SUFFIX

#define DECLARE_SET_FUNCTION(TYPE, NAME) \
    void set##NAME(TYPE NAME)

#define DECLARE_GET_SET_FUNCTION(TYPE, NAME, PREFIX, SUFFIX) \
    DECLARE_GET_FUNCTION(TYPE, NAME, PREFIX, SUFFIX); \
    DECLARE_SET_FUNCTION(TYPE, NAME)

#define DEFINE_GET_FUNCTION(TYPE, NAME, CLASS, VAR, PREFIX, SUFFIX, POST) \
    PREFIX TYPE CLASS::get##NAME() SUFFIX { POST return VAR;  }

#define DEFINE_SET_FUNCTION(TYPE, NAME, CLASS, VAR, POST, AFTER) \
    void CLASS::set##NAME(TYPE NAME) { POST this->VAR = NAME;  AFTER }

#define DEFINE_GET_SET_FUNCTION(TYPE, NAME, CLASS, VAR, PREFIX, SUFFIX, GET_POST, SET_POST, AFTER) \
    DEFINE_GET_FUNCTION(TYPE, NAME, CLASS, VAR, PREFIX, SUFFIX, GET_POST); \
    DEFINE_SET_FUNCTION(TYPE, NAME, CLASS, VAR, SET_POST, AFTER)

#define FOREACH_ITERATOR(NAME, VAR, CODE) \
    for (auto begin = NAME.begin(), \
              end = NAME.end(); begin != end; ++begin) \
    { \
        auto VAR = *begin; \
        CODE \
    }

#endif // EASYGETSET_H