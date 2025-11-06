#pragma once

#include "XmlDocument.h"
#include <vector>
#include <string>

namespace ignite
{

class XmlSerializer
{
public:
    /**
     * @brief Serialize the given \c XmlDocument to the passed in path.
     * @see \c XmlDocument
     * @param document The \c XmlDocument that is being serialized to disk.
     * @param path The path the \c XmlDocument is being serialized to.
     * @return \c true if the document was serialized correctly, \c false otherwise.
     */
    static bool Serialize(const XmlDocument& document, const std::string& path);

    /**
     * @brief Deserialize a given path into an \c XmlDocument.
     * @see \c XmlDocument
     * @param path The path of the \c XmlDocument that is being deserialized.
     * @return The \c XmlDocument that was deserialized. If the document fails to deserialize, the returned document will not have a root element [i.e. @code rootElement == nullptr@endcode]
     */
    static XmlDocument Deserialize(const std::string& path);
private:
    /**
     * @brief Recursive method to output a element. Will call itself to output the child elements of the \c XmlElement.
     * @see \c XmlElement
     * @param file The file stream the \c XmlElement is being output to.
     * @param element The \c XmlElement being output to disk.
     * @param level The level of recursion. By default, starts at 0, will call itself with level + 1.
     */
    static void RecursivelyOutputElement(std::ofstream& file, const XmlElement* element, const uint8_t level = 0);

    /**
     * @breif Recursive method to deserialize the \c XmlDocument
     * @see \c XmlDocument
     * @param lines The vector of the document lines in the XML file on disk.
     * @param previousElement
     * @param startingIndex The starting index that line vector iteration will begin.<br/>
     *          By default, starts at 0, will increase each time we call the method recursively to prevent reparsing the same line from previous recursion levels.
     * @return The index of the line vector that has been processed up to. This way when returning out of the recursion, can skip over the already processed lines from the recursion.
     */
    static uint32_t RecursivelyDeserializeDocument(const std::vector<std::string>& lines, XmlElement** previousElement, const uint32_t startingIndex = 0);

    /**
     * @brief Peek the next lines until a line is found with an angle bracket ['<']. This helps find tags that have a value split over multiple lines, such as: <br/>
     * ---------------------------<pre>
     * <SomeElement>
     *     SomeElementValue
     * </SomeElement>
     * ---------------------------</pre>
     * @param lines The vector of the document lines in the XML file on disk.
     * @param startingIndex The starting index that lines will be peeked from [inclusive].
     * @param value The value obtained from peeking [the value excludes new lines and tab characters, i.e. just the raw value - "SomeElementValue" in above example].<br/>
     *          If the value is empty after calling this method, that means the next lines contain a special \c LineType, else, if its non-empty, the peeked lines just result in multi-lined element, as per example.
     * @return The index of the next line that contains an angle bracket in ['<'].
     */
    static uint32_t PeekNextLines(const std::vector<std::string>& lines, uint32_t startingIndex, std::string& value);

    /**
     * @enum LineType Enum class to represent the line type.
     */
    enum class LineType : uint8_t
    {
        OPEN_ONLY,              // Line only contains open tag    : <fooTag attrib1="1", ..., attribN="N">
        CLOSE_ONLY,             // Line only contains closing tag : </fooTag>
        SELF_CLOSING,           // Line is a self-closing tag     : <fooTag attrib1="1", ..., attribN="N" />
        OPEN_CLOSE_SAME_LINE,   // Line opens and closes same line: <fooTag attrib1="1", ..., attribN="N">Value</fooTag>
        VERSION_LINE,           // Line is specifying version XML : <?xml version="1.0"?>
    };

    /**
     * @brief Get the \c LineType of the passed in \c line.
     * @see \c LineType
     * @param line The \c line that is being queried to check the \c LineType.
     * @param tag A string reference, will be set to the tag of the passed in \c line.
     * @param attributeString A string reference, will be set to the [unprocessed] attribute string of the passed in \c line. Will be set to empty string if there is no attribute.
     * @param value A string reference, will be set to the value of the passed in \c line. Will be set to the empty string if there is no value.
     * @return The \c LineType of the passed in \c line.
     */
    static LineType GetLineType(const std::string& line, std::string& tag, std::string& attributeString, std::string& value);
};

} // Namespace ignite.
