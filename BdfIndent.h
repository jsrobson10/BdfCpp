
#ifndef BDFINDENT_H_
#define BDFINDENT_H_

#include <string>

class BdfIndent
{
public:

	std::string indent;
	std::string breaker;

	BdfIndent(std::string indent, std::string breaker);
};

#endif /* BDFINDENT_H_ */
