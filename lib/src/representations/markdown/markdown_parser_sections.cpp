/*
 markdown_parser_sections.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "markdown_parser_sections.h"

const char *DEFAULT_NAME= "A thing";
const char *DEFAULT_MARKDOWN_NAME= "Outline";
const char *DEFAULT_MARKDOWN_DESCRIPTION= "An outline.";

using namespace std;

namespace m8r {

/*
 * MarkdownParserSections
 */

MarkdownParserSections::MarkdownParserSections(MarkdownLexerSections& lexer)
    : lexer(lexer)
{
    this->ast = nullptr;
}

MarkdownParserSections::~MarkdownParserSections()
{
    if(ast) {
        for(MarkdownAstNodeSection*& node:*ast) {
            delete node;
        }
        delete ast;
        ast = nullptr;
    }
}

void MarkdownParserSections::parse(void)
{
    if(lexer.size()) {
        if(ast!=nullptr) {
            if(!ast->empty()) {
                // TODO delete members
            } // else reuse it
        } else {
            ast = new vector<MarkdownAstNodeSection*>();
        }
        markdownRule();
    }
}

const MarkdownLexem* MarkdownParserSections::lookahead(size_t offset)
{
    if(offset<lexer.size()) {
        return lexer[offset];
    } else {
        return nullptr;
    }
}

const MarkdownLexem* MarkdownParserSections::lookahead(MarkdownLexemType lexemType, size_t offset)
{
    if(offset<lexer.size() && lexer[offset]->getType()==lexemType) {
        return lexer[offset];
    } else {
        return nullptr;
    }
}

const MarkdownLexem* MarkdownParserSections::lookaheadNot(MarkdownLexemType lexemType, size_t offset)
{
    if(offset<lexer.size() && lexer[offset]->getType()!=lexemType) {
        return lexer[offset];
    } else {
        return nullptr;
    }
}

void MarkdownParserSections::markdownRule(void)
{
    MarkdownAstNodeSection* section;
    size_t offset = 0;
    while((section=sectionRule(offset))!=nullptr) {
        ast->push_back(section);
    }
}

MarkdownAstNodeSection* MarkdownParserSections::sectionRule(size_t& offset)
{
    const MarkdownLexem* next;
    if((next=lookahead(MarkdownLexemType::SECTION,offset+1))!=nullptr) {
        unsigned depth = next->getDepth();
        MarkdownAstNodeSection* result=sectionHeaderRule(++offset);
        if(result!=nullptr) {
            result->setDepth(depth);
            result->setBody(sectionBodyRule(offset));
            return result;
        }
    }
    return nullptr;
}

void MarkdownParserSections::skipBr(size_t& offset)
{
    // skip section line's BR
    if(lookahead(MarkdownLexemType::BR, offset + 1) != nullptr) {
        ++offset;
    }
}

MarkdownAstNodeSection* MarkdownParserSections::sectionHeaderRule(size_t& offset)
{
    const MarkdownLexem* next;
    string* title;
    // seek to section token - may skip a part of the document
    while((next=lookahead(offset+1))!=nullptr
            &&
          next->getType()!=MarkdownLexemType::SECTION)
    {
        if((title=sectionTitleRule(offset))!=nullptr) {
            MarkdownAstNodeSection* result = new MarkdownAstNodeSection(title);
            if(sectionMetadataRule(result->getMetadata(), offset)) {
                // skip section line's BR
                skipBr(offset);
                // TODO
                return result;
            } else {
                // skip section line's BR
                skipBr(offset);
                return result;
            }
        } else {
            offset++;
        }
    }
    return nullptr;
}

void MarkdownParserSections::skipWhitespaces(size_t& offset)
{
    while(lookahead(MarkdownLexemType::WHITESPACES,offset+1)!=nullptr) {
        ++offset;
    }
}

void MarkdownParserSections::skipEOL(size_t& offset)
{
    while(lookaheadNot(MarkdownLexemType::BR,offset+1)!=nullptr) {
        ++offset;
    }
}

void MarkdownParserSections::skipSectionBody(size_t& offset)
{
    while(lookaheadNot(MarkdownLexemType::SECTION,offset+1)!=nullptr) {
        ++offset;
    }
}

