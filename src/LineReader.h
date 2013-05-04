#ifndef CCONS_LINE_READER_H
#define CCONS_LINE_READER_H

//
// LineReader defines an interface to be implemented by derived classes for
// reading line input from the users.
//
// Part of ccons, the interactive console for the C programming language.
//
// Copyright (c) 2009 Alexei Svitkine. This file is distributed under the
// terms of MIT Open Source License. See file LICENSE for details.
//

#include <string>

namespace ccons {

//
// LineReader interface
//

class LineReader {

public:

	virtual ~LineReader() {}

	// Reads a line by presenting the specified prompt and pre-pending
	// the specified input string.
    virtual char * readLine(char *prompt, char *input) = 0;

};

}
#endif // CCONS_LINE_READER_H
