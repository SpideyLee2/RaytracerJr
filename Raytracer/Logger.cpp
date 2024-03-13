#include "Logger.h"

//#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <iostream>

namespace mtn {

	void Logger::setLogFileNames(const std::string& allLog, const std::string& backtraceLog) {
		try {
			//allLogger = spdlog::basic_logger_mt("all_logger", "logs/" + allLog);
			allLogger = spdlog::stdout_color_mt("all_logger");
		}
		catch (spdlog::spdlog_ex& ex) {
			std::cout << "all_logger init failed: " << ex.what() << std::endl;
		}
	//
	//	try {
	//		backtraceLogger = spdlog::basic_logger_mt("backtrace_logger", "logs/" + backtraceLog);
	//	}
	//	catch (spdlog::spdlog_ex& ex) {
	//		std::cout << "backtrace_logger init failed: " << ex.what() << std::endl;
	//	}
	}

	void Logger::setLogLevel(const LogLevel& level) {
		switch (level) {
			case LogLevel::TRACE:
				spdlog::set_level(spdlog::level::trace);
				break;
			case LogLevel::DEBUG:
				spdlog::set_level(spdlog::level::debug);
				break;
			case LogLevel::WARN:
				spdlog::set_level(spdlog::level::warn);
				break;
			case LogLevel::ERR:
				spdlog::set_level(spdlog::level::err);
				break;
			case LogLevel::FATAL:
				spdlog::set_level(spdlog::level::critical);
				break;
			default:
				break;
		}
	}

	void Logger::setPattern(const char* pattern) {
		spdlog::set_pattern(pattern);
	}

	void Logger::enableBacktrace(const size_t& numMessages) {
		spdlog::enable_backtrace(numMessages);
	}

	void Logger::logBacktrace() {
		spdlog::dump_backtrace();
	}

	void Logger::init() {
		// Initialize logger settings
		#ifdef _DEBUG
		setLogLevel(INIT_LOG_LEVEL);
		spdlog::flush_on(spdlog::level::debug);
		#endif
		#ifdef NDEBUG
		setLogLevel(LogLevel::DEBUG);
		spdlog::flush_on(spdlog::level::err);
		#endif

		setLogFileNames();

		if (allLogger) {
			spdlog::set_default_logger(allLogger);
		}
		else {
			spdlog::error("allLogger not properly initialized. Using spdlog's default logger instead.");
		}

		setPattern("[%H:%M:%S] [thread %t] [%^%l%$] %v");
		//setPattern("[%H:%M:%S] [%n] [thread %t] [%^%l%$] %v");
		//spdlog::flush_on(spdlog::level::trace);
		//enableBacktrace(32);

		debug("Logger initialized.");
	}

}