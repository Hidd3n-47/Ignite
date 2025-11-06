#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace ignite
{

/**
 * @class XmlElement: A class representing a single element in an XML document.                                                                                                             <br/>
 * A node by definition will be a single tag that is present in the document.                                                                                                               <br/>
 * Some properties of elements:                                                                                                                                                             <br/>
 * - Elements can have values (need to have open and closing tags)                                                                                                                          <br/>
 * - Elements can be self-closing (i.e. it has no body or no value)                                                                                                                         <br/>
 * - Elements can have attributes: Attributes are values passed with the tag definition.                                                                                                    <br/>
 *                                                                                                                                                                                          <br/>
 * Attributes and tags should be unique, whilst the values within the tags/attributes can be non-unique.                                                                                    <br/>
 *                                                                                                                                                                                          <br/>
 * An example of an \c XmlNode:                                                                                                                                                             <br/>
 * -------------------------------------------------------                                                                                                                                  <pre>
 * <fooTag>                                               //<- This is an element that contains another element, specifically 'barTag' element.
 *     <barTag>                                           //<- This is an element that contains no other element, but has a value within of 'SomeValueInTheTag'.
 *         SomeValueInTheTag                              //<- This is the value of that is contained in the tag 'barTag', i.e. the value of the element.
 *     </barTag>                                          //<- This is the closing tag of the element 'barTag'.
 *     <SelfClosingTagWithAttrib attrib="AttribValue" />  //<- This is a self-closing element that contains no other element or value, but has an attribute 'attrib' with value 'AttribValue'.
 * </fooTag>
 * -------------------------------------------------------                                                                                                                                  </pre>
 */
class XmlElement
{
    friend class XmlDocument;
    friend class XmlSerializer;
public:
    inline XmlElement() = default;
    /**
     * @brief Constructor to create \c XmlElement with a tag, attributeString and a value.
     * @param tag The tag of the \c XmlElement.
     * @param attributeString The [unprocessed] attribute string of the \c XmlElement. Defaulted to the empty string.
     * @param value The value of the \c XmlElement. Defaulted to the empty string.
     */
    explicit XmlElement(std::string tag, const std::string& attributeString = "", std::string value = "");

    /**
     * Get the name of the tag.
     * @return The name of the tag.
     */
    [[nodiscard]] inline const std::string& GetTagName() const { return mTag; }
    /**
     * Get the value of the tag.
     * @return The value of the \c XmlElement
     */
    [[nodiscard]] inline const std::string& GetValue() const { return mValue; }
    /**
     * @brief Get if the tag is self-closing, i.e. doesn't have a value or child element.
     * @return \c true if the tag is self-closing (no value nor child element), \c false otherwise.
     */
    [[nodiscard]] inline bool IsSelfClosing() const { return mChildElements.size() == 0 && mValue == ""; }

    /**
     * @brief Get the attribute value for the given attribute name. Will return empty string if no attribute with the given name is found.
     * @param attributeName The name of the attribute whose value is being retrieved.
     * @return The value of the attribute with the queried name. If an attribute with the queried name is not found, the empty string is returned.
     */
    [[nodiscard]] std::string GetAttributeValue(const std::string& attributeName) const;

    /**
     * @brief Get the child elements of this element.
     * @return The vector of child elements of this element.
     */
    [[nodiscard]] inline const std::vector<XmlElement>& GetChildElements() const { return mChildElements; }

    /**
     * @brief Add an attribute to the element.
     * @param attributeName The name of the attribute being added. The name should be unique.
     * @param attributeValue The value of the attribute being added.
     * @return \c true if the attribute was added correctly, \c false when not - example, the element already contains an attribute with the given value.
     */
    [[nodiscard]] bool AddAttribute(const std::string& attributeName, const std::string& attributeValue);

    /**
     * @brief Add a new child element to this element.
     * @param element A reference to the element being moved into the child elements vector.
     */
    inline void AddChild(XmlElement& element) { mChildElements.emplace_back(std::move(element)); }

    /**
     * @brief Add a new child element to this element.
     * @param tag The value of the child element tag.
     * @param attributeString The attribute string of the child.
     * @param value The value of the child element.
     * @return A pointer to the newly added child element.
     */
    XmlElement* AddChild(const std::string& tag, const std::string& attributeString = "", const std::string& value = "");

    /**
     * @brief Set the element to have a given value.
     * @param value The element value.
     */
    inline void SetValue(std::string value) { mValue = std::move(value); }

    [[nodiscard]] bool operator==(const XmlElement& other) const;
private:
    std::string mTag;
    std::string mValue;

    std::vector<XmlElement> mChildElements;
    std::unordered_map<std::string, std::string> mAttributes;

    /**
     * A method used to take in an attribute string and process it into different attributes.<br/>
     * Example string 'attrib1="value1", ..., attribN="valueN"' -> { 'attrib1': 'value1', ..., 'attribN': 'valueN' }
     * @param attributeString The attribute string value that needs to be processed into attributes.
     */
    void ProcessAttributesFromString(const std::string& attributeString);
};

} // Namespace ignite.
