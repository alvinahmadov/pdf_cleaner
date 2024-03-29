#include <thread>
#include <iostream>

#include "pdf/DocumentProperty.hpp"
#include "pdf/Inspector.hpp"
#include "pdf/FileHandler.hpp"

using namespace PDF;
using namespace std;

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;

vector<string> split(string &f, char by)
{
	size_t         begin{}, pos{};
	vector<string> parts{};

	while (begin < f.size())
	{
		if (pos == string::npos)
		{
			break;
		}

		pos = f.find(by, begin);

		if (f.at(begin) == ' ')
		{
			++begin;
		}

		parts.push_back(f.substr(begin, pos - begin - 1));
		begin = pos + 1;
	}

	return parts;
}

void cleanFiles(const FileHandler &, FileHandler::Path &, DocumentProperty && = {});

void pdfCleaner(int, char **);

int main(int argc, char **argv)
{
	try
	{
		pdfCleaner(argc, argv);
	}
	catch (std::exception &e)
	{
		cerr << e.what() << endl;
	}

	return 0;
}

DocumentProperty createPropertyData(const FileHandler::Path &path, char prefix)
{
	auto       fileName = path.filename().generic_string();
	DocumentProperty properties{};

	fileName.erase(fileName.find(prefix), 1);
	auto parts = split(fileName, '-');

	if (!parts.empty())
	{
		if (parts.size() == 1)
		{
			properties = DocumentProperty("", parts.at(0));
		}
		else
		{
			properties = DocumentProperty(parts.at(1), parts.at(0));
		}
	}

	return properties;
}

void pdfCleaner(int argc, char **argv)
{
	auto                     handler = FileHandler(argc, argv);
	std::vector<std::thread> threads{};

	if (!argc)
	{
		handler.Usage();
		return;
	}
	else
	{
		handler.Parse();
	}

	auto files = handler.GetFiles();

	for (auto &file : files)
	{
		auto propData = createPropertyData(file, handler.GetOptions().prefix);
		threads.emplace_back(&cleanFiles, std::ref(handler), std::ref(file), std::move(propData));
		threads.back().join();
	}
}

void cleanFiles(const FileHandler &handler,
                FileHandler::Path &filename,
                DocumentProperty &&props)
{
	string outputName = (handler.HasPrefix()
	                     ? handler.RemovePrefix(filename).generic_string()
	                     : filename.generic_string());

	auto inspector = make_unique<PDF::Inspector>(filename.generic_string());
	bool done{};
	cout << outputName << endl;
	for (auto &u : handler.GetOptions().uris)
	{
		inspector->Delete(u, handler.GetOptions().pageNum);
		done = inspector->Done();
		if (done)
		{
			break;
		}
	}

	if (done)
	{
		inspector->SetDocumentProperties(props);
		inspector->Write(outputName);

		if (handler.HasPrefix() and handler.GetOptions().replace)
		{
			boost::filesystem::remove(filename);
		}
	}
}
