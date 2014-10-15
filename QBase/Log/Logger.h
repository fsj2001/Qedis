#ifndef BERT_LOGGER_H
#define BERT_LOGGER_H

#include <string>
#include <set>

#include "../Threads/Thread.h"
#include "../Buffer.h"
#include "../UnboundedBuffer.h"

enum LogLevel
{
    logINFO     = 0x01 << 0,
    logDEBUG    = 0x01 << 1,
    logWARN     = 0x01 << 2,
    logERROR    = 0x01 << 3,
    logUSR      = 0x01 << 4,
    logALL      = 0xFFFFFFFF,
};

enum LogDest
{
    logConsole  = 0x01 << 0,
    logFILE     = 0x01 << 1,
    logSocket   = 0x01 << 2,  // TODO : LOG SERVER
};

class Logger
{
public:

	friend class LogManager;

    bool Init(unsigned int level = logDEBUG,
              unsigned int dest = logConsole,
              const char* pDir  = 0);
    
    void Flush(LogLevel  level);
    bool IsLevelForbid(unsigned int level)  {  return  !(level & m_level); };

    Logger&  operator<<(const char* msg);
    Logger&  operator<<(const unsigned char* msg);
    Logger&  operator<<(void* );
    Logger&  operator<<(unsigned char a);
    Logger&  operator<<(char a);
    Logger&  operator<<(unsigned short a);
    Logger&  operator<<(short a);
    Logger&  operator<<(unsigned int a);
    Logger&  operator<<(int a);
    Logger&  operator<<(unsigned long a);
    Logger&  operator<<(long a);
    Logger&  operator<<(unsigned long long a);
    Logger&  operator<<(long long a);

    Logger& SetCurLevel(unsigned int level)
    {
        m_curLevel = level;
        return *this;
    }

    bool   Update();

private:
    //NONCOPYABLE(Logger);

    Logger();
   ~Logger();

    static const size_t MAXLINE_LOG = 2048; // TODO
    char            m_tmpBuffer[MAXLINE_LOG];
    std::size_t     m_pos;
    Buffer          m_buffer;

    Mutex           m_backBufLock;
    std::size_t     m_backBytes;
    UnboundedBuffer m_backBuf;

    unsigned int    m_level;
    std::string     m_directory;
    unsigned int    m_dest;
    std::string     m_fileName;

    unsigned int    m_curLevel;

    char*           m_pMemory;
    std::size_t     m_offset;
    int             m_file;

    bool    _CheckChangeFile();
    const std::string& _MakeFileName();
    bool    _OpenLogFile(const char* name);
    bool    _CloseLogFile();
    void    _WriteLog(int level, std::size_t nLen, const char* data);
    void    _Color(unsigned int color);
    void    _Reset();

    static bool _MakeDir(const char* pDir);
};



class LogHelper
{
public:
    LogHelper(LogLevel level);
    Logger& operator=(Logger& log);

private:
    LogLevel    m_level;
};



class LogManager
{
public:
    static LogManager& Instance();

    ~LogManager();

    Logger*  CreateLog(unsigned int level = logDEBUG,
                       unsigned int dest = logConsole,
                       const char* pDir  = 0);

    bool    StartLog();
    void    StopLog();
    bool    Update();

    Logger* NullLog()  {  return  &m_nullLog;  }

private:
  //  NONCOPYABLE(LogManager);

    LogManager();

    Logger  m_nullLog;

    typedef std::set<Logger* >  LOG_SET;
    LOG_SET m_logs;
    

    class LogThread : public Runnable
    {
    public:
        LogThread() : m_alive(false) {}
        
        bool  IsAlive() const { return m_alive; }
        void  Stop() {  m_alive = false; }
        
        virtual void Run();
    private:
        volatile bool m_alive;
    };
    
    SharedPtr<LogThread>  m_logThread;
};



#undef INF
#undef DBG
#undef WRN
#undef ERR
#undef USR

#define  LOG_INF(x)      (LogHelper(logINFO))=(g_log ? g_log : LogManager::Instance().NullLog())->SetCurLevel(logINFO)
#define  LOG_DBG      LOG_INF
#define  LOG_WRN      LOG_INF
#define  LOG_ERR      LOG_INF
#define  LOG_USR      LOG_INF

#define  INF      (LogHelper(logINFO))=(g_log ? g_log : LogManager::Instance().NullLog())->SetCurLevel(logINFO)
#define  DBG      (LogHelper(logDEBUG))=(g_log ? g_log : LogManager::Instance().NullLog())->SetCurLevel(logDEBUG)
#define  WRN      (LogHelper(logWARN))=(g_log ? g_log : LogManager::Instance().NullLog())->SetCurLevel(logWARN)
#define  ERR      (LogHelper(logERROR))=(g_log ? g_log : LogManager::Instance().NullLog())->SetCurLevel(logERROR)
#define  USR      (LogHelper(logUSR))=(g_log ? g_log : LogManager::Instance().NullLog())->SetCurLevel(logUSR)

#endif

