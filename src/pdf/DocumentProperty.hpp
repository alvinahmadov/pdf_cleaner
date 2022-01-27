/**
 *  Copyright (C) 2015-2022
 *  Author Alvin Ahmadov <alvin.dev.ahmadov@gmail.com>
 *
 *  This file is part of pdf_cleaner
 *  License-Identifier: MIT License
 *  See README.md for more information.
 */

#pragma once

#include <string>

#include <podofo/podofo.h>

#include "Common.hpp"

namespace PDF
{
	struct DocumentProperty
	{
		std::string title;
		
		std::string author;
		
		std::string subject;
		
		std::string creator;
		
		std::string producer;
		
		std::string keywords;
		
		DocumentProperty() = default;
		
		MAYBE_UNUSED
		DocumentProperty(std::string_view aTitle, std::string_view aAuthor,
		                 std::string_view aSubject = {},
		                 std::string_view aCreator = "Adobe InDesign CS6",
		                 std::string_view aProducer = "Adobe PDF Library 10.0.1",
		                 std::string_view aKeywords = {});
		
		void Populate(std::unique_ptr<PoDoFo::PdfMemDocument> &document) const;
	};
}