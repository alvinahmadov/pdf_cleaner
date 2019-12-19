/**
 *  Copyright (C) 2015-2019
 *  Author Alvin Ahmadov <alvin.dev.ahmadov@gmail.com>
 *
 *  This file is part of pdf_cleaner
 *  License-Identifier: MIT License
 *  See README.md for more information.
 */
#pragma once

#include <mutex>
#include <string>
#include <string_view>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

namespace PDF
{
	namespace ProgramOptions    = boost::program_options;
	namespace FileSystem        = boost::filesystem;
	
	class FileHandler
	{
	public:
		using Path              = FileSystem::path;
		using Files             = std::vector<Path>;
		using DirectoryIterator = FileSystem::directory_iterator;
	
	protected:
		struct OptionsInfo
		{
			std::string longArg;
			
			std::string shortArg;
			
			std::string description;
			
			OptionsInfo(std::string_view aLongArg, std::string_view aShortArg, std::string_view aDescription);
			
			OptionsInfo(std::string_view aLongArg, std::string_view aDescription);
			
			std::string ConcatArgs();
		};
		
		struct ArgumentParser
		{
			struct Options
			{
				char prefix;
				
				int pageNum;
				
				bool recursive;
				
				bool replace;
				
				std::vector<std::string> uris;
				
				std::vector<std::string> paths;
				
				std::vector<OptionsInfo> info;
				
				Options();
				
				OptionsInfo& GetInfo(int index);
				
				bool EmptyPrefix() const;
			};
			
			int argc;
			
			char** argv;
			
			bool parsed;
			
			boost::program_options::variables_map variables;
			
			boost::program_options::options_description description;
			
			ArgumentParser(int count, char** values);
			
			void Init();
		};
	public:
		FileHandler(int argc, char** argv);
		
		FileHandler(const FileHandler& fh) noexcept;
		
		FileHandler(FileHandler&& fh) noexcept;
		
		FileHandler& operator=(const FileHandler& fn) noexcept;
		
		FileHandler& operator=(FileHandler&& fn) noexcept;
		
		void Parse();
		
		void Usage() const;
		
		const ArgumentParser::Options&
		GetOptions() const;
		
		const Files& GetFiles() const;
		
		std::string_view GetUri(size_t index = 0) const;
		
		bool HasPrefix() const;
		
		Path RemovePrefix(const Path& path) const noexcept;
	
	private:
		void Init() noexcept;
		
		void ParseFilePaths();
	
	private:
		mutable std::mutex m_mutex;
		
		ArgumentParser m_argParser;
		
		ArgumentParser::Options m_options;
		
		Files m_files;
	};
}