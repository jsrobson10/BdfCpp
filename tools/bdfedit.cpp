
#include "../Bdf.h"

#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sstream>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>

std::string editor = "vim";
std::string indent = "\t";
std::string breaker = "\n";
std::string command;
int type = 0;

bool stringEndsWith(std::string str, std::string check)
{
	int str_size = str.size();
	int check_size = check.size();

	return str_size >= check_size && str.substr(str_size - check_size, str_size) == check;
}

void help() {
	std::cerr << "Usage: " << command << " [.bdf/.bdf.gz/.bdf.bz2/.hbdf] -m [none/human/bzip2/gzip]\n";
}

void onError(BdfError e) {
	std::cerr << "Error while reading file: " << e.getError() << "\n";
}

std::string endOfPath(std::string path)
{
	std::string ending;
	int start = 0;

	for(unsigned int i=0;i<path.size();i++)
	{
		if(path[i] == '/') {
			start = i + 1;
		}
	}

	for(unsigned int i=start;i<path.size();i++) {
		ending += path[i];
	}

	return ending;
}

BdfReader* editFile(std::string path)
{
	int pid = fork();

	if(pid > 0) {
		wait(NULL);	
	}

	else if(pid == 0)
	{
		const char* args[] = {"/usr/bin/env", editor.c_str(), path.c_str(), NULL};
		
		execv(args[0], (char* const*)args);
		
		exit(1);
	}

	else {
		exit(2);
	}

	std::ifstream in(path, std::ios::in);
	std::stringstream ss;
	char* buffer = new char[1024];

	while(in.read(buffer, 1024)) {
		ss.write(buffer, in.gcount());
	}

	ss.write(buffer, in.gcount());
	in.close();

	delete[] buffer;

	try {
		return new BdfReaderHuman(ss.str());
	}

	catch(BdfError &e) {
		onError(e);
		return NULL;
	}
}

void editBinaryFile(std::string path, std::string str, char** data, int* data_size)
{
	BdfReader* reader = new BdfReader(str.c_str(), str.size());
	
	char tmpf_template[] = "/tmp/bdfeditXXXXXX";
	std::string path_ending = endOfPath(path);
	std::string tmpf_path = std::string(mkdtemp(tmpf_template)) + "/" + path_ending + ".hbdf";
		
	std::ofstream tmpf_out(tmpf_path.c_str(), std::ios::out);
	
	// Show an empty file if the first objects type is undefined
	if(reader->getObject()->getType() != BdfTypes::UNDEFINED) {
		reader->serializeHumanReadable(tmpf_out, BdfIndent(indent, breaker));
	}

	tmpf_out.close();

	delete reader;

	reader = editFile(tmpf_path);

	if(reader == NULL) {
		*data = NULL;
		*data_size = 0;
		return;
	}

	rmdir(tmpf_path.c_str());

	reader->serialize(data, data_size);

	delete reader;
}

