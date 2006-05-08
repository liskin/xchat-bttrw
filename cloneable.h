#ifndef _CLONEABLE_H_INCLUDED
#define _CLONEABLE_H_INCLUDED

#include <stdexcept>

namespace std {
    /**
     * \brief The "clone not supported" exception. It means that the object of
     * this type has not overloaded the clone method so it is not able to
     * clone itself.
     */
    class clone_not_supported : public logic_error {
	public:
	    /**
	     * Contructor.
	     * \param classname The name of the class in question. Should be
	     * filled with classname from RTTI.
	     */
	    explicit clone_not_supported(const string &classname)
		: logic_error("Clone not supported by "	+ classname) { }
    };

    /**
     * \brief A cloneable interface. Acts just like that in Java.
     */
    class cloneable {
	public:
	    /**
	     * Clone the object. This method, unless overloaded, just throws
	     * the clone_not_supported exception.
	     * \return Nothing, but generally the pointer to the cloned
	     * object.
	     */
	    virtual cloneable * clone() {
		throw clone_not_supported(typeid(*this).name());
	    }

	    virtual ~cloneable() { }
    };
}

#endif /* _CLONEABLE_H_INCLUDED */