string* MarkdownParserSections::sectionTitleRule(size_t& offset)
{
    skipWhitespaces(offset);
    const MarkdownLexem* next;
    if((next=lookahead(offset+1))!=nullptr
          &&
        next->getType()==MarkdownLexemType::TEXT)
    {
        string* title = new string();
        string* text = nullptr;
        while((next=lookahead(offset+1))!=nullptr
                &&
              (next->getType()==MarkdownLexemType::WHITESPACES || next->getType()==MarkdownLexemType::TEXT))
        {
            text = lexer.getText(next);
            if(text!=nullptr) {
                title->append(*text);
                delete text;
                text = nullptr;
            } else {
                if(title->size()) {
                    title->append(" ");
                }
            }
            ++offset;
        }
        text = lexer.getText(next);
        if(text!=nullptr) {
            if(next->getType()==MarkdownLexemType::WHITESPACES) {
                title->erase(title->size()-1, text->size());
            }
            delete text;
            text = nullptr;
        }
        return title;
    } else {
        return nullptr;
    }
}

bool MarkdownParserSections::sectionMetadataRule(
        MarkdownAstSectionMetadata& meta,
        size_t& offset)
{
    skipWhitespaces(offset);
    const MarkdownLexem* next;
    if((next=lookahead(MarkdownLexemType::HTML_COMMENT_BEGIN,offset+1))!=nullptr) {
        skipWhitespaces(++offset);
        if((next=lookahead(MarkdownLexemType::META_BEGIN,offset+1))!=nullptr) {
            skipWhitespaces(++offset);
            bool done=false;
            time_t t;
            vector<string*>* tags;
            while(!done && (next=lookahead(offset+1))!=nullptr) {
                offset++;
                switch(next->getType()) {
                case MarkdownLexemType::META_PROPERTY_created:
                    if((t = parsePropertyValueTimestamp(offset))!=0) {
                        meta.setCreated(t);
                    }
                    break;
                case MarkdownLexemType::META_PROPERTY_importance:
                    meta.setImportance(parsePropertyValueFraction(offset));
                    break;
                case MarkdownLexemType::META_PROPERTY_tags:
                    tags = parsePropertyValueTags(offset);
                    meta.setTags(tags);
                    delete tags;
                    break;
                case MarkdownLexemType::META_PROPERTY_modified:
                    if((t = parsePropertyValueTimestamp(offset))!=0) {
                        meta.setModified(t);
                    }
                    break;
                case MarkdownLexemType::META_PROPERTY_progress:
                    meta.setProgress(parsePropertyValuePercent(offset));
                    break;
                case MarkdownLexemType::META_PROPERTY_read:
                    if((t = parsePropertyValueTimestamp(offset))!=0) {
                        meta.setRead(t);
                    }
                    break;
                case MarkdownLexemType::META_PROPERTY_reads:
                    meta.setReads(parsePropertyValueInteger(offset));
                    break;
                case MarkdownLexemType::META_PROPERTY_revision:
                    meta.setRevision(parsePropertyValueInteger(offset));
                    break;
                case MarkdownLexemType::META_PROPERTY_type:
                    meta.setType(parsePropertyValueString(offset));
                    break;
                case MarkdownLexemType::META_PROPERTY_urgency:
                    meta.setUrgency(parsePropertyValueFraction(offset));
                    break;
                default:
                    done=true;
                    break;
                }
                if(lookahead(MarkdownLexemType::META_PROPERTY_DELIMITER, offset+1)) {
                    ++offset;
                }
                skipWhitespaces(offset);
            }
        }
    }

    skipEOL(offset);
    return false;
}

const MarkdownLexem* MarkdownParserSections::parsePropertyValue(size_t& offset)
{
    if(lookahead(MarkdownLexemType::META_NAMEVALUE_DELIMITER, offset+1)!=nullptr) {
        skipWhitespaces(++offset);
        const MarkdownLexem* result;
        if((result=lookahead(MarkdownLexemType::META_PROPERTY_VALUE, offset+1))!=nullptr) {
            skipWhitespaces(++offset);
            return result;
        }
    }
    return nullptr;
}

