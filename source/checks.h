//
//  checks.h
//  textengine
//
//  Created by Robert Dionne on 9/15/13.
//  Copyright (c) 2013 Robert Scott Dionne. All rights reserved.
//

#ifndef TEXTENGINE_CHECKS_H_
#define TEXTENGINE_CHECKS_H_

#include <cstdlib>
#include <iostream>
#include <string>

/**
 * Quits the program with an error message if !state.
 */
#define CHECK_STATE(state)\
  CheckState(#state, state, __LINE__, __FILE__);

/**
 * Quits the program with the given error message.
 */
#define FAIL(message)\
  Fail(message, __LINE__, __FILE__);

inline void Fail(const std::string &message, int line, const std::string &file);

/**
 * Prints the given error message if !state and notes the line and source file name.
 */
inline void CheckState(const std::string &message, bool state, int line, const std::string &file) {
  if (!state) {
    Fail(message + " violated", line, file);
  }
}

/**
 * Prints the given error message and notes the line and the source file name.
 */
inline void Fail(const std::string &message, int line, const std::string &file) {
  std::cerr << "ERROR: " << message << " on line " << line << " of " << file << std::endl;
  exit(1);
}

#endif
