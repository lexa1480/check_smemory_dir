/********************************************************************************
 * File     : ClnSmemory.h
 * Author   : spa
 * Date     : 25.03.2020
 * Comments : client class for smemory plugin
 * ******************************************************************************/

#ifndef __smemory_CLIENT_H
#define __smemory_CLIENT_H

#include "ISmemory.h"

#include <string>
#include <vector>
#include <stdint.h>
#include <BaseSet/LibraryBase.h>

#include <nplug/clnKey.h>
#include <nplug/conKey.h>

/// @breif пространство имен плагина smemory
namespace smemory
{

////////////////////////////////////////////////////////////////////////////////

/// @brief класс клиента плагина smemory
class CClnSmemory : public b_set::CLibraryBase
{
public:
    /// @brief конструктор
    CClnSmemory() : m_pInterface(NULL)
    {
        m_hLibrary = NULL;
        SetLibraryName("smemory");
    }
    /// @brief виртуальный деструктор
    virtual ~CClnSmemory()
    {
        Free();
    }

    /// @brief проверка на успешную загрузку плагина клиентом
    /// @returns TRUE если плагин загружен успешно, FALSE - в противном случае
    bool IsOpen() { return m_pInterface != NULL; }

    /// @brief загрузка плагина
    /// @returns TRUE если плагин загружен успешно, FALSE - в противном случае
    bool Load();
    /// @brief открытие плагина
    /// @returns TRUE если плагин открыт успешно, FALSE - в противном случае
    bool Open();
    /// @brief открытие плагина с особыми настройками
    /// @returns TRUE если плагин открыт успешно, FALSE - в противном случае
    bool Open(nplug::CClnKey& key);

    /// @brief завершение работы с плагином
    void Free();

    /// @brief метод для получения версии библиотеки
    /// @returns версия библиотеки или -1 если библиотека не загружена
    int GetVer() { return m_pInterface ? m_pInterface->GetVer() : -1; }
    /// @brief получение кода последней ошибки
    /// @returns описание последней ошибки
    std::string GetLastError();

    /// @brief метод для обмена данными (получение/отправка)
    /// @param lpRequest - идентификатор запроса
    /// @param key - дерево параметров/данных
    bool ProcessRequest(const char* lpRequest, nplug::CClnKey& key);

    /// @brief метод подключения подписчика на получение событий
    /// @param pSubscriber - подписчик
    bool Subscribe(ICbSphereSubscriber* pSubscriber);
    /// @brief метод отключения подписчика
    /// @param pSubscriber - подписчик
    void UnSubscribe(ICbSphereSubscriber* pSubscriber);

    bool MemoryClear(void* pData, std::size_t sztBytes);
    bool MemoryDelete( void* pData );
    void* MemoryNew(std::size_t sztBytes);

    bool FileClear(const char* pPath, bool bRemoveFile = false);

private:
    ISmemory* m_pInterface;
};

////////////////////////////////////////////////////////////////////////////////
// inlines

// загрузка плагина
inline bool CClnSmemory::Load()
{
    // если уже плагин загружен данным клиентом, больше ничего делать не надо
    if (m_pInterface) { return true; }

    bool bResult = false;
    if (b_set::CLibraryBase::LoadLibrary())
    {
        if (!m_pInterface)
        {
            m_pInterface = (ISmemory*)GetInterface("Smemory_GetInterface");
            bResult = m_pInterface ? (m_pInterface->GetVer() != 0) : false;
        }
    }
    return bResult;
}

// открытие сеанса работы приложения с плагином
inline bool CClnSmemory::Open()
{
    return m_pInterface ? (m_pInterface->Open() == 1) : false;
}

// открытие сеанса работы плагина с заданием дополнительных параметров
inline bool CClnSmemory::Open(nplug::CClnKey& key)
{
    return m_pInterface ? (m_pInterface->Open(key.GetIF()) == 1) : false;
}

// завершение сеанса работы с плагином
inline void CClnSmemory::Free()
{
    if (m_hLibrary)
    {
        if (m_pInterface)
        {
            m_pInterface->Release();
            m_pInterface = NULL;
        }

        FreeLibrary();
    }
}

// получение строки последней ошибки в работе плагина
inline std::string CClnSmemory::GetLastError()
{
    if (!m_pInterface)
        return std::string("smemory not initialized!");

    int iLen = 0;
    bool bResult = (m_pInterface->GetLastError(iLen, NULL) != 0);
    std::string strError;
    if (bResult)
    {
        strError.resize(iLen);
        m_pInterface->GetLastError(iLen, const_cast<NChar*>(strError.data()));
    }
    else
    {
        strError = std::string("Failed to get status of smemory!");
    }

    return strError;
}

////////////////////////////////////////////////////////////////////////////////

// запрос на выполнение операции
inline bool CClnSmemory::ProcessRequest(const char* lpRequest, nplug::CClnKey& key)
{
    bool bRes = false;
    if (m_pInterface)
        bRes = (0 != m_pInterface->ProcessRequest(lpRequest, key.GetIF()));
    return bRes;
}

// подписка на оповещения от плагина
inline bool CClnSmemory::Subscribe(ICbSphereSubscriber* pSubscriber)
{
    bool bRes = false;
    if ( m_pInterface )
        bRes = ( 0 != m_pInterface->Subscribe( pSubscriber ) );
    return bRes;
}

// отмена подписки на оповещения от плагина
inline void CClnSmemory::UnSubscribe(ICbSphereSubscriber* pSubscriber)
{
    if ( m_pInterface )
        m_pInterface->UnSubscribe( pSubscriber );
}

inline bool CClnSmemory::MemoryClear(void* pData, std::size_t sztBytes)
{
    bool bRes = false;
    if ( m_pInterface )
        bRes = ( 0 != m_pInterface->MemoryClear(pData, (NUInt32)sztBytes) );
    return bRes;
}

inline void* CClnSmemory::MemoryNew(std::size_t sztBytes)
{
    void* pData = 0;
    if ( m_pInterface )
        pData = m_pInterface->MemoryNew((NUInt32)sztBytes);
    return pData;
}

inline bool CClnSmemory::MemoryDelete(void* pData)
{
    bool bRes = false;
    if ( m_pInterface )
        bRes = ( 0 != m_pInterface->MemoryDelete(pData) );
    return bRes;
}

inline bool CClnSmemory::FileClear(const char* pPath, bool bRemoveFile )
{
    bool bRes = false;
    if (m_pInterface)
    {
        nplug::CClnKey key = nplug::CConKey::CreateObj();
        std::string sCommand = Command::c_szFileClear;
        key.SetValue(Value::c_szCommand, sCommand);
        key.SetValue(Value::c_szPath, pPath);
        key.SetValue(Value::c_szRemove, bRemoveFile);
        bRes = ( 0 != m_pInterface->ProcessRequest( Request::c_szSet, key.GetIF() ) );
    }
    return bRes;
}

} // smemory

#endif // __smemory_CLIENT_H

