/**
 *  Copyright (C) 2015-2019
 *  Author Alvin Ahmadov <alvin.dev.ahmadov@gmail.com>
 *
 *  This file is part of pdf_cleaner
 *  License-Identifier: MIT License
 *  See README.md for more information.
 */
#include "Inspector.hpp"

#include <iostream>
#include <utility>

using namespace std;
using namespace PoDoFo;

namespace PDF
{
	bool matchesActionUri(PdfAction *action, string_view sv)
	{
		smatch m;
		auto   uriRegex = regex(sv.data());
		auto   uri      = action->GetURI().GetStringUtf8();
		auto   res      = regex_search(uri, m, uriRegex);
		return res;
	}

	Inspector::Inspector(boost::filesystem::path filePath)
			: m_state(State::Unedited),
			  m_keyName(),
			  m_filePath(std::move(filePath)),
			  m_document()
	{
		Init();
	}

	void Inspector::Delete(string_view uri, int pageIndex)
	{
		if (not m_document)
		{
			return;
		}

		if (not m_document->GetPageCount())
		{
			cerr << "No pages in document \'" << m_filePath.filename().generic_string() << '\'';
			return;
		}

		if (uri.empty())
		{
			return;
		}

		if (pageIndex < 0 or pageIndex > m_document->GetPageCount())
		{
			pageIndex = 0;
		}

		m_uri = uri;

		FindObjectName(pageIndex);

		if (m_state == State::NoMatch)
		{
			return;
		}

		PdfPage *page;

		for (; pageIndex < m_document->GetPageCount(); ++pageIndex)
		{
			page = m_document->GetPage(pageIndex);
			ProcessObject(page->GetObject());
			for (int annotIndex{};
			     annotIndex < page->GetNumAnnots();
			     ++annotIndex)
			{
				DeleteAnnotation(page, annotIndex);
			}
		}

		m_keyName.clear();
	}

	void Inspector::SetDocumentProperties(const DocumentProperty &props)
	{
		if (not m_document)
		{
			return;
		}
		props.Populate(m_document);
	}

	void Inspector::Write(string_view outputName)
	{
		if (not m_document)
		{
			return;
		}
		m_document->Write(outputName.data());
	}

	NODISCARD
	MAYBE_UNUSED
	string Inspector::GetStructure() const
	{
		stringstream stringStream{};

		auto      outputDev  = make_unique<PdfOutputDevice>(&stringStream);
		const int pagesCount = m_document->GetPageCount();
		PdfPage   *page      = m_document->GetPage(0);

		for (int i = 0; i < pagesCount - 1; ++i, page = m_document->GetPage(i))
		{
			auto dict = page->GetObject()->GetDictionary();
			dict.Write(outputDev.get(), EPdfWriteMode::ePdfWriteMode_Clean);
		}
		return stringStream.str();
	}

	bool Inspector::Done() const
	{
		return (m_state == State::Deleted);
	}

	void Inspector::Init()
	{
		auto fileName = m_filePath.generic_string();
		try
		{
			m_document = make_unique<PdfMemDocument>(fileName.data());
		}
		catch (PdfError &error)
		{
			error.AddToCallstack();
			cerr << "File \'" << fileName << "\' cannot be opened.\nExiting...";
			cerr << error.what();
			return;
		}
	}

	void Inspector::FindObjectName(int pageIndex)
	{
		KeywordMatcher kwm(m_uri.data());
		const int      pageCount = m_document->GetPageCount();

		do
		{
			if (pageIndex >= pageCount)
			{
				m_state = State::NoMatch;
				break;
			}
			auto page = m_document->GetPage(pageIndex++);
			ReadObjectName(page, kwm);
		} while (m_state != State::Ready);
	}

	void Inspector::ReadObjectName(PdfPage *page, KeywordMatcher kwm)
	{
		auto tokenizer = make_unique<PdfContentsTokenizer>(page);
		if (kwm.FindMatch(tokenizer.get()))
		{
			m_keyName = kwm.GetKW();
			m_state   = State::Ready;
		}
	}

	void Inspector::DeleteAnnotation(PdfPage *page, int index)
	{
		auto     annot  = page->GetAnnotation(index);
		if (auto action = annot->GetAction(); action && action->HasURI())
		{
			if (matchesActionUri(annot->GetAction(), m_uri))
			{
				page->DeleteAnnotation(index);
			}
		}
	}

	void Inspector::ProcessObject(PdfObject *object)
	{
		if (object->GetDataType() == EPdfDataType::ePdfDataType_Dictionary)
		{
			ProcessDictionary(&object->GetDictionary());
		}
	}

	void Inspector::ProcessDictionary(PdfDictionary *dictionary)
	{
		PdfName objectKeyName(m_keyName);
		
		try
		{
			for(auto& [key, object]: dictionary->GetKeys())
			{
				if(!object)
					return;
				
				if(key == objectKeyName)
				{
					bool removed = dictionary->RemoveKey(objectKeyName);
					if(removed)
					{
						m_state = State::Deleted;
					}
					continue;
				}
				ProcessObject(object);
			}
		} catch(exception &e)
		{
			printf("Exception was thrown: %s", e.what());
		}
	}
}
