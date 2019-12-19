/**
 *  Copyright (C) 2015-2019
 *  Author Alvin Ahmadov <alvin.dev.ahmadov@gmail.com>
 *
 *  This file is part of pdf_cleaner
 *  License-Identifier: MIT License
 *  See README.md for more information.
 */
#include <regex>
#include <iostream>
#include <podofo/podofo.h>

#include "Keyword.hpp"

using namespace PoDoFo;
using namespace std;

namespace PDF
{
	struct Token
	{
		const char* data;
		
		EPdfTokenType type;
		
		inline static const char* delimiter = "/";
		
		inline bool Compare(const char* aData) const
		{
			return !strcmp(data, aData);
		}
		
		inline bool IsDelimiter() const
		{
			return Compare(delimiter);
		}
	};
	
	KeywordMatcher::KeywordMatcher(string_view term)
			: m_isHead(false),
			  m_hasMatch(false),
			  m_term(term),
			  m_keyword()
	{
		m_regex = regex(m_term.begin(), m_term.end());
	}
	
	bool KeywordMatcher::FindMatch(PdfTokenizer* tokenizer)
	{
		Token token;
		while (tokenizer->GetNextToken(token.data, &token.type))
		{
			if (token.type == EPdfTokenType::ePdfTokenType_Delimiter)
				if (token.IsDelimiter())
				{
					if (not m_isHead)
					{
						m_isHead = true;
						continue;
					} else
					{
						m_keyword.clear();
						m_isHead = false;
					}
				}
			
			if (m_isHead)
			{
				if (m_keyword.empty())
				{
					m_keyword = token.data;
				} else
				{
					if (regex_search(token.data, m_regex))
					{
						m_hasMatch = true;
						break;
					}
				}
			}
		}
		return m_hasMatch;
	}
	
	string_view KeywordMatcher::GetKW() const noexcept
	{
		return m_keyword;
	}
}