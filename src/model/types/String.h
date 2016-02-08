#ifndef FUZZY_MODEL_TYPES_STRING
#define FUZZY_MODEL_TYPES_STRING

#include <string>
#include <memory>

#include "./Base.h"

namespace model {
	namespace types {
		class String : public Base {
		private:
			friend class TypeSerialiser;
			std::string _value;
			MemberSerialiser::DynamicStringMember _valueWrapper;
			MemberSerialiser _memberSerialiser;
			
			void initMemberSerialiser()
			{
				_memberSerialiser.addDynamicMember(&_valueWrapper);
			}
			
		public:
			String() :Base(100), _value(), _valueWrapper(_value)
			{
				initMemberSerialiser();
			}
			
			String(const std::string value) : Base(100), _value(value), _valueWrapper(_value)
			{
				initMemberSerialiser();
			}
			
			String(const std::string &value, unsigned char confidence) : Base(confidence), _value(value), _valueWrapper(_value)
			{
				initMemberSerialiser();
			}
			
			virtual ~String() {}

			std::string value() const { return _value; }

			virtual Subtype subtype() const
			{
				return Subtype::TypeString;
			}

			virtual std::shared_ptr<Base> Clone() override {
				return std::make_shared<String>(_value, _confidence);
			}

			virtual std::string logString() const override
			{
				return std::string("String(\"") + _value + std::string("\", ")
					+ std::to_string(confidence()) + std::string(")");
			}

			virtual std::string toString() const override {
				return _value;
			}

			// Inherited via Base
			virtual bool Equals(const std::string val) const override {
				return _value == val;
			}

		protected:
			virtual std::size_t serialiseSubclass(Serialiser &serialiser) const
			{
				return Base::serialiseSubclass(serialiser) + _memberSerialiser.serialiseDynamicMembers(serialiser);
			}

			String(const char* &serialisedData) : Base(serialisedData), _value(), _valueWrapper(_value)
			{
				initMemberSerialiser();
				serialisedData += _memberSerialiser.unserialiseDynamicMembers(serialisedData);
			}
		};
	}
}


#endif // !FUZZY_MODEL_TYPES_STRING
