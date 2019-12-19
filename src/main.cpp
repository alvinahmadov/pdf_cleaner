#include <thread>
#include <iostream>
#include "pdf/Inspector.hpp"
#include "pdf/FileHandler.hpp"

using namespace PDF;
using namespace std;

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;

vector<string>
split(string& f, char by)
{
	vector<string> parts {};
	size_t begin {}, pos {};
	
	while (begin < f.size())
	{
		if (pos == string::npos)
			break;
		
		pos = f.find(by, begin);
		
		if (f.at(begin) == ' ')
			++begin;
		
		parts.push_back(f.substr(begin, pos - begin - 1));
		begin = pos + 1;
	}
	
	return parts;
}

void cleanFiles(const FileHandler&, FileHandler::Path&, Properties&& = {});

void pdfCleaner(int, char**);

int main(int argc, char** argv)
{
	try
	{
		pdfCleaner(argc, argv);
	} catch (std::exception& e)
	{
		cerr << e.what() << endl;
	}
	
	return 0;
}

Properties createPropertyData(const FileHandler::Path& path, char prefix)
{
	auto fileName = path.filename().generic_string();
	Properties properties {};
	
	fileName.erase(fileName.find(prefix), 1);
	auto parts = split(fileName, '-');
	
	if (parts.size())
	{
		if (parts.size() == 1)
			properties = Properties("", parts.at(0));
		else
			properties = Properties(parts.at(1), parts.at(0));
	}
	
	return properties;
}

void pdfCleaner(int argc, char** argv)
{
	auto handler = FileHandler(argc, argv);
	std::vector<std::thread> threads {};
	
	if (!argc)
	{
		handler.Usage();
		return;
	} else
		handler.Parse();
	
	auto files = handler.GetFiles();
	
	for (auto& file : files)
	{
		auto propData = createPropertyData(file, handler.GetOptions().prefix);
		threads.push_back(std::thread(&cleanFiles,
									  std::ref(handler), std::ref(file), std::move(propData)));
		threads.back().join();
	}
}

void cleanFiles(const FileHandler& handler,
				FileHandler::Path& filename,
				Properties&& props)
{
	string outputName = ( handler.HasPrefix()
						  ? handler.RemovePrefix(filename).generic_string()
						  : filename.generic_string());
	
	auto inspector = make_unique<PDF::Inspector>(filename.generic_string());
	bool done {};
	cout << outputName << endl;
	for (auto& u : handler.GetOptions().uris)
	{
		inspector->Delete(u, handler.GetOptions().pageNum);
		done = inspector->Done();
		if (done)
			break;
	}
	
	if (done)
	{
		inspector->SetDocumentProperties(props);
		inspector->Write(outputName);
		
		if (handler.HasPrefix() and handler.GetOptions().replace)
			boost::filesystem::remove(filename);
	}
}