/**
 *  Copyright (C) 2015-2019
 *  Author Alvin Ahmadov <alvin.dev.ahmadov@gmail.com>
 *
 *  This file is part of pdf_cleaner
 *  License-Identifier: MIT License
 *  See README.md for more information.
 */
#include <iostream>

#include "FileHandler.hpp"

using namespace std;
using namespace boost::program_options;

namespace PDF
{
	bool contains(const string &str, char v)
	{
		return str.find(v) != string::npos;
	}

	static const char *g_pdfExtension{".pdf"};

	FileHandler::
	OptionsInfo::OptionsInfo(string_view aLongArg,
	                         string_view aShortArg,
	                         string_view aDescription)
			: longArg(aLongArg),
			  shortArg(aShortArg),
			  description(aDescription)
	{}

	FileHandler::
	OptionsInfo::OptionsInfo(string_view aLongArg,
	                         string_view aDescription)
			: longArg(aLongArg),
			  shortArg(),
			  description(aDescription)
	{}

	string FileHandler::
	OptionsInfo::ConcatArgs() const
	{
		string res{};
		res.append(this->longArg)
				.append(",")
				.append(this->shortArg);

		return res;
	}

	FileHandler::
	ArgumentParser::ArgumentParser(int count, char **values)
			: argc{count},
			  argv{values},
			  parsed{}
	{}

	void FileHandler::
	ArgumentParser::Init()
	{
		store(parse_command_line(this->argc, this->argv, this->description),
		      this->variables);
		notify(this->variables);
	}

	BOOST_ATTRIBUTE_UNUSED
	FileHandler::OptionsInfo &
	FileHandler::ArgumentParser::
	Options::GetInfo(int index)
	{
		return this->info.at(index);
	}

	bool FileHandler::ArgumentParser::
	Options::EmptyPrefix() const
	{
		char empty{};
		return this->prefix == empty;
	}

	FileHandler::ArgumentParser::
	Options::Options()
			: prefix{},
			  pageNum{0},
			  recursive{false},
			  replace{false}
	{
		this->info.emplace_back("help", "h", "Helps");
		this->info.emplace_back("dir", "d", "Directory to PDF files");
		this->info.emplace_back("uri", "u", "Uri regex to delete");
		this->info.emplace_back("prefix", "p", "Prefix to specific files with embedded uris");
		this->info.emplace_back("number", "n", "Page number to start from");
		this->info.emplace_back("replace", "R", "Replace original files with edited");
		this->info.emplace_back("recursive", "r", "Parse recursive");
	}

	FileHandler::FileHandler(int argc, char **argv)
			: m_argParser(argc, argv),
			  m_options(),
			  m_files()
	{
		Init();
	}

	FileHandler::FileHandler(const FileHandler &fh) noexcept
			: m_argParser(fh.m_argParser),
			  m_options(fh.m_options),
			  m_files(fh.m_files)
	{}

	FileHandler::FileHandler(FileHandler &&fh) noexcept
			: m_argParser(std::move(fh.m_argParser)),
			  m_options(std::move(fh.m_options)),
			  m_files(std::move(fh.m_files))
	{}

	FileHandler &
	FileHandler::operator=(const FileHandler &fh) noexcept
	{
		if (this != &fh)
		{
			FileHandler copy(fh);
			std::swap(*this, copy);
		}
		return *this;
	}

	FileHandler &
	FileHandler::operator=(FileHandler &&fh) noexcept
	{
		if (this != &fh)
		{
			FileHandler mv{std::move(fh)};
			std::swap(*this, mv);
		}
		return *this;
	}

