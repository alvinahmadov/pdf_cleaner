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

#include "DocumentProperty.hpp"
#include "Keyword.hpp"

namespace PDF
{
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
		explicit Inspector(boost::filesystem::path filePath);

		void Delete(std::string_view uri, int pageIndex = 0);

		void SetDocumentProperties(const DocumentProperty &props);

		void Write(std::string_view outputName);

		NODISCARD
		MAYBE_UNUSED
		std::string GetStructure() const;

		NODISCARD
		bool Done() const;

	private:
		void Init();

		void FindObjectName(int pageIndex = 0);

		void ReadObjectName(PoDoFo::PdfPage *page, KeywordMatcher kwm);

		void DeleteAnnotation(PoDoFo::PdfPage *page, int index);

		void ProcessObject(PoDoFo::PdfObject *object);

		void ProcessDictionary(PoDoFo::PdfDictionary *dictionary);

	private:
		State m_state;

		std::string m_keyName;

		std::string_view m_uri;

		boost::filesystem::path m_filePath;

		std::unique_ptr<PoDoFo::PdfMemDocument> m_document;
	};
}
