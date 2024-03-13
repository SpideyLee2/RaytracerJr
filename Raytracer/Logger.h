#pragma once

// Wrapper class for spdlog

#include "spdlog/spdlog.h"
#include <cstdarg>

namespace mtn {

	static std::shared_ptr<spdlog::logger> allLogger = nullptr;

	enum class LogLevel {
		TRACE,
		DEBUG,
		WARN,
		ERR,
		FATAL
	};

	class Logger {
	public:
		Logger() =						delete;
		Logger(const Logger&) =			delete;
		Logger(Logger&&) =				delete;
		~Logger() =						default;

		void operator=(const Logger&) = delete;
		void operator=(Logger&&) =		delete;

		template<typename ...Args>
		static inline void info(const char* message, Args &&... args) {
			spdlog::info(message, args...);
		}

		template<typename ...Args>
		static inline void trace(const char* message, Args &&... args) {
			spdlog::trace(message, args...);
		}

		template<typename ...Args>
		static inline void debug(const char* message, Args &&... args) {
			spdlog::debug(message, args...);
		}

		template<typename ...Args>
		static inline void warn(const char* message, Args &&... args) {
			spdlog::warn(message, args...);
		}

		template<typename ...Args>
		static inline void error(const char* message, Args &&... args) {
			spdlog::error(message, args...);
		}

		template<typename ...Args>
		static inline void fatal(const char* message, Args &&... args) {
			spdlog::critical(message, args...);
			// probably also want to dump the log trace somewhere
		}
		
		static void setLogFileNames(const std::string& allLog = "all_logs.txt", 
									const std::string& backtraceLog = "backtrace_logs.txt");
		static void setLogLevel(const LogLevel& level);
		static void setPattern(const char* pattern);
		static void enableBacktrace(const size_t& numMessages);
		static void logBacktrace();
		static void init();

	private:
		static const LogLevel INIT_LOG_LEVEL = LogLevel::DEBUG;
		//static std::shared_ptr<spdlog::logger> backtraceLogger = nullptr;
	};

}