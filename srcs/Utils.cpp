#include "Utils.hpp"
#include <algorithm> // for std::find
#include <vector>
#include <string>
#include <iostream>

/**
 * Checks if the provided string is a valid HTTP method.
 *
 * @param method A string representing the HTTP method to validate.
 * @return True if the method is a recognized HTTP method, false otherwise.
 */
bool isValidHttpMethod(const std::string &method)
{
	std::vector<std::string> validMethods;
	validMethods.push_back("GET");
	validMethods.push_back("POST");
	validMethods.push_back("DELETE");

	// std::find checks if 'method' is in 'validMethods'
	// std::find returns an iterator pointing to the found element, or validMethods.end() if it didn't find anything
	return std::find(validMethods.begin(), validMethods.end(), method) != validMethods.end();
}

/**
 * @struct IsNotSpace
 * @brief Functor to check if a character is not a space.
 */
struct IsNotSpace
{
    /**
     * @brief Overloads the function call operator.
     * @param c The character to check.
     * @return True if the character is not a space, false otherwise.
     */
    bool operator()(char c) const
    {
        return !std::isspace(static_cast<unsigned char>(c));
    }
};

/**
 * @brief Trims leading and trailing spaces from a string.
 * @param str The string to trim.
 * @return The trimmed string.
 */
std::string trimSpaces(const std::string& str)
{
	if (str.empty() || std::find_if(str.begin(), str.end(), IsNotSpace()) == str.end())
		return "";

	std::string::const_iterator start = std::find_if(str.begin(), str.end(), IsNotSpace());
	std::string::const_reverse_iterator end = std::find_if(str.rbegin(), str.rend(), IsNotSpace());

	return std::string(start, end.base());
}

/**
 * Checks if a string represents a valid port number.
 *
 * A valid port number is a numerical string that represents a number between 1 and 65535.
 *
 * @param str The string to check.
 * @return true if the string represents a valid port number, false otherwise.
 */
bool isValidPort(const std::string &str)
{
	// Check if the string is not empty
	// if (str.empty())
	// {
	// 	return false;
	// }

	// Check if all characters in the string are digits
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (!std::isdigit(*it))
		{
			return false;
		}
	}

	// Convert the string to an integer and check if it's between 1 and 65535
	int port = std::atoi(str.c_str());
	// if (port < 1 || port > 65535)
	// {
	// 	return false;
	// }
	return port;
}


/**
 * @brief Removes semicolons from the end of a string.
 *
 * This function takes a string as input and removes any semicolons that appear at the end of the string.
 *
 * @param str The input string to be trimmed.
 * @return The trimmed string with semicolons removed from the end.
 */
std::string trimSemicolons(const std::string& str) {
    std::string trimmed = str;
    while (!trimmed.empty() && trimmed[trimmed.size() - 1] == ';') {
        trimmed.erase(trimmed.size() - 1);
    }
    return trimmed;
}
