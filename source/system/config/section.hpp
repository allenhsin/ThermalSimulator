#ifndef BL_SECTION_HPP
#define BL_SECTION_HPP
/*!
 *\file section.hpp
 */
// Config Class
// Author: Charles Gruenwald III
#include <vector>
#include <map>
#include <string>

#include "boost/shared_ptr.hpp"

#include "key.hpp"

namespace config
{

    //
    class Section;
    typedef std::map < std::string, boost::shared_ptr<Section> > SectionList;
    typedef std::map < std::string, boost::shared_ptr<Key> > KeyList;


    /*! \brief Section: A configuration section entry
     * This class is used to hold a given section from a configuration.
     * It contains both a list of subsections as well as a list of keys.
     * The root of a configuration tree is of this class type.
     */
    class Section
    {
        public:
            friend class Config;

            Section(const std::string & name, bool case_sensitive);
            Section(Section const & parent, const std::string & name, bool case_sensitive);
            ~Section();

            //! Determine if this section is the root of the given configuration tree
            bool isRoot() const { return m_isroot; }

            /*! \brief returns true if the given name is a key within this section
             * \param name The name of the key
             * \param case_sensitive Whether or not the lookup should care about case
             * \return True if name is a key within this section
             */
            bool hasKey(const std::string &name) const;

            /*! \brief returns true if the given name is a subsection within this section
             * \param name The name of the subsection
             * \param case_sensitive Whether or not the lookup should care about case
             * \return True if name is a subsection within this section
             */
            bool hasSection(const std::string &name) const;

            //! SectionList() returns the list of subsections
            const SectionList & getSubsections() const { return m_subSections; }

            //! Returns the list of keys
            const KeyList & getKeys() const { return m_keys; }

            //! Returns the name of this section
            std::string getName() const { return m_name; }

            //! getKey() returns a key with the given name
            const Key & getKey(const std::string & name);

            /*! addSubSection() Add a subsection with the given name
             * \param name The name of the new subsection
             * \return A reference to the newly created subsection
             */
            Section & addSubsection(const std::string & name);

            /*! addKey() Add a key with the given name
             * \param name The name of the new key
             * \param value The value of the new key (as a string)
             * \return A reference to the newly created key
             */
            const Key & addKey(const std::string & name, const std::string & value);
            const Key & addKey(const std::string & name, const int value);
            const Key & addKey(const std::string & name, const double value);

            /*! getSection() Returns a reference to the section with the given name
             * \param name The name of the section we are trying to obtain
             * \return A reference to the named section, creating it if it doesn't exist
             */
            const Section & getSection(const std::string & name);

            //! getFullPath() Returns the path from the root to this section
            const std::string getFullPath() const;

            /*! getParent() Returns a reference to the parent section.
             * Note: The isRoot() method can be used to determine if a section is the root of the tree
             */
            const Section & getParent() const { return m_parent; }

        private:
            /*! clear() Clears out any sub-sections, used during a (re)load */
            void clear() { m_subSections.clear(); }
            Section & getSection_unsafe(const std::string & name);

            std::string m_name;

            //! A list of bl::config::Section subsections.
            SectionList m_subSections;

            //! A list of bl::config::Key keys in this current section.
            KeyList m_keys;

            //! A bl::config::Section parent
            Section const & m_parent;

            //! Whether or not this section is the root section of the config
            bool m_isroot;

            //! Whether or not key lookups are case-sensitive.
            bool m_case_sensitive;
    };

}//end of namespace bl

#endif //SECTION_HPP