int main(int argc, char** argv)
{
	// Load settings
	{
		std::string file_settings = std::string(getenv("HOME")) + "/.bdfedit.hbdf";
		std::ifstream in(file_settings.c_str(), std::ios::in);
		std::stringstream ss;

		if(in.good())
		{
			char* buffer = new char[1024];

			while(in.read(buffer, 1024)) {
				ss.write(buffer, in.gcount());
			}

			ss.write(buffer, in.gcount());

			delete[] buffer;
		}

		in.close();
		BdfReader* reader;
		
		try {
			reader = new BdfReaderHuman(ss.str());
		}

		catch(BdfError &e) {
			reader = new BdfReader();
		}

		BdfObject* bdf = reader->getObject();
		BdfNamedList* nl = bdf->getNamedList();

		if(nl->get("editor")->getType() == BdfTypes::STRING) {
			editor = nl->get("editor")->getString();
		} else {
			nl->get("editor")->setString(editor);
		}

		if(nl->get("indent")->getType() == BdfTypes::STRING) {
			indent = nl->get("indent")->getString();
		} else {
			nl->get("indent")->setString(indent);
		}

		if(nl->get("break")->getType() == BdfTypes::STRING) {
			breaker = nl->get("break")->getString();
		} else {
			nl->get("break")->setString(breaker);
		}

		std::ofstream out(file_settings.c_str(), std::ios::out);

		reader->serializeHumanReadable(out, BdfIndent("\t", "\n"));
		
		delete reader;

		out.close();
	}

	command = "";

	if(argc > 0) {
		command = argv[0];
	}

	if(argc < 2) {
		help();
		return 1;
	}

	int mode = 0;

	for(int i=2;i<argc;i++)
	{
		if(std::strcmp(argv[i], "-m") == 0)
		{
			if(i > argc) {
				help();
				return 1;
			}

			std::string arg = argv[i+1];

			if(arg == "none") {
				mode = 1;
			}

			else if(arg == "human") {
				mode = 2;
			}

			/*else if(arg == "xz" || arg == "lzma") {
				mode = 3;
			}*/

			else if(arg == "gz" || arg == "gzip") {
				mode = 4;
			}

			else if(arg == "bz2" || arg == "bzip2") {
				mode = 5;
			}

			else {
				help();
				return 1;
			}

			i += 1;
		}

		else {
			help();
			return 1;
		}
	}

	std::string path(argv[1]);

	if(mode == 0)
	{
		if(stringEndsWith(path, ".bdf")) {
			mode = 1;
		}

		else if(stringEndsWith(path, ".hbdf")) {
			mode = 2;
		}

		/*else if(stringEndsWith(path, ".bdf.xz") || stringEndsWith(path, ".bdfxz")) {
			mode = 3;
		}*/

		else if(stringEndsWith(path, ".bdf.gz") || stringEndsWith(path, ".bdfgz")) {
			mode = 4;
		}

		else if(stringEndsWith(path, ".bdf.bz2") || stringEndsWith(path, ".bdfbz2")) {
			mode = 5;
		}

		else {
			help();
			return 1;
		}
	}

	if(mode == 1)
	{
		std::ifstream in(argv[1], std::ios::binary | std::ios::in);
		std::stringstream ss;

		if(in.good())
		{
			char* buffer = new char[1024];

			while(in.read(buffer, 1024)) {
				ss.write(buffer, in.gcount());
			}

			ss.write(buffer, in.gcount());

			delete[] buffer;
		}

		in.close();

		char* data;
		int data_size;
		editBinaryFile(path, ss.str(), &data, &data_size);

		if(data == NULL) {
			return 1;
		}

		std::ofstream out(argv[1], std::ios::binary | std::ios::out);

		for(int i=0;i<data_size;i+=1024) {
			out.write(data + i, std::min(i + 1024, data_size));
		}

		delete[] data;
		out.close();
	}

	/*else if(mode == 3)
	{
		std::stringstream ss;
	
		{
			std::ifstream in(argv[1], std::ios::binary | std::ios::in);
	
			if(in.good())
			{	
				boost::iostreams::filtering_streambuf<boost::iostreams::input> filter;
				filter.push(boost::iostreams::lzma_decompressor());
				filter.push(in);
				boost::iostreams::copy(filter, ss);
			}

			in.close();
		}

		char* data;
		int data_size;
		editBinaryFile(path, ss.str(), &data, &data_size);

		if(data == NULL) {
			return 1;
		}

		{			
			std::ofstream out(argv[1], std::ios::binary | std::ios::out);
			std::stringstream in(std::string(data, data_size));

			boost::iostreams::filtering_streambuf<boost::iostreams::input> filter;
			filter.push(boost::iostreams::lzma_compressor());
			filter.push(in);
			boost::iostreams::copy(filter, out);

			out.close();
		}

		delete[] data;
	}*/

	else if(mode == 4)
	{
		std::stringstream ss;
	
		{
			std::ifstream in(argv[1], std::ios::binary | std::ios::in);
	
			if(in.good())
			{	
				boost::iostreams::filtering_streambuf<boost::iostreams::input> filter;
				filter.push(boost::iostreams::gzip_decompressor());
				filter.push(in);
				boost::iostreams::copy(filter, ss);
			}

			in.close();
		}

		char* data;
		int data_size;
		editBinaryFile(path, ss.str(), &data, &data_size);

		if(data == NULL) {
			return 1;
		}

		{			
			std::ofstream out(argv[1], std::ios::binary | std::ios::out);
			std::stringstream in(std::string(data, data_size));

			boost::iostreams::filtering_streambuf<boost::iostreams::input> filter;
			filter.push(boost::iostreams::gzip_compressor());
			filter.push(in);
			boost::iostreams::copy(filter, out);

			out.close();
		}

		delete[] data;
	}

	else if(mode == 5)
	{
		std::stringstream ss;
	
		{
			std::ifstream in(argv[1], std::ios::binary | std::ios::in);
	
			if(in.good())
			{
				boost::iostreams::filtering_streambuf<boost::iostreams::input> filter;
				filter.push(boost::iostreams::bzip2_decompressor());
				filter.push(in);
				boost::iostreams::copy(filter, ss);
			}

			in.close();
		}

		char* data;
		int data_size;
		editBinaryFile(path, ss.str(), &data, &data_size);

		if(data == NULL) {
			return 1;
		}

		{			
			std::ofstream out(argv[1], std::ios::binary | std::ios::out);
			std::stringstream in(std::string(data, data_size));

			boost::iostreams::filtering_streambuf<boost::iostreams::input> filter;
			filter.push(boost::iostreams::bzip2_compressor());
			filter.push(in);
			boost::iostreams::copy(filter, out);

			out.close();
		}

		delete[] data;
	}

	else if(mode == 2)
	{
		BdfReader* reader = editFile(path);
		
		if(reader != NULL) {
			delete reader;
		}
	}
	
	return 0;
}
