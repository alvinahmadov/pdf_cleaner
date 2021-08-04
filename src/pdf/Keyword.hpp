/**
 *  Copyright (C) 2015-2019
 *  Author Alvin Ahmadov <alvin.dev.ahmadov@gmail.com>
 *
 *  This file is part of pdf_cleaner
 *  License-Identifier: MIT License
 *  See README.md for more information.
 */
#pragma once

#include <string>
#include <regex>

namespace PoDoFo
{
	class PdfTokenizer;
}

namespace PDF
{
	class KeywordMatcher
	{
	public:
		explicit KeywordMatcher(std::string_view term);

		bool FindMatch(PoDoFo::PdfTokenizer *tokenizer);

		BOOST_ATTRIBUTE_NODISCARD
		std::string_view GetKW() const noexcept;

	private:
		bool m_isHead;

		bool m_hasMatch;

		std::regex m_regex;

		std::string_view m_term;

		std::string m_keyword;
	};
}
