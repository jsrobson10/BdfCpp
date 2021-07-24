
#ifndef BDFHELPERS_HPP_
#define BDFHELPERS_HPP_

#include <string>

namespace BdfHelpers
{
	std::string serializeString(std::string v);

	void put_netsl(char* data, int64_t num);
	void put_netsi(char* data, int32_t num);
	void put_netss(char* data, int16_t num);
	void put_netul(char* data, uint64_t num);
	void put_netui(char* data, uint32_t num);
	void put_netus(char* data, uint16_t num);
	void put_netf(char* data, float num);
	void put_netd(char* data, double num);

	int64_t get_netsl(const char* data);
	int32_t get_netsi(const char* data);
	int16_t get_netss(const char* data);
	uint64_t get_netul(const char* data);
	uint32_t get_netui(const char* data);
	uint16_t get_netus(const char* data);
	float get_netf(const char* data);
	double get_netd(const char* data);
}

#endif
