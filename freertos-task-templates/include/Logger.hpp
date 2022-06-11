#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <string>
#include <stdexcept>
/* Got help From : https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
    template<typename ... Args>
    std::string string_format( const std::string& format, Args ... args )
    {
        int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
        if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
        auto size = static_cast<size_t>( size_s );
        std::unique_ptr<char[]> buf( new char[ size ] );
        std::snprintf( buf.get(), size, format.c_str(), args ... );
        return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
    }
*/
/**
 * @brief Helper class designed to encapsulate the logging functions of the given platform.
 * The class itself is a singleton, that creates the output file upon creation and closes it before being destroyed.
 *
 * The implementation of this singleton is taken from this link
 * [https://stackoverflow.com/questions/1008019/c-singleton-design-pattern]
 */

class Logger
{
  protected:
    /**
     * @brief Logger constructor. This constructor is kept private in order to prevent misusage
     */
    Logger() {};

    /**
     * @brief Logger destructor. This is kept private in order to prevent misusage
     */
    ~Logger() {};

    /**
     * @brief Get a string as timestamp.
     * 
     */
    std::string GetTimestamp();

    /**
     * @brief Print log message in log file.
     */
    template<typename... TArgs>
    static inline constexpr void PrintLog(const char* fmt, TArgs... args);

    /**
     * @brief Function that returns a pointer to the output stream associated to the log file
     * When this function is called for the first time, it constructs the static instance of Logger
     *
     * @return Pointer to the Logger instance.
     */
    static Logger * GetInstance() {
        static Logger static_instance;         // C++11 makes this thread-safe
        return &static_instance;
    };

    /**
     * @brief Formatter function.
     */
    template<typename ... Args>
    static inline constexpr std::string GetFormattedString( const std::string& format, Args ... args );

  public:
    /**
     * @brief Main log function. Only this funcion must be called. 
     * 
     * @tparam TArgs 
     * @param fmt 
     * @param args 
     */
    template<typename... TArgs>
    static inline constexpr void Log(const char* fmt, TArgs... args);


    /**
     * @brief Deleted copy constructor. This is done to prevent users to create copies of the singleton
     */
    Logger( Logger const& ) = delete;

    /**
     * @brief Deleted operator. This is done to prevent users to create copies of the singleton
     */
    void operator=( Logger const& ) = delete;
    
};

template<typename ... Args>
constexpr std::string Logger::GetFormattedString( const std::string& format, Args ... args )
{ 
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args... ) + 1;        // This step is used to calculate the required size_s. Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    char buf[size];                                                               // We created a char array to populate the format %s, %d, %x, etc.
    std::snprintf( buf, size, format.c_str(), args... );
    return std::string( buf, buf + size - 1 );                                    // We don't want the '\0' inside
}

template<typename... TArgs>
constexpr void Logger::Log(const char* fmt, TArgs... args)
{
    PrintLog(fmt, std::forward<TArgs>(args)...);
}

template<typename... TArgs>
constexpr void Logger::PrintLog(const char* fmt, TArgs... args)
{
    printf("%s\r\n", GetFormattedString(fmt, std::forward<TArgs>(args)... ).c_str()  );
}

#endif // LOGGER