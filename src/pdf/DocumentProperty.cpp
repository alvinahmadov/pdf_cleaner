/**
 *  Copyright (C) 2015-2022
 *  Author Alvin Ahmadov <alvin.dev.ahmadov@gmail.com>
 *
 *  This file is part of pdf_cleaner
 *  License-Identifier: MIT License
 *  See README.md for more information.
 */

#include "DocumentProperty.hpp"

using PdfMemDocumentPtr = std::unique_ptr<PoDoFo::PdfMemDocument>;

namespace PDF
{
	DocumentProperty::DocumentProperty(
			std::string_view aTitle,
			std::string_view aAuthor,
			std::string_view aSubject,
			std::string_view aCreator,
			std::string_view aProducer,
			std::string_view aKeywords
	)
			: title(aTitle),
			  author(aAuthor),
			  subject(aSubject),
			  creator(aCreator),
			  producer(aProducer),
			  keywords(aKeywords)
	{}
	
	void DocumentProperty::Populate(PdfMemDocumentPtr &document) const
	{
		auto pdfInfo = document->GetInfo();
		pdfInfo->SetTitle(title);
		pdfInfo->SetAuthor(author);
		pdfInfo->SetSubject(subject);
		pdfInfo->SetCreator(creator);
		pdfInfo->SetProducer(producer);
		pdfInfo->SetKeywords(keywords);
	}
}