time_t MarkdownParserSections::parsePropertyValueTimestamp(size_t& offset)
{
    const MarkdownLexem* valueLexem = parsePropertyValue(offset);
    if(valueLexem != nullptr) {
        struct tm tm;
        // C-style initialization as GCC doesn't like {}
        memset(&tm, 0, sizeof tm);
        string* s = lexer.getText(valueLexem);
        datetimeFrom(s->c_str(), &tm);
        delete s;
        time_t result = datetimeSeconds(&tm);
        return result;
    }
    return 0;
}

int MarkdownParserSections::parsePropertyValueInteger(size_t& offset)
{
    const MarkdownLexem* valueLexem = parsePropertyValue(offset);
    if(valueLexem != nullptr) {
        string* s = lexer.getText(valueLexem);
        // IMPROVE do this in C++
        int result = atoi(s->c_str());
        delete s;
        return result;
    }
    return 0;
}

vector<string*>* MarkdownParserSections::parsePropertyValueTags(size_t& offset)
{
    const MarkdownLexem* valueLexem = parsePropertyValue(offset);
    if(valueLexem != nullptr) {
        string* s = lexer.getText(valueLexem);
        if(s!=nullptr) {
            if(s->size()) {
                vector<string*>* result = new vector<string*>();
                string buffer;
                bool ws{};
                for(size_t i=0; i<s->size(); i++) {
                    switch(s->at(i)) {
                    case ' ':
                        ws = true;
                        break;
                    case ',':
                        if(buffer.size()) {
                            result->push_back(new string(buffer));
                            buffer.clear();
                        }
                        ws = false;
                        break;
                    default:
                        if(ws && buffer.size()) {
                            buffer += ' ';
                        }
                        buffer += s->at(i);
                        ws = false;
                        break;
                    }
                }
                if(buffer.size()) {
                    result->push_back(new string(buffer));
                    buffer.clear();
                }
                delete s;
                return result;
            } else {
                delete s;
            }
        }
    }
    return nullptr;
}

string* MarkdownParserSections::parsePropertyValueString(size_t& offset)
{
    const MarkdownLexem* valueLexem = parsePropertyValue(offset);
    if(valueLexem != nullptr) {
        string* s = lexer.getText(valueLexem);
        if(s!=nullptr) {
            if(s->size()) {
                return s;
            } else {
                delete s;
            }
        }
    }
    return nullptr;
}

int MarkdownParserSections::parsePropertyValueFraction(size_t& offset)
{
    const MarkdownLexem* valueLexem = parsePropertyValue(offset);
    if(valueLexem != nullptr) {
        string* s = lexer.getText(valueLexem);
        if(s!=nullptr) {
            if(s->size()) {
                int r = (int)s->at(0) - '0';
                delete s;
                return r;
            } else {
                delete s;
            }
        }
    }
    return 0;
}

int MarkdownParserSections::parsePropertyValuePercent(size_t& offset)
{
    const MarkdownLexem* valueLexem = parsePropertyValue(offset);
    if(valueLexem != nullptr) {
        string* s = lexer.getText(valueLexem);
        if(s!=nullptr) {
            if(s->size()) {
                s->resize(s->size()-1);
                int result = atoi(s->c_str());
                delete s;
                return result;
            } else {
                delete s;
            }
        }
    }
    return 0;
}

vector<string*>* MarkdownParserSections::sectionBodyRule(size_t& offset)
{
    vector<string*>* result = new vector<string*>();
    string* s;
    const MarkdownLexem* l;
    while((l=lookaheadNot(MarkdownLexemType::SECTION,offset+1))!=nullptr) {
        ++offset;
        switch(l->getType()) {
        case MarkdownLexemType::LINE:
            if((s=lexer.getText(l))!=nullptr) {
                result->push_back(s);
            }
            // skip line's BR
            skipBr(offset);
            break;
        case MarkdownLexemType::BR:
            // empty line
            if((s=lexer.getText(l))!=nullptr) {
                result->push_back(s);
            }
            break;
        default:
            ; // IMPROVE skipping unknown lexems
        }
    }
    return result;
}

} /* namespace m8r */
