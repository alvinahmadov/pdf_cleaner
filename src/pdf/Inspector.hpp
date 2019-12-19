/**
 *  Copyright (C) 2015-2019
 *  Author Alvin Ahmadov <alvin.dev.ahmadov@gmail.com>
 *
 *  This file is part of pdf_cleaner
 *  License-Identifier: MIT License
 *  See README.md for more information.
 */
#pragma once

#include <memory>
#include <boost/filesystem.hpp>
#include <podofo/podofo.h>

#include "Keyword.hpp"

namespace PDF
{
	struct Properties
	{
		std::string title;
		
		std::string author;
		
		std::string subject;
		
		std::string creator;
		
		std::string producer;
		
		std::string keywords;
		
		Properties() = default;
		
		Properties(std::string_view aTitle, std::string_view aAuthor,
				   std::string_view aSubject = {},
				   std::string_view aCreator = "Adobe InDesign CS6 (Windows)",
				   std::string_view aProducer = "Adobe PDF Library 10.0.1",
				   std::string_view aKeywords = {});
		
		void Populate(std::unique_ptr<PoDoFo::PdfMemDocument>& document) const;
	};
	
	class Inspector
	{
		enum State
		{
			Unedited, // Document not edited
			Deleted,  // Data deleted
			Ready,    // Ready to delete
			NoMatch   // document doesn't contain search data
		};
	public:
		Inspector(const boost::filesystem::path& filePath);
		
		void Delete(std::string_view uri, int pageIndex = 0);
		
		void SetDocumentProperties(const Properties& props);
		
		void Write(std::string_view outputName);
		
		std::string GetStructure() const;
		
		bool Done() const;
	
	private:
		void Init();
		
		void FindObjectName(int pageIndex = 0);
		
		void ReadObjectName(PoDoFo::PdfPage* page, KeywordMatcher kwm);
		
		void DeleteAnnotation(PoDoFo::PdfPage* page, int index);
		
		void ProcessObject(PoDoFo::PdfObject* object);
		
		void ProcessDictionary(PoDoFo::PdfDictionary* dictionary);
	
	private:
		State m_state;
		
		std::string m_keyName;
		
		std::string_view m_uri;
		
		boost::filesystem::path m_filePath;
		
		std::unique_ptr<PoDoFo::PdfMemDocument> m_document;
	};
}