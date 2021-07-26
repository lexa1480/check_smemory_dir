/********************************************************************************
 * File     : ISmemory.h
 * Author   : spa
 * Date     : 25.03.2020
 * Comments : interface for smemory plugin
 * ******************************************************************************/

#ifndef __I_Smemory_H
#define __I_Smemory_H

#include <NitaDataTypes.h>

namespace nplug { struct IKey; } // struct name declaration only

/// @breif пространство имен плагина smemory
namespace smemory
{

namespace Request
{
    const char c_szGet[]                = "get";
    const char c_szSet[]                = "set";
} // Request

namespace Value
{
    const char c_szCommand[]            = "Command";
    const char c_szPath[]               = "Path";
    const char c_szRemove[]             = "Remove";
    const char c_szRes[]                = "Result";
    const char c_szErrors[]             = "Errors";
} // Value

namespace Command
{
    const char c_szFileClear[]          = "file_clear";
} // Command

/// @brief события sphere/smemory
namespace Event
{
    const char c_szUserChanged[]        = "user_changed";
} // Event

/// @brief интерфейс подписчика на события
struct ICbSphereSubscriber
{
     virtual NBool OnSphereEvent(LPCNStr lpEvent, nplug::IKey* lpKey) = 0;
};

/// @brief интерфейс
struct ISmemory
{
    /// @brief открытие объекта
    /// @returns TRUE если объект проинициализирован успешно, FALSE - в противном случае
    virtual NBool Open(nplug::IKey* lpKey = NULL) = 0;

    /// @brief высвобождение smemory
    virtual NVoid Release() = 0;

    /// @brief метод для получения версии библиотеки
    /// @returns версия библиотеки, реализующей данный интерфейс
    virtual NInt GetVer() = 0;

    /// @brief метод для получения кода последней ошибки работы библиотеки
    /// @param iLength длина строки ошибки
    /// @param strError строка ошибки
    virtual NBool GetLastError(NInt &iLength, NChar* strError = NULL) = 0;

    /// @brief метод для обмена данными (получение/отправка)
	/// @param lpRequest - идентификатор запроса
	/// @param lpKey - дерево параметров/данных
    virtual NBool ProcessRequest(LPCNStr lpRequest, nplug::IKey* lpKey) = 0;

    /// @brief метод подключения подписчика на получение событий
	/// @param pSubscriber - подписчик
    virtual NBool Subscribe(ICbSphereSubscriber* pSubscriber) = 0;

	/// @brief метод отключения подписчика
	/// @param pSubscriber - подписчик
    virtual NVoid UnSubscribe(ICbSphereSubscriber* pSubscriber) = 0;

    virtual NBool MemoryClear(void* pData, NUInt32 uBytes) = 0;
    virtual void* MemoryNew(NUInt32 uBytes) = 0;
    virtual NBool MemoryDelete( void* pData ) = 0;
};

} // smemory

#endif // __I_Smemory_H
