#include "XmlElement.h"

#include <stdexcept>

namespace ignite
{

namespace
{
constexpr char EQUAL        = '=';
constexpr char DOUBLE_QUOTE = '"';
} // Anonymous namespace.

XmlElement::XmlElement(std::string tag, const std::string& attributeString /* = "" */, std::string value /* = "" */)
    : mTag(std::move(tag))
    , mValue(std::move(value))
{
    ProcessAttributesFromString(attributeString);
}

std::string XmlElement::GetAttributeValue(const std::string& attributeName) const
{
    if (!mAttributes.contains(attributeName))
    {
        return "";
    }

    return mAttributes.at(attributeName);
}

bool XmlElement::AddAttribute(const std::string& attributeName, const std::string& attributeValue)
{
    if (mAttributes.contains(attributeName))
    {
        return false;
    }

    mAttributes[attributeName] = attributeValue;
    return true;
}

XmlElement* XmlElement::AddChild(const std::string& tag, const std::string& attributeString /* = "" */, const std::string& value /* = "" */)
{
    mChildElements.emplace_back(tag, attributeString, value);

    XmlElement* back = &mChildElements.back();

    return back;
}

bool XmlElement::operator==(const XmlElement& other) const
{
    // Check if the element's values are equal.
    if (mChildElements.size()   != other.mChildElements.size() ||
        mTag                    != other.mTag ||
        mValue                  != other.mValue ||
        mAttributes.size()      != other.mAttributes.size())
    {
        return false;
    }

    bool equal = true;

    // Check if the elements attributes are the same.
    for (const auto& [attributeName, attributeValue] : mAttributes)
    {
        try
        {
            equal &= attributeValue == other.mAttributes.at(attributeName);
        }
        catch (const std::out_of_range&)
        {
            return false;
        }
    }

    // Check if the children elements are equal.
    for (uint32_t i{ 0 }; i < mChildElements.size(); ++i)
    {
        equal &= mChildElements[i] == other.mChildElements[i];
    }

    return equal;
}

void XmlElement::ProcessAttributesFromString(const std::string& attributeString)
{
    // To prevent having to reallocate strings, we will iterate over the string and not remove the processed strings.
    // Offset will be the index to the next unprocessed part of the string.
    uint32_t offset         = 0;
    size_t equalPosition    = attributeString.find(EQUAL);

    while (equalPosition != std::string::npos)
    {
        // Find the double quote's after the offset.
        const size_t startingQuotePosition  = attributeString.find(DOUBLE_QUOTE, offset);
        const size_t endingQuotePosition    = attributeString.find(DOUBLE_QUOTE, startingQuotePosition + 1);

        // The attribute name will be the text before the equal sign.
        const std::string attributeName     = attributeString.substr(offset, equalPosition - offset);
        // The attribute value will be the text between the quotes. Subtract 2 at the end to remove the two quotes as the quotes are after the equal position.
        const std::string attributeValue    = attributeString.substr(startingQuotePosition + 1, endingQuotePosition - equalPosition - 2);

        mAttributes[attributeName] = attributeValue;

        // Add 2 to the offset so we skip of the double quote and also the space that is between attributes.
        // This is safe because even if offset > str.len, we won't enter the while loop due to not finding an equal after this position.
        offset = static_cast<uint32_t>(endingQuotePosition + 2);
        equalPosition = attributeString.find(EQUAL, offset);
    }
}

} // Namespace ignite.
