#pragma once

#include "XmlElement.h"

namespace ignite
{

/**
 * @class XmlDocument: A wrapper that holds a reference to the XmlDocument that is loaded from a file.  <br/>
 * The document holds a reference to the top root element which is the first XML tag in the file.       <br/>
 *                                                                                                      <br/>
 * An example of an \c XmlDocument:                                                                     <br/>
 * --------------------------------------------------                                                   <pre>
 * <fooTag>
 *     <barTag>
 *         SomeValueInTheTag
 *     </barTag>
 * </fooTag>
 * --------------------------------------------------                                                   </pre>
 *                                                                                                      <br/>
 * In this example, the root element would be the 'fooTag' element.                                     <br/>
 * @see XmlElement
 */
class XmlDocument
{
friend class XmlSerializer;
public:
    explicit inline XmlDocument(XmlElement* rootElement)
        : mRootElement(rootElement)
    { /* Empty. */ }
    XmlDocument() = default;
    inline ~XmlDocument() { delete mRootElement; }

    XmlDocument(const XmlDocument&)               = delete;
    XmlDocument(XmlDocument&&)                    = default;
    XmlDocument& operator=(XmlDocument&&)         = delete;
    XmlDocument& operator=(const XmlDocument&)    = delete;

    /**
     * @brief Get the root element [the first tag] of the XML document.
     * @see \c XmlElement
     * @return Get the root element of the XML document.
     */
    [[nodiscard]] inline const XmlElement* GetRootElement() const { return mRootElement; }
    /**
     * @brief Get if the queried \c XmlDocument is valid or not.
     * @return Returns \c true if the \c XmlDocument is valid, false otherwise.
     */
    [[nodiscard]] inline bool IsValidDocument() const { return mRootElement != nullptr; }

    [[nodiscard]]bool operator==(const XmlDocument& other) const { return *mRootElement == *other.mRootElement; }
private:
    XmlElement* mRootElement = nullptr;
};

} // Namespace ignite.
