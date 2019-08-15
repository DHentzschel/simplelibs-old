﻿#ifndef LOGGER_H
#define LOGGER_H

#include "astring.h"
#include "consolecolor.h"
#include "plogtp.h"
#include "file.h"
#include "simplecore.h"

/**
 * Provides a simple logger with four predefined log types.
 *
 * @author Daniel Hentzschel on 11.05.2019.
 */
class Logger {
public:
	/**
	 * Closes the filestream if opened.
	 */
	SIMPLECORE_API ~Logger();

	/**
	 * Initializes the log filestream by opening it with the path given by
	 * parameter.
	 *
	 * @param path the file path of the log file
	 */
	SIMPLECORE_API static void prepareLogFile(const AString& path);

	/**
	 * Prints text to the console.
	 *
	 * @param text the text to print
	 * @param newLine whether there should be a new line appended
	 * @param color the foreground color of the text (default is Cyan)
	 * @param backgroundColor the background color of the text (default is Black)
	 */
	SIMPLECORE_API static void print(const AString& text,
		bool newLine = true,
		ConsoleColor::Type color = ConsoleColor::Type::Cyan,
		ConsoleColor::Type backgroundColor = ConsoleColor::Type::Black);

	/**
	 * Prints debug text to the console.
	 * The current time and output mode will be prepended.
	 *
	 * @param text the text to print
	 * @param newLine whether there should be a new line appended
	 * @param color the foreground color of the text (default is Cyan)
	 * @param backgroundColor the background color of the text (default is Black)
	 */
	SIMPLECORE_API static void debug(const AString& text,
		bool newLine = true,
		ConsoleColor::Type color = ConsoleColor::Type::Cyan,
		ConsoleColor::Type backgroundColor = ConsoleColor::Type::Black);

	/**
	 * Prints information text to the console.
	 * The current time and output mode will be prepended.
	 *
	 * @param text the text to print
	 * @param newLine whether there should be a new line appended
	 * @param color the foreground color of the text (default is LightGray)
	 * @param backgroundColor the background color of the text (default is Black)
	 */
	SIMPLECORE_API static void info(const AString& text,
		bool newLine = true,
		ConsoleColor::Type color = ConsoleColor::Type::LightGray,
		ConsoleColor::Type backgroundColor = ConsoleColor::Type::Black);

	/**
	 * Prints error text to the console.
	 * The current time and output mode will be prepended.
	 *
	 * @param text the text to print
	 * @param newLine whether there should be a new line appended
	 * @param color the foreground color of the text (default is LightGray)
	 * @param backgroundColor the background color of the text (default is Black)
	 */
	SIMPLECORE_API static void error(const AString& text,
		bool newLine = true,
		ConsoleColor::Type color = ConsoleColor::Type::LightGray,
		ConsoleColor::Type backgroundColor = ConsoleColor::Type::Black);

	/**
	 * Prints warning text to the console.
	 * The current time and output mode will be prepended.
	 *
	 * @param text the text to print
	 * @param newLine whether there should be a new line appended
	 * @param color the foreground color of the text (default is LightGray)
	 * @param backgroundColor the background color of the text (default is Black)
	 */
	SIMPLECORE_API static void warn(const AString& text,
		bool newLine = true,
		ConsoleColor::Type color = ConsoleColor::Type::LightGray,
		ConsoleColor::Type backgroundColor = ConsoleColor::Type::Black);

#ifdef SIMPLELIBS_TEST
#  include "console.h"
#  define LOGGER_LOG_TEST_INIT(x) Logger::info(x, true, ConsoleColor::Type::Yellow)
#  define LOGGER_LOG_TEST_BEGIN(x) Logger::info(x + AString(": "), false, ConsoleColor::Type::DarkYellow)
#  define LOGGER_LOG_RESULT(x) ConsoleColor::Type type = (x ? ConsoleColor::Type::DarkGreen : ConsoleColor::Type::DarkRed); Console::print(x, true, type); return x;
#endif // SIMPLELIBS_TEST

private:
	static File file_;

	/**
	 * Prints the prefix consisting of the timestamp and output mode.
	 * Example: [HH:mm:ss INFO]
	 *
	 * @param type the log type to be used
	 */
	static void printPrefix(LogType type);
};

#endif   // LOGGER_H
