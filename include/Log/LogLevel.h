#pragma once

/**
 * @author gda
 */

namespace logger
{

enum enu_log_level
{
    debug_lvl         = 0,
    normal_lvl        = 1,
    critical_lvl      = 2
};

struct level_wrapper
{
    enu_log_level       m_eLvl;

    explicit level_wrapper(enu_log_level eLvl)
        : m_eLvl(eLvl)
    {
    }
};

inline level_wrapper level(enu_log_level eLvl)
{
	return level_wrapper(eLvl);
}

inline level_wrapper debug_level()
{
    return level(debug_lvl);
}
inline level_wrapper normal_level()
{
    return level(normal_lvl);
}

inline level_wrapper critical_level()
{
    return level(critical_lvl);
}

}               // logger
