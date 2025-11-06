#include "XmlSerializer.h"

#include <ranges>
#include <fstream>

namespace ignite
{

namespace
{

constexpr char TAB_CHARACTER            = '\t';
constexpr char SLASH_CHARACTER          = '/';
constexpr char NEW_LINE_CHARACTER       = '\n';
constexpr char LESS_THAN_CHARACTER      = '<';
constexpr char WHITESPACE_CHARACTER     = ' ';
constexpr char GREATER_THAN_CHARACTER   = '>';
constexpr char QUESTION_MARK_CHARACTER  = '?';

} // Anonymous Namespace.

bool XmlSerializer::Serialize(const XmlDocument& document, const std::string& path)
{
    if (!document.mRootElement)
    {
        return false;
    }

    std::ofstream file(path, std::ios::out);
    if (file.fail())
    {
        return false;
    }

    RecursivelyOutputElement(file, document.mRootElement);

    file.close();
    return true;
}

XmlDocument XmlSerializer::Deserialize(const std::string& path)
{
    std::ifstream file(path, std::ios::in);

    if (file.fail())
    {
        return {};
    }

    std::string line;
    std::vector<std::string> lines;

    while (std::getline(file, line))
    {
        lines.push_back(line);
    }

    XmlElement* rootElement = nullptr;
    RecursivelyDeserializeDocument(lines, &rootElement);

    return XmlDocument{ rootElement };
}

void XmlSerializer::RecursivelyOutputElement(std::ofstream& file, const XmlElement* element, const uint8_t level /* = 0 */)
{
    // The current element would be the root element only if the level was 0. There should only ever exist a single element on level 0.
    const bool isRootElement = level == 0;

    const std::string indent(level, TAB_CHARACTER);

    std::string attributeString;
    for(auto& [attribute, attributeValue] : element->mAttributes)
    {
        attributeString += " " + attribute + "=\"" + attributeValue + "\"";
    }

    // This is for the very special case that the XmlDocument is only a single element with a (possibly empty) value string.
    // Handle this case first if applicable and return out as no further processing or recursion will be needed.
    if (isRootElement && element->mChildElements.empty())
    {
        file << LESS_THAN_CHARACTER << element->GetTagName() << attributeString << GREATER_THAN_CHARACTER << NEW_LINE_CHARACTER;
        file << TAB_CHARACTER << element->GetValue() << NEW_LINE_CHARACTER;
        file << LESS_THAN_CHARACTER << SLASH_CHARACTER << element->GetTagName() << GREATER_THAN_CHARACTER;
        return;
    }

    // If the tag is self-closing add a space and the slash character to the attribute string, so that once the line is closed, it would be self-closed.
    if (element->IsSelfClosing())
    {
        attributeString += WHITESPACE_CHARACTER;
        attributeString += SLASH_CHARACTER;
    }

    // Output the Opening tag to the file. Note this could be self-closing from above.
    file << indent << LESS_THAN_CHARACTER << element->GetTagName() << attributeString << GREATER_THAN_CHARACTER;

    // If this was a self-closing tag, we don't need to process further as there will be no value nor closing tag.
    if (element->IsSelfClosing())
    {
        file << NEW_LINE_CHARACTER;
        return;
    }

    // If the current element has no child elements, then we have reached the highest level and can finally output the (possibly empty) value of the element.
    if (element->mChildElements.empty())
    {
        file << element->GetValue();
    }
    else
    {
        // When we have more children, we add a new line so that when the child elements are processed, the next level will start on the next line.
        file << NEW_LINE_CHARACTER;
    }

    for (size_t child { 0 }; child < element->mChildElements.size(); ++child)
    {
        RecursivelyOutputElement(file, &element->mChildElements[child], level + 1);
    }

    // If the child elements where not empty, that means we need to indent the line.
    // We need this indent because this will be the closing tag of a element that has children, and hence the closing tag will be on its own separate line.
    // We do not need to do this when it has no children as in this case, the starting tag, value, and closing tag will be on the same line, and hence we don't need to indent for the closing tag.
    if (!element->mChildElements.empty())
    {
        file << indent;
    }

    // Finally output the closing tag to the file.
    file << LESS_THAN_CHARACTER << SLASH_CHARACTER << element->GetTagName() << GREATER_THAN_CHARACTER << NEW_LINE_CHARACTER;
}

uint32_t XmlSerializer::RecursivelyDeserializeDocument(const std::vector<std::string>& lines, XmlElement** previousElement, const uint32_t startingIndex /* = 0 */)
{
    /**
    * Deserialization will take the passed in file and load it into our definition of \c XmlDocument.
    * In order to load the doucment, we need to consider how the lines can be structured.
    *
    * 1)
    * Firstly lines can be self closing with attributes:
    *
    * <someTag attrib1="value1" ... attribN="valueN" />
    *
    * 2)
    * Secondly, lines can be opened and closed [could have attributes] on the same line with a value in-between:
    *
    * <someTag attrib1="value1" ... attribN="valueN" >ElementValue</someTag>
    *
    * 3)
    * Lastly, lines can be opened and closed [could have attributes] over muliple lines:
    *
    * <someTag attrib1="value1" ... attribN="valueN" >
    *     ElementValue
    * </someTag>
    */

    for (uint32_t i{ startingIndex }; i < lines.size(); ++i)
    {
        std::string tag, attributeString, value;
        switch (GetLineType(lines[i], tag, attributeString, value))
        {
        case LineType::OPEN_ONLY:
        {
            XmlElement* newElement;
            if (*previousElement != nullptr)
            {
                newElement = (*previousElement)->AddChild(tag, attributeString);
            }
            else
            {
                // *previousElement should only be null the very first iteration of RecursivelyDeserializeDocument, this is so we can create the new XmlElement for the document.
                *previousElement = new XmlElement{ tag, attributeString };
                newElement = *previousElement;
            }

            // Peek the future lines. If the future lines are just ordinary lines (none of the special LineTypes) value will be non-empty.
            const uint32_t nextLineWithAngleBracket = PeekNextLines(lines, i + 1, value);
            if (!value.empty())
            {
                // Since value is non-empty, that means we had a element that was over multiple lines.
                // We can set the new element's value to be the peeked value.
                newElement->SetValue(value);

                // We set 'i' to be the value returned from peek value as this is the next line with an angle bracket ['<']. This will be the closing tag of the current element.
                i = nextLineWithAngleBracket;
                // We set 'i' to the closing tag and then continue, so that the '++i' will move to the first line after the closing tag of the current element.
                continue;
            }

            // If the future lines are another layer of open tag, then we recursively call DeserializeDocument.
            i = RecursivelyDeserializeDocument(lines, &newElement, i + 1);
            break;
        }
        case LineType::CLOSE_ONLY:
            /** ============================================================================================
             * This case should occur when we are closing a tag that spans over multiple lines such as:
             * <foo>
             *     <child>Value</child>
             * </foo>                    //<- This should be the only time we hit this line.
             ** ============================================================================================= */

             // Return 'i' here so when we pop out of the recursion, the '++i' of the for-loop will move to the line after the closing tag.
            return i;
        case LineType::OPEN_CLOSE_SAME_LINE:
            (*previousElement)->AddChild(tag, attributeString, value);
            break;
        case LineType::SELF_CLOSING:
            (*previousElement)->AddChild(tag, attributeString);
            break;
        case LineType::VERSION_LINE:
            // The version line is the first line. Since its just like a comment, recursively search from the next starting index.
            return RecursivelyDeserializeDocument(lines, previousElement, startingIndex + 1);
        default:
            break;
        }
    }

    return -1;
}

uint32_t XmlSerializer::PeekNextLines(const std::vector<std::string>& lines, uint32_t startingIndex, std::string& value)
{
    while (lines[startingIndex].find('<') == std::string::npos)
    {
        value += lines[startingIndex++];
    }

    auto filtered = value | std::views::filter([](const char c) { return c != TAB_CHARACTER; });
    value = { filtered.begin(), filtered.end() };

    return startingIndex;
}

XmlSerializer::LineType XmlSerializer::GetLineType(const std::string& line, std::string& tag, std::string& attributeString, std::string& value)
{
    const size_t lessThanPosition       = line.find(LESS_THAN_CHARACTER);
    const size_t greaterThanPosition    = line.find(GREATER_THAN_CHARACTER);

    if (line[lessThanPosition + 1] == line[greaterThanPosition - 1] && line[greaterThanPosition - 1] == QUESTION_MARK_CHARACTER)
    {
        return LineType::VERSION_LINE;
    }

    const size_t whitespacePosition = line.find(WHITESPACE_CHARACTER, lessThanPosition);
    if (whitespacePosition == std::string::npos || whitespacePosition > greaterThanPosition)
    {
        tag = line.substr(lessThanPosition + 1, greaterThanPosition - lessThanPosition - 1);
    }
    else
    {
        attributeString = line.substr(whitespacePosition + 1, greaterThanPosition - whitespacePosition - 1);

        tag = line.substr(lessThanPosition + 1, whitespacePosition - lessThanPosition - 1);
    }

    const size_t slashPosition = line.find(SLASH_CHARACTER, lessThanPosition);
    if (slashPosition != std::string::npos && slashPosition < greaterThanPosition && slashPosition != lessThanPosition + 1)
    {
        return LineType::SELF_CLOSING;
    }

    const size_t lessThanPositionClose      = line.find(LESS_THAN_CHARACTER, greaterThanPosition + 1);
    const size_t greaterThanPositionClose   = line.find(GREATER_THAN_CHARACTER, greaterThanPosition + 1);

    value = line.substr(greaterThanPosition + 1, lessThanPositionClose - greaterThanPosition - 1);

    // std::string::npos is max value size, meaning that we don't need to check if less than is not npos since lessThan would be > than greaterThan if lessThan was npos.
    if (greaterThanPositionClose != std::string::npos && lessThanPositionClose < greaterThanPositionClose)
    {
        return LineType::OPEN_CLOSE_SAME_LINE;
    }

    if (slashPosition != std::string::npos && slashPosition < greaterThanPosition)
    {
        return LineType::CLOSE_ONLY;
    }

    return LineType::OPEN_ONLY;
}

} // Namespace ignite.