	void FileHandler::Parse()
	{
		auto pathArg    = m_options.info.at(1).longArg;
		auto uriArg     = m_options.info.at(2).longArg;
		auto prefArg    = m_options.info.at(3).longArg;
		auto pageNumArg = m_options.info.at(4).longArg;
		auto replArg    = m_options.info.at(5).longArg;
		auto recArg     = m_options.info.at(6).longArg;

		if (not m_argParser.argc)
		{
			Usage();
			return;
		}

		m_argParser.Init();

		// Paths
		if (m_argParser.variables.count(pathArg))
		{
			m_options.paths = m_argParser.variables[pathArg].as<vector<string>>();
		}
		// Uris
		if (m_argParser.variables.count(uriArg))
		{
			m_options.uris = m_argParser.variables[uriArg].as<vector<string>>();
		}
		// Prefix
		if (m_argParser.variables.count(prefArg))
		{
			m_options.prefix = m_argParser.variables[prefArg].as<char>();
		}
		// Page Number
		if (m_argParser.variables.count(pageNumArg))
		{
			m_options.pageNum = m_argParser.variables[pageNumArg].as<int>();
		}
		// Replace?
		if (m_argParser.variables.count(replArg))
		{
			m_options.replace = m_argParser.variables[replArg].as<bool>();
		}
		// Recursive?
		if (m_argParser.variables.count(recArg))
		{
			m_options.recursive = m_argParser.variables[recArg].as<bool>();
		}

		if (not m_options.uris.empty() and not m_options.paths.empty())
		{
			m_argParser.parsed = true;
		}
		else
		{
			Usage();
			return;
		}

		ParseFilePaths();
	}

	void FileHandler::Usage() const
	{
		m_argParser.description.print(cout);
	}

	const FileHandler::ArgumentParser::Options &
	FileHandler::GetOptions() const
	{
		return m_options;
	}

	const FileHandler::Files &
	FileHandler::GetFiles() const
	{
		return m_files;
	}

	BOOST_ATTRIBUTE_UNUSED
	string_view FileHandler::GetUri(size_t index) const
	{
		return m_options.uris.at(index);
	}

	bool FileHandler::HasPrefix() const
	{
		return not m_options.EmptyPrefix();
	}

	FileHandler::Path
	FileHandler::RemovePrefix(const Path &path) const noexcept
	{
		char     empty{};
		if (auto pref     = m_options.prefix; pref == empty)
		{
			return path;
		}

		std::lock_guard l(m_mutex);

		size_t     index;
		auto       genStr = path.generic_string();
		const auto &npos  = string::npos;

		if ((index = genStr.find(m_options.prefix)) != npos)
		{
			genStr.erase(index, 1);
		}

		return Path(genStr);
	}

	void FileHandler::Init() noexcept
	{
		auto info = m_options.info;
		m_argParser
				.description
				.add_options()
						// Help
						(m_options.info.at(0).ConcatArgs().data(),
						 m_options.info[0].description.data())
						// Path -d
						(m_options.info.at(1).ConcatArgs().data(),
						 value<vector<string>>()->multitoken(),
						 m_options.info[1].description.data())
						// Uri -u
						(m_options.info.at(2).ConcatArgs().data(),
						 value<vector<string>>()->multitoken(),
						 m_options.info[2].description.data())
						// Prefix -p
						(m_options.info.at(3).ConcatArgs().data(),
						 value<char>(), m_options.info[3].description.data())
						// Page Number -n
						(m_options.info.at(4).ConcatArgs().data(),
						 value<int>()->default_value(0), m_options.info[4].description.data())
						// Replace -R
						(m_options.info.at(5).ConcatArgs().data(),
						 value<bool>()->implicit_value(true),
						 m_options.info[5].description.data())
						// Recursive -r
						(m_options.info.at(6).ConcatArgs().data(),
						 value<bool>()->implicit_value(true),
						 m_options.info[6].description.data());
	}

	void FileHandler::ParseFilePaths()
	{
		std::lock_guard l(m_mutex);

		for (auto &pathStr : m_options.paths)
		{
			auto path = Path(pathStr);

			if (not path.empty())
			{
				DirectoryIterator dirIter(path);

				for (auto &iter : dirIter)
				{
					const auto &content = iter.path();

					if (content.has_extension())
					{
						if (content.extension().compare(g_pdfExtension))
						{
							continue;
						}
					}

					if (string fileName = content.filename().generic_string();
							contains(fileName, m_options.prefix))
					{
						m_files.push_back(content.generic_string());
					}
				}
			}
		}
	}
}
